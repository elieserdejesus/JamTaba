#include "Slider.h"
#include "Utils.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QToolTip>

Slider::Slider(QWidget *parent) :
    QSlider(parent),
    sliderType(Slider::AudioSlider)
{
    setMaximum(120);

    connect(this, &Slider::valueChanged, this, &Slider::showToolTip);
}

void Slider::updateToolTipValue()
{
    if (sliderType == Slider::AudioSlider) {
        double poweredGain = Utils::linearGainToPower(value()/100.0);
        double faderDb = Utils::linearToDb(poweredGain);
        int precision = faderDb > -10 ? 1 : 0;
        QString text = QString::number(faderDb, 'f', precision) + " dB";
        if (faderDb > 0)
            text = "+" + text;

        setToolTip(text);
    }
    else { // pan slider
        if (value() == 0) {
            setToolTip(tr("center"));
        }
        else {
            int percent = qAbs(static_cast<float>(value())/maximum() * 100);
            QString percentualText = QString::number(percent);
            if (value() < 0)
                setToolTip(percentualText + "% " + tr("L"));
            else
                setToolTip(percentualText + "% " + tr("R"));
        }
    }
}

void Slider::showToolTip()
{
    updateToolTipValue();

    QPoint pos = QCursor::pos();
    QString text = toolTip();
    QString currentToolTipText= QToolTip::text();
    if (currentToolTipText != text || !QToolTip::isVisible()) {
        QToolTip::showText(pos, text, this);
    }
}

void Slider::paintEvent(QPaintEvent *ev)
{
    {
        QPainter painter(this);
        drawMarker(painter);
    }

    QSlider::paintEvent(ev);
}

void Slider::mouseDoubleClickEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);

    if (sliderType == Slider::AudioSlider)
        setValue(100); // set to unit gain
    else
        setValue(0); // set pan slider to center
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
        updateToolTipValue();
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
