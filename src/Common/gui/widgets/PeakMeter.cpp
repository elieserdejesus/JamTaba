#include "PeakMeter.h"
#include "Utils.h"
#include <QDebug>
#include <QResizeEvent>
#include <QDateTime>
#include <QPainter>
#include <QStyle>

const int BaseMeter::LINES_MARGIN = 3;
const int BaseMeter::MIN_SIZE = 1;
const int BaseMeter::DEFAULT_DECAY_TIME = 3000;

const QColor AudioMeter::GRADIENT_FIRST_COLOR = QColor(255, 0, 0); //red
const QColor AudioMeter::GRADIENT_MIDDLE_COLOR = QColor(0, 220, 0); //mid green
const QColor AudioMeter::GRADIENT_LAST_COLOR = QColor(0, 100, 0); //dark green
const QColor AudioMeter::MAX_PEAK_COLOR = QColor(0, 0, 0, 80);
const QColor AudioMeter::RMS_COLOR = QColor(255, 255, 255, 120);

const int AudioMeter::MAX_PEAK_MARKER_SIZE = 2;
const int AudioMeter::MAX_PEAK_SHOW_TIME = 1500;

bool AudioMeter::paintingMaxPeakMarker = true;
bool AudioMeter::paintingPeaks = true;
bool AudioMeter::paintingRMS = true;

BaseMeter::BaseMeter(QWidget *parent) :
    QFrame(parent),
    lastUpdate(QDateTime::currentMSecsSinceEpoch()),
    decayTime(DEFAULT_DECAY_TIME),
    orientation(Qt::Vertical)
{
    //setAttribute(Qt::WA_NoBackground);
    //update();
}

BaseMeter::~BaseMeter()
{

}

QSize BaseMeter::minimumSizeHint() const
{
    bool isVerticalMeter = isVertical();
    int w = isVerticalMeter ? MIN_SIZE : width();
    int h = isVerticalMeter ? height() : MIN_SIZE;
    return QSize(w, h);
}

void BaseMeter::setOrientation(Qt::Orientation orientation)
{
    this->orientation = orientation;
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void BaseMeter::setDecayTime(quint32 decayTimeInMiliseconds)
{
    this->decayTime = decayTimeInMiliseconds;
    update();
}


float BaseMeter::limitFloatValue(float value, float minValue, float maxValue)
{
    if (value < minValue)
        return minValue;

    if( value > maxValue)
        return maxValue;

    return value;
}

QRectF BaseMeter::getPaintRect(float peakValue) const
{
    bool isVerticalMeter = isVertical();
    return QRectF(isVerticalMeter ? 1.0f : 0.0f,                // x
               isVerticalMeter ? height() - peakValue : 1.0f,   // y
               isVerticalMeter ? width()-2.0f : peakValue,      // width
               height() - (isVerticalMeter ? 0.0f : 2.0f));     // height
}

//--------------------------------------------------------------------------------
AudioMeter::AudioMeter(QWidget *parent)
    :
      BaseMeter(parent),
      currentPeak(0.0f),
      currentRms(0.0f),
      maxPeak(0),
      lastMaxPeakTime(0)
{

}

void AudioMeter::setOrientation(Qt::Orientation orientation)
{
    BaseMeter::setOrientation(orientation);
    this->gradient = createGradient();
    update();
}


void AudioMeter::resizeEvent(QResizeEvent * /*ev*/)
{
    gradient = createGradient();
}

QLinearGradient AudioMeter::createGradient()
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

void AudioMeter::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // meter
    if (isEnabled()) {
        bool isVerticalMeter = isVertical();
        int rectSize = isVerticalMeter ? height() : width();

        if (currentPeak && paintingPeaks) {
            float peakValue = Utils::poweredGainToLinear(currentPeak) * rectSize;
            painter.fillRect(getPaintRect(peakValue), gradient);
        }

        //draw the rms rect in the top layer
        if (currentRms && paintingRMS) {
            float rmsValue = Utils::poweredGainToLinear(currentRms) * rectSize;
            QRectF rmsRect = getPaintRect(rmsValue);
            if (paintingPeaks)
                painter.fillRect(rmsRect, RMS_COLOR); //paint the "transparent white" rect to highlight the rms meter
            else
                painter.fillRect(rmsRect, gradient);
        }

        // draw max peak marker
        if (maxPeak > 0 && paintingMaxPeakMarker) {
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

void AudioMeter::setPeak(float peak, float rms)
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



void AudioMeter::setPaintMaxPeakMarker(bool paintMaxPeak)
{
    AudioMeter::paintingMaxPeakMarker = paintMaxPeak;
}

void AudioMeter::paintRmsOnly()
{
    paintingRMS = true;
    paintingPeaks = false;
}

void AudioMeter::paintPeaksOnly()
{
    paintingRMS = false;
    paintingPeaks = true;
}

void AudioMeter::paintPeaksAndRms()
{
    paintingRMS = true;
    paintingPeaks = true;
}

bool AudioMeter::isPaintingPeaksOnly()
{
    return paintingPeaks && !paintingRMS;
}

bool AudioMeter::isPaintingRmsOnly()
{
    return paintingRMS && !paintingPeaks;
}

//--------------------------------------------

MidiActivityMeter::MidiActivityMeter(QWidget *parent)
    : BaseMeter(parent),
      solidColor(Qt::red)
{

}

void MidiActivityMeter::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // meter
    if (isEnabled()) {
        float value = (isVertical() ? height() : width()) * activityValue;
        painter.fillRect(getPaintRect(value), solidColor);

        quint64 now = QDateTime::currentMSecsSinceEpoch();

        // decay
        long ellapsedTimeFromLastUpdate = now - lastUpdate;
        float deltaTime = (float)ellapsedTimeFromLastUpdate/decayTime;
        activityValue -= deltaTime;

        if (activityValue < 0)
            activityValue = 0;

        lastUpdate = now;
    }
}

void MidiActivityMeter::setSolidColor(const QColor &color)
{
    this->solidColor = color;
    update();
}

void MidiActivityMeter::setActivityValue(float value)
{
    this->activityValue = limitFloatValue(value);
}
