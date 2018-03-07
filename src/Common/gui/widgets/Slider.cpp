#include "Slider.h"
#include "Utils.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QToolTip>

AudioSlider::AudioSlider(QWidget *parent) :
    QSlider(parent)
{
    setMaximum(120);

    connect(this, &AudioSlider::valueChanged, this, &AudioSlider::showToolTip);
}

void AudioSlider::updateToolTipValue()
{
    double poweredGain = Utils::linearGainToPower(value()/100.0);
    double faderDb = Utils::linearToDb(poweredGain);
    int precision = faderDb > -10 ? 1 : 0;
    QString text = QString::number(faderDb, 'f', precision) + " dB";
    if (faderDb > 0)
        text = "+" + text;

    setToolTip(text);
}

void AudioSlider::showToolTip()
{
    updateToolTipValue();

    QPoint pos = QCursor::pos();
    QString text = toolTip();
    QString currentToolTipText= QToolTip::text();
    if (currentToolTipText != text || !QToolTip::isVisible()) {
        QToolTip::showText(pos, text, this);
    }
}

void AudioSlider::paintEvent(QPaintEvent *ev)
{
    {
        QPainter painter(this);
        drawMarker(painter);
    }

    QSlider::paintEvent(ev);
}

void AudioSlider::mouseDoubleClickEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);

    setValue(100); // set to unit gain
}

qreal AudioSlider::getMarkerPosition() const
{
    qreal position = 0;
    if (orientation() == Qt::Vertical)
        position = 1 - 100.0/maximum();
    else
        position = 100.0/maximum();

    return position;
}

void AudioSlider::drawMarker(QPainter &painter)
{
//    if (sliderType == Slider::AudioSlider && maximum() <= 100)
//        return;

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

    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect rectHandle = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, NULL);

    if (orientation() == Qt::Vertical)
        line.translate(0.0, rectHandle.height()/2.0);
    else
        line.translate(-rectHandle.width()/2.0, 0.0);

    painter.drawLine(line);
}

// --------------------------------------------------------------------------

PanSlider::PanSlider(QWidget *parent) :
    QSlider(parent)
{
    connect(this, &PanSlider::valueChanged, this, &PanSlider::showToolTip);

    setMinimum(-4);
    setMaximum(4);
}

void PanSlider::updateToolTipValue()
{
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

void PanSlider::mouseDoubleClickEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);

    setValue(0); // set pan slider to center
}

void PanSlider::paintEvent(QPaintEvent *ev)
{
    {
        QPainter painter(this);
        drawMarker(painter);
    }

    QSlider::paintEvent(ev);
}

void PanSlider::drawMarker(QPainter &painter)
{
    static const QColor MARKER_COLOR(0, 0, 0, 80);
    painter.setPen(MARKER_COLOR);

    const qreal markerSize = height();
    const qreal center = height()/2.0;

    const qreal markerPosition = 0.5;

    qreal x1, x2, y1, y2;
    y1 = center - markerSize/2.0;
    y2 = center + markerSize/2.0;
    x1 = x2 = markerPosition * width();

    QLine line(x1, y1, x2, y2);

    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect rectHandle = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, NULL);

    line.translate(-rectHandle.width()/2.0, 0.0);

    painter.drawLine(line);
}

void PanSlider::showToolTip()
{
    updateToolTipValue();

    QPoint pos = QCursor::pos();
    QString text = toolTip();
    QString currentToolTipText= QToolTip::text();
    if (currentToolTipText != text || !QToolTip::isVisible()) {
        QToolTip::showText(pos, text, this);
    }
}
