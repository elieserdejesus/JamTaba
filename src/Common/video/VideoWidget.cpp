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
    currentImage = image;

    if (!currentImage.isNull() && activated)
        updateGeometry();

    if (activated)
        update();
}

void VideoWidget::resizeEvent(QResizeEvent *ev)
{
    Q_UNUSED(ev);

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

QSize VideoWidget::sizeHint() const
{
    return minimumSizeHint();
}

QSize VideoWidget::minimumSizeHint() const
{
    static const int MAX_HEIGHT = 90;
    static const int MAX_WIDTH = 120;
    static const int MIN_SIZE = 32;

    if (activated) {
        if (!currentImage.isNull()) {
            bool isInVerticalLayout = parentWidget()->height() > parentWidget()->width();
            QImage img = isInVerticalLayout ? (currentImage.scaledToWidth(qMin(width(), MAX_WIDTH))) : (currentImage.scaledToHeight(qMin(MAX_HEIGHT, height())));
            return img.size();
        }

        return QSize(MAX_WIDTH, MAX_HEIGHT);
    }

    return QSize(MIN_SIZE, MIN_SIZE);
}
