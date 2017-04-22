#include "Slider.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOptionSlider>

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
        qreal position = 0;
        if (orientation() == Qt::Vertical)
            position = 1 - 100.0/maximum();
        else
            position = 100.0/maximum();

        return position;
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

    static const QColor MARKER_COLOR(0, 0, 0, 80);
    painter.setPen(MARKER_COLOR);

    const static int markerPreferredSize = 13;
    const qreal markerSize = (orientation() == Qt::Vertical) ? qMin(markerPreferredSize, width()) : qMin(markerPreferredSize, height());
    const qreal center = (orientation() == Qt::Vertical) ? width()/2.0 : height()/2.0;

    const qreal markerPosition = getMarkerPosition();

    qreal x1, x2, y1, y2;
    if (orientation() == Qt::Vertical) {
        x1 = center - markerSize/2.0;
        x2 = center + markerSize/2.0;
        y1 = y2 = markerPosition * height();

    }
    else {
        y1 = center - markerSize/2.0;
        y2 = center + markerSize/2.0;
        x1 = x2 = markerPosition * width();
    }

    QLine line(x1, y1, x2, y2);

    if (sliderType == Slider::AudioSlider) {
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        QRect rectHandle = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, NULL);

        if (orientation() == Qt::Vertical)
            line.translate(0.0, rectHandle.height()/2.0);
        else
            line.translate(-rectHandle.width()/2.0, 0.0);
    }

    painter.drawLine(line);
}
