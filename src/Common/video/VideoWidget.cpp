#include "VideoWidget.h"
#include <QIcon>

VideoWidget::VideoWidget(QWidget *parent) :
    QWidget(parent),
    activated(true)
{

    iconOn = QIcon(":/images/webcam_on.png");
    iconOff = QIcon(":/images/webcam_off.png");

}

void VideoWidget::activate(bool status)
{
    if (status != activated) {
        activated = status;
        update();
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

        painter.drawImage(
                    QRectF(targetX, targetY, targetWidth, targetHeight),
                    currentImage,
                    currentImage.rect());
    }
    else { // not activated
        painter.fillRect(rect(), QColor(0, 0, 0, 30));
    }

    if (activated)
        iconOn.paint(&painter, rect(), Qt::AlignBottom | Qt::AlignRight);
    else
        iconOff.paint(&painter, rect(), Qt::AlignBottom | Qt::AlignRight);
}
