#ifndef FFMPEGDEMUXER_H
#define FFMPEGDEMUXER_H

#include "FFMpegCommon.h"

#include <QByteArray>
#include <QDataStream>
#include <QBuffer>
#include <QImage>

class FFMpegDemuxer : public QObject
{

    Q_OBJECT

public:
    FFMpegDemuxer(QObject *parent, const QByteArray &encodedData);
    ~FFMpegDemuxer();

    void decode();

signals:
    void imagesDecoded(QList<QImage> images, uint frameRate);
private:
    AVFormatContext *formatContext;
    AVIOContext *avioContext;
    //AVCodecContext *codecContext;
    SwsContext *swsContext;
    AVFrame *frame;
    AVFrame *frameRGB;
    uint8_t *rgbBuffer;

    unsigned char *buffer; // avio buffer used in callback

    QByteArray encodedData;
    QBuffer encodedBuffer;

    static int readCallback(void *stream, uint8_t *buffer, int bufferSize);
    static int64_t seekCallback(void *opaque, int64_t offset, int whence);

    void close();
    bool open();
    uint getFrameRate() const;
};

#endif // FFMPEGDEMUXER_H
