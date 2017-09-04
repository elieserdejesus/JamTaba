#include "VideoFrameGrabber.h"

#include <QPainter>
#include <QDateTime>

CameraFrameGrabber::CameraFrameGrabber(QObject * parent) :
    QAbstractVideoSurface(parent)
{

}

bool CameraFrameGrabber::present(const QVideoFrame& frame)
{
    if (frame.isValid()) {

        QVideoFrame cloneFrame(frame);

        if (cloneFrame.map(QAbstractVideoBuffer::ReadOnly)) {

            QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat());

            if (imageFormat != QImage::Format_Invalid) {
                lastImage = QImage(cloneFrame.bits(), cloneFrame.width(), cloneFrame.height(), imageFormat)
                        .copy() // necessary
                        .mirrored(false, true);

                emit frameAvailable(lastImage);
            }

            cloneFrame.unmap();
        }

        return true;
    }

    return false;
}

// +++++++++++++++++++=


//QImage DummyFrameGrabber::grab(const QSize &size)
//{

//    QImage image(size, QImage::Format_RGB32);
//    image.fill(Qt::black);

//    QPainter painter(&image);

//    //painter.fillRect(rect(), palette().background());

//    static int counter = 0;

//    qint64 now = QDateTime::currentMSecsSinceEpoch();
//    static qint64 lastCounterUpdate = 0;

//    if (now - lastCounterUpdate >= 1000) {
//        counter++;
//        lastCounterUpdate = now;
//    }

//    const QString text = QString::number(counter % 10);// "JamTaba";

//    painter.setPen(Qt::white);
//    painter.drawText(QRect(QPoint(0, 0), size), text, QTextOption(Qt::AlignCenter));

//    return image;
//}
