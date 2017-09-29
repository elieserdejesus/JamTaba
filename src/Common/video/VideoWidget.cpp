#include "VideoWidget.h"
#include <QIcon>
#include <QDebug>

const int VideoWidget::MIN_SIZE = 32;

VideoWidget::VideoWidget(QWidget *parent, bool activated) :
    QWidget(parent),
    activated(activated),
    targetRect(0, 0, VideoWidget::MIN_SIZE, VideoWidget::MIN_SIZE)
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
    currentImage = image;

    if (!currentImage.isNull() && activated) {

        updateScaledImage();
        updateGeometry();
    }

    if (activated)
        update();
}

void VideoWidget::updateScaledImage()
{
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

    targetRect = QRect(targetX, targetY, targetWidth, targetHeight);

    scaledImage = currentImage.scaled(targetRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void VideoWidget::resizeEvent(QResizeEvent *ev)
{
    Q_UNUSED(ev);

    if (!currentImage.isNull())
        updateScaledImage();

    updateGeometry();
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

    static const QColor bgColor(0, 0, 0, 30);
    painter.fillRect(rect(), bgColor);

    if (!currentImage.isNull() && activated) {
        painter.drawImage(targetRect, scaledImage, scaledImage.rect());
    }

    bool paintIcon = !activated || underMouse();
    Qt::Alignment alignment = !activated ? Qt::AlignCenter : (Qt::AlignRight | Qt::AlignBottom);

    if (paintIcon)
        webcamIcon.paint(&painter, targetRect.marginsRemoved(QMargins(3, 3, 3, 3)), alignment);
}

QSize VideoWidget::sizeHint() const
{
    return minimumSizeHint();
}

void VideoWidget::showEvent(QShowEvent *ev)
{
    Q_UNUSED(ev);

    emit visibilityChanged(true);
}

void VideoWidget::hideEvent(QHideEvent *ev)
{
    Q_UNUSED(ev);

    emit visibilityChanged(false);
}

QSize VideoWidget::minimumSizeHint() const
{
    static const int MAX_HEIGHT = 90;
    static const int MAX_WIDTH = 120;

    if (activated)
        return QSize(MAX_WIDTH, MAX_HEIGHT);

    return QSize(VideoWidget::MIN_SIZE, VideoWidget::MIN_SIZE);
}
