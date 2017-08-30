#ifndef _JTBA_VIDEO_CODEC_
#define _JTBA_VIDEO_CODEC_

#include <QPixmap>
#include <QObject>

class VideoEncoder : public QObject
{
    Q_OBJECT

public:
    virtual ~VideoEncoder(){}
    virtual void encodeFrame(const QPixmap &frame, int intervalPosition) = 0;

signals:
    void frameEncoded(const QByteArray &encodedData, int intervalPosition);

};


class VideoDecoder
{

public:
    virtual ~VideoDecoder(){}
    virtual QPixmap decodeFrame() = 0;
};

#endif
