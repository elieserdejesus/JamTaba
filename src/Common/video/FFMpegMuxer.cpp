#include "FFMpegMuxer.h"

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
        av_frame_free(&frame);
        av_frame_free(&tempFrame);
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

FFMpegMuxer::FFMpegMuxer()
    : encodeAudio(false),
      encodeVideo(false),
      videoStream(nullptr),
      audioStream(nullptr),
      videoResolution(QSize(352, 288)),
      videoFrameRate(25),
      videoBitRate(static_cast<uint>(FFMpegMuxer::LOW_VIDEO_QUALITY)),
      videoPts(0),
      file(nullptr),
      savingToFile(false),
      formatContext(nullptr),
      avioContext(nullptr),
      mutex(QMutex::NonRecursive),
      initialized(false),
      encodingThread(nullptr),
      buffer(nullptr),
      startNewIntervalRequested(false)
{
    encodingThread = new QThread(this);
    this->moveToThread(encodingThread);

    connect(encodingThread, &QThread::started, this, &FFMpegMuxer::encodeInBackground);
}

void FFMpegMuxer::encodeInBackground()
{

    av_register_all();

    while (!QThread::currentThread()->isInterruptionRequested()) {

        if (startNewIntervalRequested) {
            if (prepareToEncodeNewInterval())
                startNewIntervalRequested = false;
            else
                qCritical() << "Can't prepare for next interval!";
        }

        //wait until we have more images to encode
        mutex.lock();
        if (imagesToEncode.isEmpty()) {
            waitingMoreDataToEncode.wait(&mutex);
        }

        QImage image = imagesToEncode.takeFirst();
        mutex.unlock();

        if (encodeVideo)
            encodeVideo = !doEncodeVideoFrame(image.copy());

    }
}

void FFMpegMuxer::initialize()
{
    //QMutexLocker locker(&mutex);

    if (!buffer)
        buffer = (unsigned char*)av_malloc(FFMPEG_BUFFER_SIZE);
    else
        std::memset(buffer, 0, FFMPEG_BUFFER_SIZE);

    encodeAudio = encodeVideo = false;
    audioStream = nullptr;
    videoStream = nullptr;

    if (savingToFile) {
        if (file && file->isOpen()) {
            file->deleteLater();
        }

        static int counter = 1;
        file = new QFile("teste" + QString::number(counter++) + ".avi", this);
        if(!file->open(QIODevice::WriteOnly))
            qCritical() << "Can't open the file " << file << file->errorString();
    }

    videoPts = 0;
}

FFMpegMuxer::~FFMpegMuxer()
{
    finishCurrentInterval();

    encodingThread->requestInterruption();
}

void FFMpegMuxer::encodeImage(const QImage &image)
{
    QMutexLocker locker(&mutex);

    if (encodeVideo)
        imagesToEncode.append(image);

    waitingMoreDataToEncode.wakeAll(); // wakeup the encodig thread


//    if (!initialized)
//        return;


//        bool canEncode = encodeVideo &&
//                                (!encodeAudio || av_compare_ts(videoStream->frame->pts, videoStream->stream->codec->time_base,
//                                                                audioStream->frame->pts, audioStream->stream->codec->time_base) <= 0);
//        if (canEncode) {
//            encodeVideo = !doEncodeVideoFrame(image);
//        }


}

void FFMpegMuxer::encodeAudioFrame()
{
    QMutexLocker locker(&mutex);
    if (!initialized)
        return;

    if (encodeAudio)
        encodeAudio = !doEncodeAudioFrame();
}

int FFMpegMuxer::writeCallback(void *instancePointer, uint8_t *buffer, int bufferSize)
{
    FFMpegMuxer *instance = (FFMpegMuxer *)instancePointer;

    QByteArray encodedBytes((const char*)buffer, bufferSize);

    if (instance->savingToFile && instance->file) {
        instance->file->write(encodedBytes);
    }

    bool isFirstPacket = instance->videoPts == 0;
    emit instance->dataEncoded(encodedBytes, isFirstPacket);

    return bufferSize;
}

void FFMpegMuxer::startNewInterval()
{
    startNewIntervalRequested = true;

    if (!encodingThread->isRunning()) {
       encodingThread->start();
    }

}

bool FFMpegMuxer::prepareToEncodeNewInterval()
{
    if(initialized)
        finishCurrentInterval();

    initialized = false;

    initialize();

    AVOutputFormat *format = av_guess_format("avi", nullptr, nullptr);
    if (!format) {
        qCritical() << "Can't guess the format!";
        return false;
    }

    int ret = avformat_alloc_output_context2(&formatContext, format, nullptr, nullptr); // allocate the output media context
    if (ret < 0) {
        qCritical() << "Error allocing output format context " << av_err2str(ret);
        return false;
    }

    // streaming to memory  https://trac.ffmpeg.org/ticket/984
    avioContext = avio_alloc_context(buffer, FFMPEG_BUFFER_SIZE, 1, this, nullptr, writeCallback, nullptr);
    avioContext->seekable = 0; // no seek
    formatContext->pb = avioContext;

    // Add the audio and video streams using the default format codecs and initialize the codecs.
    encodeVideo = addVideoStream(format->video_codec);

    if (format->audio_codec != AV_CODEC_ID_NONE) {
        //add_stream(&audio_st, &audio_codec, fmt->audio_codec);
        encodeAudio = false;
    }

    // Now that all the parameters are set, we can open the audio and video codecs and allocate the necessary encode buffers.
    if (encodeVideo && videoStream)
        if(!openVideoCodec(formatContext->video_codec))
            return false;

    if (encodeAudio && audioStream)
        openAudioCodec(formatContext->audio_codec);

     //av_dump_format(formatContext, 0, nullptr, 1);

    ret = avformat_write_header(formatContext, nullptr); // Write the stream header, if any.
    if (ret < 0) {
        qCritical() << "Error occurred when opening output file: " << av_err2str(ret);
        return false;
    }

    initialized = true;

    return initialized;
}

void FFMpegMuxer::finishCurrentInterval()
{
    //QMutexLocker locker(&mutex);

    if (!avioContext)
        return;

    if (videoStream)
        if (!videoStream->stream->codec || !avcodec_is_open(videoStream->stream->codec))
        return;

    if (audioStream)
        if (!audioStream->stream->codec || !avcodec_is_open(audioStream->stream->codec))
            return;


    /** Write the trailer, if any. The trailer must be written before you close the CodecContexts opened when you
     * wrote the header; otherwise av_write_trailer() may try to use memory that was freed on av_codec_close(). */
    int ret = av_write_trailer(formatContext);
    if(ret != 0)
        qCritical() << "Decoder Error while writing trailer:" << av_err2str(ret) << ret;

    avio_flush(avioContext);

    // Close each codec.
    if (videoStream) {
        delete videoStream;
        videoStream = nullptr;
    }

    if (audioStream) {
        delete audioStream;
        audioStream = nullptr;
    }

    avformat_free_context(formatContext); // free the stream

    av_free(avioContext);

    if (savingToFile && file && file->isOpen()) {
        file->deleteLater();
        file = nullptr;
    }

    initialized = false;
}

int FFMpegMuxer::writeFrame(const AVRational *time_base, AVStream *stream, AVPacket *packet)
{
    // rescale output packet timestamp values from codec to stream timebase
    av_packet_rescale_ts(packet, *time_base, stream->time_base);
    packet->stream_index = stream->index;

    // Write the compressed frame to the media file.
    return av_interleaved_write_frame(formatContext, packet);
}

// Add an output stream.
void FFMpegMuxer::addAudioStream(AVCodecID codecID)
{
    if (audioStream)
        return;

    audioStream = new AudioOutputStream();

    // find the encoder
    AVCodec *codec = avcodec_find_encoder(codecID);
    if (!codec) {
        qCritical() << "Could not find encoder for " << avcodec_get_name(codecID);
    }

    audioStream->stream = avformat_new_stream(formatContext, NULL);
    if (!audioStream->stream) {
        qCritical() << "Could not allocate stream";
    }
    audioStream->stream->id = formatContext->nb_streams-1;

    AVCodecContext *codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        qCritical() << "Could not alloc an encoding context";
    }
    audioStream->stream->codec = codecContext;

    codecContext->sample_fmt  = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
    codecContext->bit_rate    = 64000;
    codecContext->sample_rate = 44100;
    if (codec->supported_samplerates) {
        codecContext->sample_rate = codec->supported_samplerates[0];
        for (int i = 0; codec->supported_samplerates[i]; i++) {
            if (codec->supported_samplerates[i] == 44100)
                codecContext->sample_rate = 44100;
        }
    }
    codecContext->channels = av_get_channel_layout_nb_channels(codecContext->channel_layout);
    codecContext->channel_layout = AV_CH_LAYOUT_STEREO;
    if (codec->channel_layouts) {
        codecContext->channel_layout = codec->channel_layouts[0];
        for (int i = 0; codec->channel_layouts[i]; i++) {
            if (codec->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
                codecContext->channel_layout = AV_CH_LAYOUT_STEREO;
        }
    }
    codecContext->channels = av_get_channel_layout_nb_channels(codecContext->channel_layout);
    audioStream->stream->time_base = AVRational{ 1, codecContext->sample_rate };



    // Some formats want stream headers to be separate.
    if (formatContext->oformat->flags & AVFMT_GLOBALHEADER)
        codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}


bool FFMpegMuxer::addVideoStream(AVCodecID codecID)
{
    if (videoStream)
        return false;

    videoStream = new VideoOutputStream();

    // find the encoder
    AVCodec *codec = avcodec_find_encoder(codecID);
    if (!codec) {
        qCritical() << "Could not find encoder for " << avcodec_get_name(codecID);
        return false;
    }

    videoStream->stream  = avformat_new_stream(formatContext, NULL);
    if (!videoStream->stream) {
        qCritical() << "Could not allocate stream";
        return false;
    }
    videoStream->stream->id = formatContext->nb_streams-1;

    AVCodecContext *codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        qCritical() << "Could not alloc an encoding context";
        return false;
    }
    videoStream->stream->codec = codecContext;

    codecContext->codec_id = codecID;
    codecContext->bit_rate = videoBitRate;
    codecContext->width    = videoResolution.width(); // Resolution must be a multiple of two.
    codecContext->height   = videoResolution.height();

    /** timebase: This is the fundamental unit of time (in seconds) in terms
         * of which frame timestamps are represented. For fixed-fps content,
         * timebase should be 1/framerate and timestamp increments should be
         * identical to 1. */
    videoStream->stream->time_base = AVRational{ 1, videoFrameRate };
    codecContext->time_base       = videoStream->stream->time_base;
    qDebug() << "Setting frame rate to" << videoFrameRate;

    codecContext->gop_size      = 12; // emit one intra frame every twelve frames at most
    codecContext->pix_fmt       = AV_PIX_FMT_YUV420P;
    if (codecContext->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
        codecContext->max_b_frames = 2; /* just for testing, we also add B-frames */
    }
    if (codecContext->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
        /** Needed to avoid using macroblocks in which some coeffs overflow.
             * This does not happen with normal video, it just happens here as
             * the motion of the chroma plane does not match the luma plane. */
        codecContext->mb_decision = 2;
    }

    // Some formats want stream headers to be separate.
    if (formatContext->oformat->flags & AVFMT_GLOBALHEADER)
        codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

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
    AVPacket packet = { 0 }; // data and size must be 0;
    AVFrame *frame;
    int dst_nb_samples;

    av_init_packet(&packet);
    AVCodecContext *codecContext = audioStream->stream->codec;

    frame = nullptr; // TODO convert SamplesBuffer to frame

    if (frame) {
        /* convert samples from native format to destination codec format, using the resampler */
        /* compute destination number of samples */
        dst_nb_samples = av_rescale_rnd(swr_get_delay(audioStream->swrContext, codecContext->sample_rate) + frame->nb_samples,
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
        qCritical() << "Error encoding audio frame: " << av_err2str(ret);
        exit(1);
    }

    if (gotPacket) {
        ret = writeFrame(&codecContext->time_base, audioStream->stream, &packet);
        if (ret < 0) {
            qCritical() << "Error while writing audio frame: " << av_err2str(ret);
            exit(1);
        }
    }

    return (frame || gotPacket) ? false : true;
}

/**************************************************************/
/* video output */

AVFrame *FFMpegMuxer::allocPicture(enum AVPixelFormat pixelFormat, int width, int height)
{
    AVFrame *picture = nullptr;

    picture = av_frame_alloc();
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

bool FFMpegMuxer::openVideoCodec(AVCodec *codec)
{

    Q_ASSERT(videoStream && videoStream->stream && videoStream->stream->codec);

    AVCodecContext *codecContext = videoStream->stream->codec;

    int ret = avcodec_open2(codecContext, codec, nullptr); /* open the codec */
    if (ret < 0) {
        qCritical() << "Could not open video codec: " << av_err2str(ret);
        return false;
    }

    /* allocate and init a re-usable frame */
    //qDebug() << "Allocating codec context width:" << codecContext->width << "height:" << codecContext->height;
    videoStream->frame = allocPicture(codecContext->pix_fmt, codecContext->width, codecContext->height);

    if (!videoStream->frame) {
        qCritical() << "Could not allocate video frame";
        return false;
    }

    /* If the output format is not YUV420P, then a temporary YUV420P
     * picture is needed too. It is then converted to the required
     * output format. */
    videoStream->tempFrame = NULL;
    if (codecContext->pix_fmt != AV_PIX_FMT_YUV420P) {
        videoStream->tempFrame = allocPicture(AV_PIX_FMT_YUV420P, codecContext->width, codecContext->height);
        if (!videoStream->tempFrame) {
            qCritical() << "Could not allocate temporary picture";
            return false;
        }
    }

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(videoStream->stream->codecpar, codecContext);
    if (ret < 0) {
        qCritical() << "Could not copy the stream parameters";
        return false;
    }

    return true;
}

void FFMpegMuxer::imageToYuvPicture(const QImage &image, AVFrame *picture, int width, int height)
{

    //avoiding crash when camera preview is resized
    width = qMin(width, image.width());
    height = qMin(height, image.height());

    // Preparing the buffer to get YUV420P data
    int size = avpicture_get_size(AV_PIX_FMT_YUV420P, width, height);
    uint8_t *pic_dat = (uint8_t *) av_malloc(size);

    // Transforming data from RGB to YUV420P
    RGBtoYUV420P(image.bits(), pic_dat, image.depth()/8, true, width, height);

    picture->quality = 0;

    // Filling AVFrame with YUV420P data
    avpicture_fill((AVPicture *)picture, pic_dat, AV_PIX_FMT_YUV420P, width, height);
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
    AVCodecContext *codecContext = videoStream->stream->codec;

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally; make sure we do not overwrite it here */
    if (av_frame_make_writable(videoStream->frame) < 0) {
        qCritical() << "frame not writable";
        return;
    }

    if (codecContext->pix_fmt != AV_PIX_FMT_YUV420P) { /* need image convertion? as we only generate a YUV420P picture, we must convert it to the codec pixel format if needed */
        if (!videoStream->swsContext) {
            //videoStream->swsContext = sws_getContext(codecContext->width, codecContext->height, AV_PIX_FMT_YUV420P, codecContext->width, codecContext->height, codecContext->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
            videoStream->swsContext = sws_getContext(image.width(), image.height(), AV_PIX_FMT_YUV420P, codecContext->width, codecContext->height, codecContext->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
            if (!videoStream->swsContext) {
                qCritical() << "Could not initialize the conversion context";
                return;
            }
        }
        imageToYuvPicture(image, videoStream->tempFrame, codecContext->width, codecContext->height);
        sws_scale(videoStream->swsContext, (const uint8_t * const *)videoStream->tempFrame->data, videoStream->tempFrame->linesize, 0, codecContext->height, videoStream->frame->data, videoStream->frame->linesize);
    } else {
        imageToYuvPicture(image, videoStream->frame, codecContext->width, codecContext->height);
    }

    videoStream->frame->pts = videoPts++;
}

/*
 * encode one video frame and send it to the muxer
 * return true when encoding is finished, false otherwise
 */
bool FFMpegMuxer::doEncodeVideoFrame(const QImage &image)
{
    if (!videoStream)
        return false;

    AVCodecContext *codecContext = videoStream->stream->codec;

    if(avcodec_is_open(codecContext) <= 0) {
        qCritical() << "Codec is not opened!";
        return false;
    }

    int gotPacket = 0;
    AVPacket packet = { 0 };

    Q_ASSERT(videoStream->stream != nullptr);
    Q_ASSERT(videoStream->stream->codec != nullptr);

    fillFrameWithImageData(image);

    av_init_packet(&packet);

    /* encode the image */
    int ret = avcodec_encode_video2(codecContext, &packet, videoStream->frame, &gotPacket);
    if (ret < 0) {
        qCritical() << "Error encoding video frame: " << av_err2str(ret);
        return false;
    }

    if (gotPacket)
        ret = writeFrame(&codecContext->time_base, videoStream->stream, &packet);
    else
        ret = 0;

    if (ret < 0) {
        qCritical() << "Error while writing video frame: " << av_err2str(ret);
        return false;
    }

    return (videoStream->frame || gotPacket) ? false : true;
}
