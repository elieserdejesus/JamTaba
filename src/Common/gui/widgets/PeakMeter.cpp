#include "PeakMeter.h"
#include "Utils.h"
#include <QDebug>
#include <QResizeEvent>
#include <QDateTime>
#include <QPainter>
#include <QStyle>

const int PeakMeter::LINES_MARGIN = 3;
const int PeakMeter::MIN_SIZE = 1;

const QColor PeakMeter::GRADIENT_FIRST_COLOR = QColor(255, 0, 0); //red
const QColor PeakMeter::GRADIENT_MIDDLE_COLOR = QColor(0, 220, 0); //mid green
const QColor PeakMeter::GRADIENT_LAST_COLOR = QColor(0, 100, 0); //dark green
const QColor PeakMeter::MAX_PEAK_COLOR = QColor(0, 0, 0, 80);
const QColor PeakMeter::RMS_COLOR = QColor(255, 255, 255, 120);

const int PeakMeter::MAX_PEAK_MARKER_SIZE = 2;
const int PeakMeter::DEFAULT_DECAY_TIME = 3000;
const int PeakMeter::MAX_PEAK_SHOW_TIME = 1500;

PeakMeter::PeakMeter(QWidget *) :
    currentPeak(0),
    maxPeak(0),
    lastUpdate(QDateTime::currentMSecsSinceEpoch()),
    usingGradient(true),
    paintingMaxPeak(true),
    decayTime(DEFAULT_DECAY_TIME),
    orientation(Qt::Vertical),
    lastMaxPeakTime(0)
{
    gradient = createGradient();
    setAttribute(Qt::WA_NoBackground);
    update();
}

QSize PeakMeter::minimumSizeHint() const
{
    bool isVerticalMeter = isVertical();
    int w = isVerticalMeter ? MIN_SIZE : width();
    int h = isVerticalMeter ? height() : MIN_SIZE;
    return QSize(w, h);
}

void PeakMeter::setOrientation(Qt::Orientation orientation)
{
    this->orientation = orientation;
    this->gradient = createGradient();
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void PeakMeter::resizeEvent(QResizeEvent * /*ev*/)
{
    gradient = createGradient();
}

QLinearGradient PeakMeter::createGradient()
{
    int x1 = isVertical() ? 0 : width()-1;
    int y1 = 0;
    int x2 = 0;
    int y2 = isVertical() ? height()-1 : 0;
    QLinearGradient linearGradient(x1, y1, x2, y2);
    linearGradient.setColorAt(0, GRADIENT_FIRST_COLOR);
    linearGradient.setColorAt(0.3, GRADIENT_MIDDLE_COLOR);
    linearGradient.setColorAt(0.8, GRADIENT_LAST_COLOR);
    return linearGradient;
}

void PeakMeter::setDecayTime(quint32 decayTimeInMiliseconds)
{
    this->decayTime = decayTimeInMiliseconds;
    update();
}

void PeakMeter::setPaintMaxPeakMarker(bool paintMaxPeak)
{
    this->paintingMaxPeak = paintMaxPeak;
    update();
}

void PeakMeter::setSolidColor(const QColor &color)
{
    this->usingGradient = false;
    this->solidColor = color;
    update();
}

float PeakMeter::limitFloatValue(float value, float minValue, float maxValue)
{
    if (value < minValue)
        return minValue;

    if( value > maxValue)
        return maxValue;

    return value;
}

void PeakMeter::setPeak(float peak, float rms)
{
    peak = limitFloatValue(peak);
    rms = limitFloatValue(rms);

    if (peak > currentPeak) {
        currentPeak = peak;
        if (peak > maxPeak) {
            maxPeak = peak;
            lastMaxPeakTime = QDateTime::currentMSecsSinceEpoch();
        }
    }

    if (rms > currentRms)
        currentRms = rms;

    update();
}

QRectF PeakMeter::getPaintRect(float peakValue) const
{
    bool isVerticalMeter = isVertical();
    return QRectF(isVerticalMeter ? 1.0f : 0.0f,                // x
               isVerticalMeter ? height() - peakValue : 1.0f,   // y
               isVerticalMeter ? width()-2.0f : peakValue,      // width
               height() - (isVerticalMeter ? 0.0f : 2.0f));     // height
}

void PeakMeter::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // meter
    if (isEnabled()) {
        bool isVerticalMeter = isVertical();
        int rectSize = isVerticalMeter ? height() : width();
        float peakValue = Utils::poweredGainToLinear(currentPeak) * rectSize;

        painter.fillRect(getPaintRect(peakValue), usingGradient ? gradient : QBrush(solidColor));

        //draw the rms rect in the top layer
        if (currentRms) {
            float rmsValue = Utils::poweredGainToLinear(currentRms) * rectSize;
            QRectF rmsRect = getPaintRect(rmsValue);
            painter.fillRect(rmsRect, RMS_COLOR);

            //draw a rms mark
            painter.setPen(MAX_PEAK_COLOR);
            QPointF firstPoint  = isVerticalMeter ? rmsRect.topLeft() : rmsRect.topRight();
            QPointF secondPoint = isVerticalMeter ? rmsRect.topRight() : rmsRect.bottomRight();
            painter.drawLine(firstPoint, secondPoint);
        }

        // draw max peak marker
        if (maxPeak > 0 && paintingMaxPeak) {
            float linearPeak = Utils::poweredGainToLinear(maxPeak);
            QRect maxPeakRect(isVerticalMeter ? 0 : (linearPeak * width()),
                           isVerticalMeter ? (height() - linearPeak * height()) : 0,
                           isVerticalMeter ? width() : MAX_PEAK_MARKER_SIZE,
                           isVerticalMeter ? MAX_PEAK_MARKER_SIZE : height());
            painter.fillRect(maxPeakRect, MAX_PEAK_COLOR);
        }
    }

    quint64 now = QDateTime::currentMSecsSinceEpoch();

    // decay
    long ellapsedTimeFromLastUpdate = now - lastUpdate;
    float deltaTime = (float)ellapsedTimeFromLastUpdate/decayTime;
    currentPeak -= deltaTime;
    currentRms  -= deltaTime;

    if (currentPeak < 0)
        currentPeak = 0;

    if (currentRms < 0)
        currentRms = 0;

    lastUpdate = now;

    // max peak
    long ellapsedTimeFromLastMaxPeak = now - lastMaxPeakTime;
    if (ellapsedTimeFromLastMaxPeak >= MAX_PEAK_SHOW_TIME)
        maxPeak = 0;
}
