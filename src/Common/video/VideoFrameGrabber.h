#ifndef _VIDEO_FRAME_GRABBER_
#define _VIDEO_FRAME_GRABBER_

#include <QVideoWidget>
#include <QAbstractVideoSurface>
#include <QWidget>
#include <QThread>

class VideoFrameGrabber
{

public:
    virtual ~VideoFrameGrabber() {}
    virtual QImage grab(const QSize &size = QSize()) = 0;
};


class DummyFrameGrabber : public VideoFrameGrabber
{

public:
    virtual QImage grab(const QSize &size) override;
};

class CameraFrameGrabber : public QAbstractVideoSurface, public VideoFrameGrabber
{
    Q_OBJECT

public:

    CameraFrameGrabber(QObject * parent);

    inline QList<QVideoFrame::PixelFormat>supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const override
    {
        Q_UNUSED(type)

        return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_RGB32;
    }

    bool present(const QVideoFrame& frame) override;

    inline QImage grab(const QSize &size) override
    {
        Q_UNUSED(size);
        return lastImage;
    }

signals:
    void frameAvailable(const QImage &frame);


private:
    QImage lastImage;

};

#endif
