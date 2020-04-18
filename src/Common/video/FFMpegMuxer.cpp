#include "FFMpegMuxer.h"

#include <cstring>

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QtConcurrent/QtConcurrent>
#include <QMutex>

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class BaseOutputStream
{
public:
    BaseOutputStream()
        : stream(nullptr),
          frame(nullptr),
          tempFrame(nullptr)
    {

    }

    virtual ~BaseOutputStream()
    {
        avcodec_free_context(&stream->codec);
        //av_frame_free(&frame);
        //av_frame_free(&tempFrame);
    }

    AVStream *stream;
    AVFrame *frame;
    AVFrame *tempFrame;     // used to rescale/resample
};

class FFMpegMuxer::VideoOutputStream : public BaseOutputStream
{
public:
    VideoOutputStream()
        : swsContext(nullptr)
    {

    }

    ~VideoOutputStream()
    {
        if (swsContext)
            sws_freeContext(swsContext);

    }

    SwsContext *swsContext; // used to rescale images
};

class FFMpegMuxer::AudioOutputStream : public BaseOutputStream
{
public:
    AudioOutputStream()
        : swrContext(nullptr),
          samplesCount(0)
    {

    }

    ~AudioOutputStream()
    {
        qDebug() << "closing audio outputstream";
        if (swrContext)
            swr_free(&swrContext);
    }

    SwrContext *swrContext; // resampler
    int samplesCount;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

FFMpegMuxer::FFMpegMuxer(QObject *parent) :
      QObject(parent),
      encodeVideo(false),
      encodeAudio(false),
      videoPts(0),
      encodedFrames(0),
      audioStream(nullptr),
      videoResolution(QSize(320, 240)),
      videoFrameRate(25),
      videoBitRate(static_cast<uint>(FFMpegMuxer::VideoQualityLow)),
      initialized(false),
      startNewIntervalRequested(false)
{
    av_register_all();

    av_log_set_level(AV_LOG_QUIET); // disabling ffmpeg encoder messages

    threadPool.setMaxThreadCount(1);
}

void FFMpegMuxer::initialize()
{
    //QMutexLocker locker(&mutex);

    encodeAudio = encodeVideo = false;
    audioStream = nullptr;

    videoPts = 0;
    encodedFrames = 0;
}

FFMpegMuxer::~FFMpegMuxer()
{
    finish();
}

void FFMpegMuxer::finish()
{
    QtConcurrent::run(&threadPool, this, &FFMpegMuxer::finishCurrentInterval);

}

void FFMpegMuxer::encodeImage(const QImage &image, bool async)
{
    // encoding in a separated thread

    auto lambda = [=](){

        if (startNewIntervalRequested) {
            if (prepareToEncodeNewInterval())
                startNewIntervalRequested = false;
        }

        if (encodeVideo && !image.isNull())
            encodeVideo = !doEncodeVideoFrame(image);
    };

    if (async)
        QtConcurrent::run(&threadPool, lambda);
    else
        lambda();
}

void FFMpegMuxer::encodeAudioFrame()
{
    //QMutexLocker locker(&mutex);
    //if (!initialized)
    //    return;

    //if (encodeAudio)
    //    encodeAudio = !doEncodeAudioFrame();
}

void FFMpegMuxer::startNewInterval()
{
    startNewIntervalRequested = true;
}

bool FFMpegMuxer::prepareToEncodeNewInterval()
{
    if(initialized)
        finishCurrentInterval();

    initialized = false;

    initialize();

    // Add the audio and video streams using the default format codecs and initialize the codecs.

    AVDictionary * opts = nullptr;
    AVCodecID codecID = AV_CODEC_ID_H264;
    encodeVideo = addVideoStream(codecID, &opts);

    encodeAudio = false;

    // Now that all the parameters are set, we can open the audio and video codecs and allocate the necessary encode buffers.
    if (encodeVideo)
        if(!openVideoCodec(codec, &opts))
            return false;

    initialized = true;

    return initialized;
}

void FFMpegMuxer::finishCurrentInterval()
{
    if (!initialized)
        return;

    if (!codecContext || (codecContext && !avcodec_is_open(codecContext))) {
        return;
    }

    if (audioStream) {
        if (!audioStream->stream->codec || !avcodec_is_open(audioStream->stream->codec)) {
            qCritical() << "audio codec is null or not opened!";
            return;
        }
    }

    // drain non encoded frames in last interval
    bool finished = false;
    do {
        finished = doEncodeVideoFrame(QImage());
    }
    while(!finished);

    audioStream.reset(nullptr);

    initialized = false;
    encodedFrames = 0;

    if (frame)
        av_frame_free(&frame);

    if (tempFrame)
        av_frame_free(&tempFrame);

    if (codecContext)
        avcodec_free_context(&codecContext);

    emit encodingFinished();
}

bool FFMpegMuxer::addVideoStream(AVCodecID codecID, AVDictionary **opts)
{
    // find the encoder
    codec = avcodec_find_encoder(codecID);
    if (!codec) {
        qCritical() << "Could not find encoder for " << avcodec_get_name(codecID);
        return false;
    }

    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        qCritical() << "Could not alloc an encoding context";
        return false;
    }

    codecContext->codec_id = codecID;
    codecContext->bit_rate = videoBitRate;
    codecContext->rc_max_rate = videoBitRate;
    codecContext->rc_buffer_size = videoBitRate;
    codecContext->width    = videoResolution.width(); // Resolution must be a multiple of two.
    codecContext->height   = videoResolution.height();

    /** timebase: This is the fundamental unit of time (in seconds) in terms
         * of which frame timestamps are represented. For fixed-fps content,
         * timebase should be 1/framerate and timestamp increments should be
         * identical to 1. */
    codecContext->time_base = AVRational{ 1, static_cast<int>(videoFrameRate) };

    codecContext->gop_size = 30; // emit one intra frame every N frames at most
    codecContext->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codecContext->codec_id == AV_CODEC_ID_H264) {
        int ret = av_dict_set(opts, "preset", "veryfast", 0);
        if (ret != 0) {
            qCritical() << "Error setting h264 preset" << av_error_to_qt_string(ret) << ret;
            return false;
        }
    }

    return true;
}


AVFrame * FFMpegMuxer::allocAudioFrame(enum AVSampleFormat sampleFormat, uint64_t channelLayout, int sampleRate, int nbSamples)
{
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        qCritical() << "Error allocating an audio frame";
        return nullptr;
    }

    frame->format = sampleFormat;
    frame->channel_layout = channelLayout;
    frame->sample_rate = sampleRate;
    frame->nb_samples = nbSamples;

    if (nbSamples) {
        int ret = av_frame_get_buffer(frame, 0);
        if (ret < 0) {
            qCritical() << "Error allocating an audio buffer";
            return nullptr;
        }
    }

    return frame;
}

void FFMpegMuxer::openAudioCodec(AVCodec *codec)
{
    Q_UNUSED(codec);
    AVCodecContext *codecContext = audioStream->stream->codec;

    /* open it */
//    av_dict_copy(&opt, options, 0);
//    int ret = avcodec_open2(codecContext, codec, &opt);
//    av_dict_free(&opt);
//    if (ret < 0) {
//        qCritical() << "Could not open audio codec: " << av_err2str(ret);
//        exit(1);
//    }

//    /* init signal generator */
//    ost->t     = 0;
//    ost->tincr = 2 * M_PI * 110.0 / c->sample_rate;
//    /* increment frequency by 110 Hz per second */
//    ost->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

    int nbSamples;
    if (codecContext->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
        nbSamples = 10000;
    else
        nbSamples = codecContext->frame_size;

    audioStream->frame     = allocAudioFrame(codecContext->sample_fmt, codecContext->channel_layout,
                                       codecContext->sample_rate, nbSamples);
    audioStream->tempFrame = allocAudioFrame(AV_SAMPLE_FMT_S16, codecContext->channel_layout,
                                       codecContext->sample_rate, nbSamples);

    /* copy the stream parameters to the muxer */
    int ret = avcodec_parameters_from_context(audioStream->stream->codecpar, codecContext);
    if (ret < 0) {
        qCritical() << "Could not copy the stream parameters";
        return;
    }

    audioStream->swrContext = swr_alloc();     /* create resampler context */
    if (!audioStream->swrContext) {
        qCritical() << "Could not allocate resampler context";
        return;
    }

    /* set options */
    av_opt_set_int       (audioStream->swrContext, "in_channel_count",   codecContext->channels,       0);
    av_opt_set_int       (audioStream->swrContext, "in_sample_rate",     codecContext->sample_rate,    0);
    av_opt_set_sample_fmt(audioStream->swrContext, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int       (audioStream->swrContext, "out_channel_count",  codecContext->channels,       0);
    av_opt_set_int       (audioStream->swrContext, "out_sample_rate",    codecContext->sample_rate,    0);
    av_opt_set_sample_fmt(audioStream->swrContext, "out_sample_fmt",     codecContext->sample_fmt,     0);

    if ((ret = swr_init(audioStream->swrContext)) < 0) {     /* initialize the resampling context */
        qCritical() << "Failed to initialize the resampling context";
    }
}

/**
 * encode one audio frame and send it to the muxer
 * return true when encoding is finished, false otherwise
 */
bool FFMpegMuxer::doEncodeAudioFrame()
{
    AVPacket packet = AVPacket(); // avoiding initializing with {0} because GCC is emiting a warning -> data and size must be 0;
    AVFrame *frame;

    av_init_packet(&packet);
    AVCodecContext *codecContext = audioStream->stream->codec;

    frame = nullptr; // TODO convert SamplesBuffer to frame

    if (frame) {
        /* convert samples from native format to destination codec format, using the resampler */
        /* compute destination number of samples */
        int dst_nb_samples = av_rescale_rnd(swr_get_delay(audioStream->swrContext, codecContext->sample_rate) + frame->nb_samples,
                                        codecContext->sample_rate, codecContext->sample_rate, AV_ROUND_UP);
        av_assert0(dst_nb_samples == frame->nb_samples);

        /* when we pass a frame to the encoder, it may keep a reference to it
         * internally;
         * make sure we do not overwrite it here
         */
        int ret = av_frame_make_writable(audioStream->frame);
        if (ret < 0)
            exit(1);

        /* convert to destination format */
        ret = swr_convert(audioStream->swrContext, audioStream->frame->data, dst_nb_samples, (const uint8_t **)frame->data, frame->nb_samples);
        if (ret < 0) {
            qCritical() << "Error while converting";
            exit(1);
        }
        frame = audioStream->frame;

        frame->pts = av_rescale_q(audioStream->samplesCount, AVRational{1, codecContext->sample_rate}, codecContext->time_base);
        audioStream->samplesCount += dst_nb_samples;
    }

    int gotPacket;
    int ret = avcodec_encode_audio2(codecContext, &packet, frame, &gotPacket);
    if (ret < 0) {
        qCritical() << "Error encoding audio frame: " << av_error_to_qt_string(ret);
        exit(1);
    }

    if (gotPacket) {
//        ret = writeFrame(&codecContext->time_base, &packet);
//        if (ret < 0) {
//            qCritical() << "Error while writing audio frame: " << av_error_to_qt_string(ret);
//            exit(1);
//        }
    }

    return (frame || gotPacket) ? false : true;
}

/**************************************************************/
/* video output */

AVFrame *FFMpegMuxer::allocPicture(enum AVPixelFormat pixelFormat, int width, int height)
{
    AVFrame *picture = av_frame_alloc();
    if (!picture)
        return NULL;

    picture->format = pixelFormat;
    picture->width  = width;
    picture->height = height;

    /* allocate the buffers for the frame data */
    int ret = av_frame_get_buffer(picture, 32);
    if (ret < 0) {
        qCritical() << "Could not allocate frame data.";
    }

    return picture;
}

bool FFMpegMuxer::openVideoCodec(AVCodec *codec, AVDictionary **opts)
{

    int ret = avcodec_open2(codecContext, codec, opts); /* open the codec */
    if (ret < 0) {
        qCritical() << "Could not open video codec: " << av_error_to_qt_string(ret) << ret;
        return false;
    }

    /* allocate and init a re-usable frame */
    frame = allocPicture(codecContext->pix_fmt, codecContext->width, codecContext->height);

    if (!frame) {
        qCritical() << "Could not allocate video frame";
        return false;
    }

    /* If the output format is not YUV420P, then a temporary YUV420P picture is needed too. It is then converted to the required output format. */
    tempFrame = NULL;
    if (codecContext->pix_fmt != AV_PIX_FMT_YUV420P) {
        tempFrame = allocPicture(AV_PIX_FMT_YUV420P, codecContext->width, codecContext->height);
        if (!tempFrame) {
            qCritical() << "Could not allocate temporary picture";
            return false;
        }
    }

    return true;
}

void FFMpegMuxer::imageToYuvPicture(const QImage &image, AVFrame *picture, int width, int height)
{

    //avoiding crash when camera preview is resized
    width = qMin(width, image.width());
    height = qMin(height, image.height());

    // Preparing the buffer to get YUV420P data
    int size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, width, height, 1);
    uint8_t *pic_dat = (uint8_t *) av_malloc(size);

    // Transforming data from RGB to YUV420P
    RGBtoYUV420P(image.bits(), pic_dat, image.depth()/8, true, width, height);

    picture->quality = 0;

    // Filling AVFrame with YUV420P data
    av_image_fill_arrays(picture->data, picture->linesize, pic_dat, AV_PIX_FMT_YUV420P, width, height, 1);
}

void FFMpegMuxer::RGBtoYUV420P(const uint8_t *bufferRGB, uint8_t *bufferYUV, uint rgbIncrement, bool swapRGB, int width, int height)
{
    const unsigned iPlaneSize = width * height;
    const unsigned iHalfWidth = width >> 1;

    // get pointers to the data
    uint8_t *yplane  = bufferYUV;
    uint8_t *uplane  = bufferYUV + iPlaneSize;
    uint8_t *vplane  = bufferYUV + iPlaneSize + (iPlaneSize >> 2);
    const uint8_t *bufferRGBIndex = bufferRGB;

    int iRGBIdx[3];
    iRGBIdx[0] = 0;
    iRGBIdx[1] = 1;
    iRGBIdx[2] = 2;
    if (swapRGB)  {
        iRGBIdx[0] = 2;
        iRGBIdx[2] = 0;
    }

    for (int y = 0; y < (int) height; y++) {
         uint8_t *yline  = yplane + (y * width);
         uint8_t *uline  = uplane + ((y >> 1) * iHalfWidth);
         uint8_t *vline  = vplane + ((y >> 1) * iHalfWidth);

         for (int x=0; x<width; x+=2) {
              RGBtoYUV(bufferRGBIndex[iRGBIdx[0]], bufferRGBIndex[iRGBIdx[1]], bufferRGBIndex[iRGBIdx[2]], *yline, *uline, *vline);
              bufferRGBIndex += rgbIncrement;
              yline++;
              RGBtoYUV(bufferRGBIndex[iRGBIdx[0]], bufferRGBIndex[iRGBIdx[1]], bufferRGBIndex[iRGBIdx[2]], *yline, *uline, *vline);
              bufferRGBIndex += rgbIncrement;
              yline++;
              uline++;
              vline++;
         }
    }
}

void FFMpegMuxer::fillFrameWithImageData(const QImage &image)
{
    /* when we pass a frame to the encoder, it may keep a reference to it internally; make sure we do not overwrite it here */
    if (av_frame_make_writable(frame) < 0) {
        qCritical() << "frame not writable";
        return;
    }

    if (codecContext->pix_fmt != AV_PIX_FMT_YUV420P) { /* need image convertion? as we only generate a YUV420P picture, we must convert it to the codec pixel format if needed */
        if (!swsContext) {
            swsContext = sws_getContext(image.width(), image.height(), AV_PIX_FMT_YUV420P, codecContext->width, codecContext->height, codecContext->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
            if (!swsContext) {
                qCritical() << "Could not initialize the conversion context";
                return;
            }
        }
        imageToYuvPicture(image, tempFrame, codecContext->width, codecContext->height);
        sws_scale(swsContext, (const uint8_t * const *)tempFrame->data, tempFrame->linesize, 0, codecContext->height, frame->data, frame->linesize);
    } else {
        imageToYuvPicture(image, frame, codecContext->width, codecContext->height);
    }

    frame->pts = videoPts++;
}

/*
 * encode one video frame and send it to the muxer
 * return true when encoding is finished, false otherwise
 */
bool FFMpegMuxer::doEncodeVideoFrame(const QImage &image)
{
    if (!initialized)
        return false;

    if (!codec)
        return false;

    if (!frame)
        return false;

    if(avcodec_is_open(codecContext) <= 0) {
        qCritical() << "Codec is not opened!";
        return false;
    }

    if (!image.isNull())
        fillFrameWithImageData(image);

    // send the image to encoder, send nullpr if finishing
    int ret = avcodec_send_frame(codecContext, (!image.isNull()) ? frame : nullptr);

    if (!image.isNull()) {
        // releasing the allocated memory - fix https://github.com/elieserdejesus/JamTaba/issues/951
        av_freep(frame);

        if (ret != 0 && ret != AVERROR_EOF) {
            qCritical() << "Error encoding video frame: " << av_error_to_qt_string(ret) << ret;
            return false;
        }
    }

    // get the encoded packet
    AVPacket packet = AVPacket();// avoiding {0} initializer because GCC is emitting warning;
    packet.size = 0;
    packet.data = nullptr;
    packet.stream_index = 0; // always using the first stream

    av_init_packet(&packet);

    ret = avcodec_receive_packet(codecContext, &packet);

    if (ret == 0 || ret == AVERROR(EAGAIN)) {
        if (ret == 0) { // we have a decoded frame?
            QByteArray encodedBytes(reinterpret_cast<const char*>(packet.data), packet.size);
            bool isFirstPacket = encodedFrames == 0;
            emit dataEncoded(encodedBytes, isFirstPacket);
            encodedFrames++;
            av_packet_unref(&packet);
        }
        av_free_packet(&packet);

        return false;
    }
    av_free_packet(&packet);

    return true;
}
