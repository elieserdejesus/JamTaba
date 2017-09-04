#include "VideoWidget.h"
#include <QIcon>

VideoWidget::VideoWidget(QWidget *parent, bool activated) :
    QWidget(parent),
    activated(activated)
{

    webcamIcon = QIcon(":/images/webcam.png");
}

void VideoWidget::activate(bool status)
{
    if (status != activated) {
        activated = status;
        update();

        emit statusChanged(activated);
    }
}

void VideoWidget::setCurrentFrame(const QImage &image)
{

    this->currentImage = image;

    update();
}

void VideoWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);

    activate(!activated);

    updateGeometry();
}

void VideoWidget::paintEvent(QPaintEvent *ev)
{

    Q_UNUSED(ev);

    QPainter painter(this);

    if (!currentImage.isNull() && activated) {

        QRect sourceRect = currentImage.rect();

        qreal ratio = 1.0;

        bool small = height() < width();

        if (small)
            ratio =  static_cast<float>(height())/sourceRect.height();
        else
            ratio =  static_cast<float>(width())/sourceRect.width();

        qreal targetHeight = small ? height() : currentImage.height() * ratio;
        qreal targetWidth = small ? currentImage.width() * ratio : width();
        qreal targetX = (width() - targetWidth) / 2.0;
        qreal targetY = (height() - targetHeight) / 2.0;

        QRectF targetRect(targetX, targetY, targetWidth, targetHeight);
        painter.drawImage(targetRect, currentImage, currentImage.rect());
    }
    else { // not activated

        painter.fillRect(rect(), QColor(0, 0, 0, 30));
    }

    bool paintIcon = !activated || underMouse();
    Qt::Alignment alignment = !activated ? Qt::AlignCenter : (Qt::AlignRight | Qt::AlignBottom);

    if (paintIcon)
        webcamIcon.paint(&painter, rect().marginsRemoved(QMargins(3, 3, 3, 3)), alignment);

}

QSize VideoWidget::minimumSizeHint() const
{
    if (activated)
        return QSize(120, 90);

    return QSize(30, 30);
}
