#include "FFMpegDemuxer.h"

#include <QDebug>
#include <QImage>
#include <QFile>
#include <QFileInfo>

FFMpegDemuxer::FFMpegDemuxer(QObject *parent, const QByteArray &encodedData) :
    QObject(parent),
    formatContext(nullptr),
    avioContext(nullptr),
    swsContext(nullptr),
    frame(nullptr),
    frameRGB(nullptr),
    rgbBuffer(nullptr),
    buffer(nullptr),
    encodedData(encodedData)
{
    av_register_all();
    avcodec_register_all();

    qRegisterMetaType<QList<QImage>>();
}

FFMpegDemuxer::~FFMpegDemuxer()
{
    close();
}

void FFMpegDemuxer::close()
{
    if (formatContext) {
        avformat_close_input(&formatContext);
        formatContext = nullptr;
        avioContext = nullptr;
        //codecContext = nullptr;
        swsContext = nullptr;
    }

    if (frame) {
        av_free(frame);
        frame = nullptr;
    }

    if (frameRGB) {
        av_free(frameRGB);
        frameRGB = nullptr;
    }

    if (rgbBuffer) {
        delete [] rgbBuffer;
        rgbBuffer = nullptr;
    }

    if (buffer) {
        //av_free(buffer);
        buffer = nullptr;
    }

}

int FFMpegDemuxer::readCallback(void *stream, uint8_t *buffer, int bufferSize)
{
    if (!stream || !buffer)
        return 0;

    auto st = reinterpret_cast<QIODevice *>(stream);

    if (st)
        return st->read((char *)buffer, bufferSize);
}

bool FFMpegDemuxer::open()
{
    encodedBuffer.setBuffer(&(this->encodedData));
    if(!encodedBuffer.open(QIODevice::ReadOnly)) {
        qCritical() << "Error opening demuxer " << encodedBuffer.errorString();
        return false;
    }

    buffer = (unsigned char*)av_malloc(FFMPEG_BUFFER_SIZE);

    formatContext = avformat_alloc_context();

    //avio_op
    avioContext = avio_alloc_context(buffer, FFMPEG_BUFFER_SIZE, 0, &(this->encodedBuffer), readCallback, nullptr, nullptr);

    if (!avioContext || !formatContext)
        return false;

    avioContext->seekable = 0; // no seek

    formatContext->pb = avioContext;
    formatContext->flags = AVFMT_FLAG_CUSTOM_IO;

    int ret = avformat_open_input(&formatContext, nullptr, nullptr, nullptr);
    if (ret != 0) {
        qCritical() << "Decoder Error while opening input:" << av_error_to_qt_string(ret) << ret;
        return false;
    }

    if (formatContext->nb_streams <= 0) {
        qCritical() << "Error opening the demuxer: nb_streams <= 0";
        return false;
    }

    auto stream = formatContext->streams[0]; // first stream

    if (!stream) {
        qWarning() << "Error in FFMpegDemuxer::open, the first stream is null";
        return false;
    }

    auto codecContext = stream->codec;

    /* find decoder for the stream */
    auto decoder = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!decoder) {
        qCritical() << "Failed to find the codec" << avcodec_get_name(stream->codecpar->codec_id);
        return false;
    }

    if (!codecContext) {
        qWarning() << "Error in FFMpegDemuxer::open, the codecContext is null";
        return false;
    }

    ret = avcodec_open2(codecContext, decoder, nullptr);
    if (ret < 0) {
        qCritical() << av_error_to_qt_string(ret);
        return false;
    }

    frame = av_frame_alloc();
    if (!frame) {
        qCritical() << "Could not allocate frame";
        return false;
    }

    return true;
}

uint FFMpegDemuxer::getFrameRate() const
{
    if (formatContext && formatContext->nb_streams > 0) {
        auto firstStream = formatContext->streams[0];
        if (firstStream && firstStream->codec) {
            return firstStream->codec->framerate.num;
        }
    }

    return 0;
}

void FFMpegDemuxer::decode()
{
    QList<QImage> decodedImages;

    if (!open()) {
        qCritical() << "Can't open the video decoder!";
        emit imagesDecoded(decodedImages, getFrameRate());
        return;
    }

    if (!formatContext || formatContext->nb_streams <= 0)
        return;

    auto codecContext = formatContext->streams[0]->codec;

    if (!codecContext)
        return;

    /* initialize packet, set data to NULL, let the demuxer fill it */
    AVPacket packet;
    packet.data = nullptr;
    packet.size = 0;

    int ret = 0;

    /* read frames */
    while ((ret = av_read_frame(formatContext, &packet)) == 0)
    {

        int framesDecoded = 0;

        while (framesDecoded <= 0)
        {
            ret = avcodec_send_packet(codecContext, &packet);

            if (ret != 0) { // error
                if (ret != AVERROR(EAGAIN)) {
                    qCritical() << "error decoding video frame" << av_error_to_qt_string(ret) << ret;
                    emit imagesDecoded(decodedImages, getFrameRate());
                }
                return;
            }

            while((ret = avcodec_receive_frame(codecContext, frame)) == 0)  // got a frame?
            {
                int width = (codecContext->width > 0) ? codecContext->width : frame->width;
                int height = (codecContext->height > 0) ? codecContext->height : frame->height;

                if (!frameRGB) {
                    frameRGB = av_frame_alloc();

                    // Determine required buffer size and allocate buffer
                    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
                    rgbBuffer = new uint8_t[numBytes];

                    // Assign appropriate parts of buffer to image planes in pFrameRGB
                    av_image_fill_arrays(frameRGB->data, frameRGB->linesize, rgbBuffer, AV_PIX_FMT_RGB24, width, height, 1);
                }

                // Convert the image format (init the context the first time)
                AVPixelFormat sourcePixelFormat = codecContext->pix_fmt;
                AVPixelFormat destinationPixelFormat = AV_PIX_FMT_RGB24;

                if (!frame->width || !frame->height) // 0 size images are skipped
                    continue;

                swsContext = sws_getCachedContext(swsContext, width, height, sourcePixelFormat, width, height, destinationPixelFormat, SWS_BICUBIC, nullptr, nullptr, nullptr);

                if(!swsContext){
                    qCritical() << "Cannot initialize the conversion context!";
                    emit imagesDecoded(decodedImages, getFrameRate());
                    return;
                }
                sws_scale(swsContext, frame->data, frame->linesize, 0, height, frameRGB->data, frameRGB->linesize);

                // Convert the frame to QImage
                QImage img(width, height, QImage::Format_RGB888);

                for(int y=0; y < height; y++)
                    memcpy(img.scanLine(y), frameRGB->data[0] + y * frameRGB->linesize[0], width * 3);

                decodedImages << img;

                framesDecoded++;
            }

            if (ret != 0 && ret != AVERROR(EAGAIN)) {
                qCritical() << "error decoding video frame in avcodec_receive_frame" << av_error_to_qt_string(ret) << ret;
                emit imagesDecoded(decodedImages, getFrameRate());
                return;
            }
        }
    }

    emit imagesDecoded(decodedImages, getFrameRate());
}
