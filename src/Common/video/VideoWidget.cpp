#include "VideoWidget.h"
#include <QIcon>
#include <QDebug>

const int VideoWidget::MIN_SIZE = 32;

VideoWidget::VideoWidget(QWidget *parent, const QIcon &icon, bool activated) :
    QWidget(parent),
    activated(activated),
    targetRect(0, 0, VideoWidget::MIN_SIZE, VideoWidget::MIN_SIZE),
    webcamIcon(icon),
    imageRatio(0.75)
{
    setMinimumSize(VideoWidget::MIN_SIZE, VideoWidget::MIN_SIZE);
}

void VideoWidget::setIcon(const QIcon &icon)
{
    webcamIcon = icon;
    update();
}

void VideoWidget::activate(bool status)
{
    if (status != activated) {
        activated = status;
        //update();

        emit statusChanged(activated);

        updateGeometry();
    }
}

void VideoWidget::setCurrentFrame(const QImage &image)
{
    currentImage = image;

    if (!currentImage.isNull() && activated) {
        imageRatio = static_cast<qreal>(currentImage.height())/currentImage.width();
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
}

QSize VideoWidget::minimumSizeHint() const
{
    if (activated) {
        auto w = width();
        auto h = height();
        if (parentWidget()) {
            bool parentIsVertical = parentWidget()->height() > parentWidget()->width();
            if (parentIsVertical) {
                w = parentWidget()->width();
                h = heightForWidth(w);
            }
            else {
                h = height();
                w = h * 1.0/imageRatio;
            }
        }
        return QSize(w, h);
    }

    return minimumSize();
}

bool VideoWidget::hasHeightForWidth() const
{
    return activated;
}

int VideoWidget::heightForWidth(int width) const
{
    if (activated) {
        if (parentWidget() && parentWidget()->width() < width)
            width = parentWidget()->width();

        return width * imageRatio;
    }

    return MIN_SIZE;
}

void VideoWidget::paintEvent(QPaintEvent *ev)
{

    Q_UNUSED(ev);

    QPainter painter(this);
    if (!painter.isActive())
        return;

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
