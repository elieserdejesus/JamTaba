#include "FFMpegDemuxer.h"

#include <QDebug>
#include <QImage>
#include <QFile>
#include <QFileInfo>

FFMpegDemuxer::FFMpegDemuxer(QObject *parent, const QByteArray &encodedData) :
    QObject(parent),
    formatContext(nullptr),
    avioContext(nullptr),
    //codecContext(nullptr),
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

//int64_t FFMpegDemuxer::seekCallback(void *opaque, int64_t offset, int whence)
//{
//    QIODevice *stream = (QIODevice *)opaque;

//    if (stream->seek(offset)) {
//        return stream->pos();
//    }

//    // handling AVSEEK_SIZE doesn't seem mandatory
//    return -1;
//}

int64_t FFMpegDemuxer::seekCallback(void *opaque, int64_t offset, int whence)
{

    QString w = "";
    if (whence == SEEK_SET)
        w = "SEEK_SET";
    if (whence == SEEK_CUR)
        w = "SEEK_CUR";
    if (whence == SEEK_END)
        w = "SEEK_END";
    if (whence == AVSEEK_SIZE)
        w = "AVSEEK_SIZE";

    qDebug() << "Seek:" << offset << whence << w;


    QBuffer *stream = (QBuffer *)opaque;

    switch (whence) {
        case AVSEEK_SIZE:
            return stream->size();
        case SEEK_SET:
            stream->seek(0); // begin
        break;
    }

    if (whence == AVSEEK_SIZE)
        return stream->size();

    if (!stream->seek(stream->pos() + offset))
        return -1;

    qDebug() << "stream->pos" << stream->pos();
    return stream->pos();
}

int FFMpegDemuxer::readCallback(void *stream, uint8_t *buffer, int bufferSize)
{
    QIODevice *st = (QIODevice *)stream;
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
    avioContext = avio_alloc_context(buffer, FFMPEG_BUFFER_SIZE, 0, &(this->encodedBuffer), readCallback, nullptr, seekCallback);
    avioContext->seekable = 0; // no seek

    formatContext->pb = avioContext;
    formatContext->flags = AVFMT_FLAG_CUSTOM_IO;

    int ret = avformat_open_input(&formatContext, nullptr, nullptr, nullptr);
    if (ret != 0) {
        qCritical() << "Decoder Error while opening input:" << av_error_to_qt_string(ret) << ret;
        return false;
    }

    AVInputFormat *format = formatContext->iformat;
    if (!format) {
        qCritical() << "Decoder Error: Format is null!";
        return false;
    }

    AVMediaType type = AVMEDIA_TYPE_VIDEO;

    if (formatContext->nb_streams <= 0) {
        qCritical() << "Error opening the demuxer: nb_streams <= 0";
        return false;
    }

    AVStream *stream = formatContext->streams[0]; // first stream

    auto codecContext = stream->codec;

    /* find decoder for the stream */
    AVCodec *decoder = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!decoder) {
        qCritical() << "Failed to find the codec" << av_get_media_type_string(type);
        return false;
    }

    ret = avcodec_open2(codecContext, decoder, nullptr);
    if (ret < 0) {
        qCritical() << av_error_to_qt_string(ret);
        return false;
    }

    if (!codecContext)
        qCritical() << "video codec is null";

    frame = av_frame_alloc();
    if (!frame) {
        qCritical() << "Could not allocate frame";
        return false;
    }

    frameRGB = av_frame_alloc();
    if (frameRGB) {
        // Determine required buffer size and allocate buffer
        int width = codecContext->width;
        int height = codecContext->height;
        int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
        rgbBuffer = new uint8_t[numBytes];

        // Assign appropriate parts of buffer to image planes in pFrameRGB
        av_image_fill_arrays(frameRGB->data, frameRGB->linesize, rgbBuffer, AV_PIX_FMT_RGB24, width, height, 1);
    }
    else {
        qCritical() << "Could not allocate frameRGB";
        return false;
    }

    return true;
}

uint FFMpegDemuxer::getFrameRate() const
{
    if (formatContext && formatContext->nb_streams > 0) {
        auto firstStream = formatContext->streams[0];
        if (firstStream->codec) {
            auto codecContext = formatContext->streams[0]->codec;
            return codecContext->framerate.num;
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

    /* initialize packet, set data to NULL, let the demuxer fill it */
    AVPacket packet;
    av_init_packet(&packet);
    packet.data = nullptr;
    packet.size = 0;

    int gotFrame = 0;

    if (!formatContext || formatContext->nb_streams <= 0)
        return;

    auto codecContext = formatContext->streams[0]->codec;

    /* read frames */
    int ret = 0;
    while ((ret = av_read_frame(formatContext, &packet)) == 0) {

        ret = avcodec_decode_video2(codecContext, frame, &gotFrame, &packet);

        av_free_packet(&packet);

        if (ret < 0) { // error
            qCritical() << "error decoding video frame" << av_error_to_qt_string(ret) << ret;
            emit imagesDecoded(decodedImages, getFrameRate());
            return;
        }

        if (gotFrame) {
            // Convert the image format (init the context the first time)
            int width = codecContext->width;
            int height = codecContext->height;
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
        }
    }

    qDebug() << av_error_to_qt_string(ret) << ret;

    emit imagesDecoded(decodedImages, getFrameRate());
}
