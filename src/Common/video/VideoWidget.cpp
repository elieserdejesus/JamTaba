#include "VideoWidget.h"

VideoWidget::VideoWidget(QWidget *parent) :
    QWidget(parent)
{

}

void VideoWidget::setCurrentFrame(const QImage &image)
{

    this->currentImage = image;

    update();
}

void VideoWidget::paintEvent(QPaintEvent *ev)
{

    Q_UNUSED(ev);

    if (!currentImage.isNull()) {

        QPainter painter(this);

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
}
