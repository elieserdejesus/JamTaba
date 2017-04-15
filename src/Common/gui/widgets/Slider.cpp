#include "Slider.h"

#include <QPainter>

#include "Utils.h"

Slider::Slider(QWidget *parent)
    : QSlider(parent),
      sliderType(Slider::AudioSlider)
{
    setMaximum(120);
}

void Slider::paintEvent(QPaintEvent *ev)
{
    {
        QPainter painter(this);
        drawMarker(painter);
    }

    QSlider::paintEvent(ev);
}

qreal Slider::getMarkerPosition() const
{
    if (sliderType == Slider::AudioSlider) {
        if (orientation() == Qt::Vertical)
            return 1 - 100.0/maximum();
        else
            return 100.0/maximum();
    }

    return 0.5; // pan marker is always drawed in center
}

void Slider::setSliderType(SliderType type)
{
    if (this->sliderType != type) {
        this->sliderType = type;
        update();
    }
}

void Slider::drawMarker(QPainter &painter)
{
    if (sliderType == Slider::AudioSlider && maximum() <= 100)
        return;

    static const QColor MARKER_COLOR(0, 0, 0, 120);
    painter.setPen(MARKER_COLOR);

    const static int markerPreferredSize = 13;
    const qreal markerSize = (orientation() == Qt::Vertical) ? qMin(markerPreferredSize, width()) : qMin(markerPreferredSize, height());
    const qreal center = (orientation() == Qt::Vertical) ? width()/2.0 : height()/2.0;
    const static qreal offset = -1.0;

    const qreal markerPosition = getMarkerPosition();

    qreal x1, x2, y1, y2;
    if (orientation() == Qt::Vertical) {
        x1 = center - markerSize/2.0 + offset;
        x2 = center + markerSize/2.0 + offset;
        y1 = y2 = markerPosition * height();

    }
    else {
        y1 = center - markerSize/2.0 + offset;
        y2 = center + markerSize/2.0 + offset;
        x1 = x2 = markerPosition * width();
    }

    painter.drawLine(x1, y1, x2, y2);
}
