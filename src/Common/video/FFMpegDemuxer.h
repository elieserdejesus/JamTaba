#ifndef FFMPEGDEMUXER_H
#define FFMPEGDEMUXER_H

#include "FFMpegCommon.h"

#include <QObject>
#include <QByteArray>
#include <QDataStream>
#include <QBuffer>
#include <QImage>

class FFMpegDemuxer : public QObject
{

    Q_OBJECT

public:
    FFMpegDemuxer();
    ~FFMpegDemuxer();
    bool open(const QByteArray &encodedData);
    void close();
    void decodeNextFrame();
    uint getFrameRate() const;
    bool isOpened() const;

signals:
    void opened(uint frameRate);
    void frameDecoded(const QImage &decodedFrame);

private:
    AVFormatContext *formatContext;
    AVIOContext *avioContext;
    AVCodecContext *codecContext;
    SwsContext *swsContext;
    AVFrame *frame;
    AVFrame *frameRGB;
    uint8_t *rgbBuffer;

    unsigned char *buffer; // avio buffer used in callback

    QByteArray encodedData;
    QBuffer encodedBuffer;

    bool initialized;

    static int readCallback(void *stream, uint8_t *buffer, int bufferSize);
    static int64_t seekCallback(void *opaque, int64_t offset, int whence);

    void decode();

    AVInputFormat *probeInputFormat();
};

inline bool FFMpegDemuxer::isOpened() const
{
    return initialized;
}

#endif // FFMPEGDEMUXER_H
