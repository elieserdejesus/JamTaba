#include "PeakMeter.h"
#include "Utils.h"
#include <QDebug>
#include <QResizeEvent>
#include <QDateTime>
#include <QPainter>
#include <QStyle>

const int BaseMeter::LINES_MARGIN = 3;
const int BaseMeter::MIN_SIZE = 1;
const int BaseMeter::DEFAULT_DECAY_TIME = 2000;

const int AudioMeter::MAX_PEAK_MARKER_SIZE = 2;
const int AudioMeter::MAX_PEAK_SHOW_TIME = 1500;

bool AudioMeter::paintingMaxPeakMarker = true;
bool AudioMeter::paintingPeaks = true;
bool AudioMeter::paintingRMS = true;

const quint8 BaseMeter::SEGMENTS_SIZE = 6;

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

void BaseMeter::resizeEvent(QResizeEvent * /*ev*/)
{
    recreateInterpolatedColors();

    update();
}


void BaseMeter::paintSegments(QPainter &painter, float peakValue, const std::vector<QColor> &segmentsColors, int offset, bool halfSize)
{
    const quint32 segmentsToPaint = (quint32)peakValue /SEGMENTS_SIZE;

    if (segmentsColors.size() < segmentsToPaint)
        return;

    const bool isVerticalMeter = isVertical();

    int x = isVerticalMeter ? offset : 0;
    int y = isVerticalMeter ? (height() - SEGMENTS_SIZE) : offset;
    const int w = isVerticalMeter ? (halfSize ? width()/2 : width()) : SEGMENTS_SIZE - 1;
    const int h = isVerticalMeter ? (SEGMENTS_SIZE - 1) : (halfSize ? height()/2 : height() - 2);
    for (quint32 i = 0; i < segmentsToPaint; ++i) {
        painter.fillRect(x, y, w, h, segmentsColors[i]);
        if (isVerticalMeter)
            y -= SEGMENTS_SIZE;
        else
            x += SEGMENTS_SIZE;
    }
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

//--------------------------------------------------------------------------------
AudioMeter::AudioMeter(QWidget *parent)
    :
      BaseMeter(parent),
      currentPeak(0.0f),
      currentRms(0.0f),
      maxPeak(0),
      lastMaxPeakTime(0),
      rmsColor(QColor(255, 255, 255, 200)),
      peakStartColor(Qt::darkGreen),
      peakEndColor(Qt::red),
      maxPeakColor(QColor(0, 0, 0, 80))
{
    setAttribute(Qt::WA_NoBackground);
    setAutoFillBackground(false);
}

void AudioMeter::setOrientation(Qt::Orientation orientation)
{
    BaseMeter::setOrientation(orientation);
    update();
}


QColor AudioMeter::interpolateColor(const QColor &start, const QColor &end, float ratio)
{
    int r = (int)(ratio * end.red()   + (1 - ratio) * start.red());
    int g = (int)(ratio * end.green() + (1 - ratio) * start.green());
    int b = (int)(ratio * end.blue()  + (1 - ratio) * start.blue());

    return QColor::fromRgb(r, g, b);
}

void AudioMeter::setRmsColor(const QColor &newColor){
    this->rmsColor = newColor;
    recreateInterpolatedColors();
    update();
}

void AudioMeter::setMaxPeakColor(const QColor &newColor)
{
    this->maxPeakColor = newColor;
    recreateInterpolatedColors();
    update();
}

void AudioMeter::setPeaksStartColor(const QColor &newColor)
{
    this->peakStartColor = newColor;
    recreateInterpolatedColors();
    update();
}

void AudioMeter::setPeaksEndColor(const QColor &newColor)
{
    this->peakEndColor = newColor;
    recreateInterpolatedColors();
    update();
}

void AudioMeter::recreateInterpolatedColors()
{
    // rebuild the peak and RMS colors vector
    peakColors.clear();
    rmsColors.clear();

    const quint32 size = isVertical() ? height() : width();
    const quint32 segments = size/SEGMENTS_SIZE;
    const int rmsInitialAlpha = rmsColor.alpha() * 0.6; // interpolate rms colors alpha from 60% to 100%

    for (quint32 i = 0; i < segments; ++i) {
        float interpolationPosition = std::pow(((float)i/segments), 2);
        peakColors.push_back(interpolateColor(peakStartColor, peakEndColor, interpolationPosition));

        QColor newRmsColor(rmsColor);
        int newAlpha = (float)i/segments * rmsColor.alpha();
        rmsColor.setAlpha(qMin(newAlpha + rmsInitialAlpha, 255));
        rmsColors.push_back(newRmsColor);
    }
}

void AudioMeter::paintMaxPeakMarker(QPainter &painter, bool halfSize)
{
    const bool isVerticalMeter = isVertical();
    float linearPeak = Utils::poweredGainToLinear(maxPeak);
    QRect maxPeakRect(isVerticalMeter ? 0 : (linearPeak * width()),
                   isVerticalMeter ? (height() - linearPeak * height()) : 0,
                   isVerticalMeter ? width() : MAX_PEAK_MARKER_SIZE,
                   isVerticalMeter ? MAX_PEAK_MARKER_SIZE : height());

    if (halfSize) {
        if (isVerticalMeter)
            maxPeakRect.setWidth(maxPeakRect.width()/2);
        else
            maxPeakRect.setHeight(maxPeakRect.height()/2);
    }


    painter.fillRect(maxPeakRect, maxPeakColor);
}

void AudioMeter::updateInternalValues()
{
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

void AudioMeter::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if (isEnabled()) {

        const bool halfSizePainting = paintingPeaks && paintingRMS;
        const int rectSize = isVertical() ? height() : width();

        if (currentPeak && paintingPeaks) {
            const int offset = paintingRMS ? 0 : 1;
            float peakValue = Utils::poweredGainToLinear(currentPeak) * rectSize;
            paintSegments(painter, peakValue, peakColors, offset, halfSizePainting);
        }

        if (currentRms && paintingRMS) {
            const int offset = paintingPeaks ? (isVertical() ? width()/2 : height()/2) : 1;
            float rmsValue = Utils::poweredGainToLinear(currentRms) * rectSize;
            paintSegments(painter, rmsValue, rmsColors, offset + 1, halfSizePainting);
        }

        if (maxPeak && paintingMaxPeakMarker)
            paintMaxPeakMarker(painter, halfSizePainting);
    }

    updateInternalValues(); // compute decay and max peak
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
      midiActivityColor(Qt::red)
{

}

void MidiActivityMeter::recreateInterpolatedColors()
{
    colors.clear();

    const quint32 size = isVertical() ? height() : width();
    const quint32 segments = size/SEGMENTS_SIZE;
    const int initialAlpha = midiActivityColor.alpha() * 0.6; // interpolate colors alpha from 60% to 100%

    for (quint32 i = 0; i < segments; ++i) {
        int newAlpha = (float)i/segments * midiActivityColor.alpha();
        QColor newColor(midiActivityColor);
        newColor.setAlpha(qMin(newAlpha + initialAlpha, 255));
        colors.push_back(newColor);
    }
}

void MidiActivityMeter::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if (isEnabled()) {
        float value = (isVertical() ? height() : width()) * activityValue;
        paintSegments(painter, value, colors);
        updateInternalValues();
    }
}

void MidiActivityMeter::updateInternalValues()
{
    quint64 now = QDateTime::currentMSecsSinceEpoch();

    // decay
    long ellapsedTimeFromLastUpdate = now - lastUpdate;
    float deltaTime = (float)ellapsedTimeFromLastUpdate/decayTime;
    activityValue -= deltaTime;

    if (activityValue < 0)
        activityValue = 0;

    lastUpdate = now;
}

void MidiActivityMeter::setSolidColor(const QColor &color)
{
    this->midiActivityColor = color;
    update();
}

void MidiActivityMeter::setActivityValue(float value)
{
    this->activityValue = limitFloatValue(value);
}
