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
const quint8 BaseMeter::SEGMENTS_SIZE = 6;


const int AudioMeter::MAX_PEAK_MARKER_SIZE = 2;
const int AudioMeter::MAX_PEAK_SHOW_TIME = 1500;

const float AudioMeter::MAX_DB_VALUE = 6.0f;
const float AudioMeter::MAX_LINEAR_VALUE = Utils::dbToLinear(AudioMeter::MAX_DB_VALUE);
const float AudioMeter::MAX_SMOOTHED_LINEAR_VALUE = AudioMeter::getSmoothedLinearPeakValue(AudioMeter::MAX_LINEAR_VALUE);

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

void BaseMeter::resizeEvent(QResizeEvent * /*ev*/)
{
    recreateInterpolatedColors();

    update();
}


void BaseMeter::paintSegments(QPainter &painter, const QRect &rect, float peakValue, const std::vector<QColor> &segmentsColors)
{
    const quint32 segmentsToPaint = (quint32)peakValue /SEGMENTS_SIZE;

    if (segmentsColors.size() < segmentsToPaint)
        return;

    const bool isVerticalMeter = isVertical();

    int x = rect.left();
    int y = isVerticalMeter ? (rect.height() - SEGMENTS_SIZE) : rect.top();
    const int w = isVerticalMeter ? rect.width() - 1 : SEGMENTS_SIZE - 1;
    const int h = isVerticalMeter ? (SEGMENTS_SIZE - 1) : rect.height() - 1;

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
      rmsColor(QColor(255, 255, 255, 200)),
      peakStartColor(Qt::darkGreen),
      peakEndColor(Qt::red),
      maxPeakColor(QColor(0, 0, 0, 80)),
      dBMarksColor(Qt::lightGray),
      stereo(true)
{
    //setAttribute(Qt::WA_NoBackground);
    setAutoFillBackground(false);

    for (int i = 0; i < 2; ++i) {
        currentPeak[i] = 0.0f;
        maxPeak[i] = 0.0f;
        currentRms[i] = 0.0f;
        lastMaxPeakTime[i] = 0;
    }
}

void AudioMeter::setStereo(bool stereo)
{
    if (this->stereo != stereo) {
        this->stereo = stereo;
        update();
    }
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

void AudioMeter::setDbMarksColor(const QColor &newColor)
{
    this->dBMarksColor = newColor;

    update();
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

void AudioMeter::paintMaxPeakMarker(QPainter &painter, float maxLinearPeak, const QRect &rect)
{
    const bool isVerticalMeter = isVertical();
    QRect maxPeakRect(isVerticalMeter ? rect.left() : (rect.left() + maxLinearPeak * rect.width() - MAX_PEAK_MARKER_SIZE),
                   isVerticalMeter ? (height() - maxLinearPeak * height()) : rect.top(),
                   isVerticalMeter ? rect.width() - 1 : MAX_PEAK_MARKER_SIZE,
                   isVerticalMeter ? MAX_PEAK_MARKER_SIZE : rect.height());

    painter.fillRect(maxPeakRect, maxPeakColor);
}

void AudioMeter::updateInternalValues()
{
    quint64 now = QDateTime::currentMSecsSinceEpoch();

    // decay
    long ellapsedTimeFromLastUpdate = now - lastUpdate;
    float deltaTime = (float)ellapsedTimeFromLastUpdate/decayTime;

    for (int i = 0; i < 2; ++i) {
        currentPeak[i] -= deltaTime;
        currentRms[i]  -= deltaTime;

        if (currentPeak[i] < 0)
            currentPeak[i] = 0;

        if (currentRms[i] < 0)
            currentRms[i] = 0;
    }

    lastUpdate = now;

    // max peak
    for (uint i = 0; i < 2; ++i) {
        long ellapsedTimeFromLastMaxPeak = now - lastMaxPeakTime[i];
        if (ellapsedTimeFromLastMaxPeak >= MAX_PEAK_SHOW_TIME)
            maxPeak[i] = 0;
    }
}

uint AudioMeter::getParallelSegments() const
{
    uint parallelSegments = 0;
    if (isPaintingPeaks())
        parallelSegments++;

    if (isPaintingRMS())
        parallelSegments++;

    if (stereo)
        parallelSegments *= 2;

    return parallelSegments;
}

void AudioMeter::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    const static float peakValuesOffset = MAX_SMOOTHED_LINEAR_VALUE - 1.0f; // peaks are not starting at 0dB, so we need a offset

    if (isEnabled()) {
        const uint channels = stereo ? 2 : 1;
        const int rectSize = isVertical() ? height() : width();
        QRect drawRect(rect());

        const int parallelSegments = getParallelSegments();

        if (isVertical())
            drawRect.setWidth(width()/parallelSegments);
        else
            drawRect.setHeight(height()/parallelSegments);

        if (paintingPeaks) {
            for (uint i = 0; i < channels; ++i) {
                if (currentPeak[i]) {
                    float smoothedPeakValue = getSmoothedLinearPeakValue(currentPeak[i]) - peakValuesOffset ;
                    float peakValue = smoothedPeakValue * rectSize;
                    paintSegments(painter, drawRect, peakValue, peakColors);
                }

                if (maxPeak[i] && paintingMaxPeakMarker) {
                    float maxPeakValue = getSmoothedLinearPeakValue(maxPeak[i]) - peakValuesOffset;
                    paintMaxPeakMarker(painter, maxPeakValue, drawRect);
                }

                if (isVertical())
                    drawRect.translate(drawRect.width(), 0);
                else
                    drawRect.translate(0, drawRect.height());
            }
        }

        if (paintingRMS) {
            for (uint i = 0; i < channels; ++i) {
                if (currentRms[i]) {
                    float rmsValue = (getSmoothedLinearPeakValue(currentRms[i]) - peakValuesOffset) * rectSize;
                    paintSegments(painter, drawRect, rmsValue, rmsColors);
                }
                if (isVertical())
                    drawRect.translate(drawRect.width(), 0);
                else
                    drawRect.translate(0, drawRect.height());
            }
        }

        drawDbMarkers(painter);
    }

    updateInternalValues(); // compute decay and max peak
}

void AudioMeter::drawDbMarkers(QPainter &painter)
{
    float db = MAX_DB_VALUE;

    painter.setPen(dBMarksColor);
    QFontMetrics metrics = fontMetrics();
    qreal fontHeight = metrics.height();
    qreal fontAscent = metrics.descent();
    qreal center = isVertical() ? width()/2.0 : height()/2.0;

    const bool drawTicks = !isVertical();
    const static float tickHeight = 2.5f;
    float tickY = height() - 1 - tickHeight;

    while (db > -64) {
        QString text = QString::number(db);
        if (db > 0)
            text = "+" + text;
        int textWidth = metrics.width(text);

        float linearValue = getSmoothedLinearPeakValue(Utils::dbToLinear(db));

        float y = (isVertical() ? (MAX_SMOOTHED_LINEAR_VALUE - linearValue) * height() : center) + fontHeight/2.0 - fontAscent;
        float x = (isVertical() ? center - textWidth/2.0 : (1 - (MAX_SMOOTHED_LINEAR_VALUE - linearValue)) * width() - textWidth);
        painter.drawText(x, y, text);

        if (drawTicks) {
            float tickX = (1 - (MAX_SMOOTHED_LINEAR_VALUE - linearValue)) * width() - 1;
            painter.drawLine(tickX, tickY, tickX, tickY + tickHeight);
        }

        db -= 6;
    }
}

void AudioMeter::setPeak(float peak, float rms)
{
    peak = limitFloatValue(peak, 0.0f, AudioMeter::MAX_LINEAR_VALUE);
    rms = limitFloatValue(rms, 0.0f, AudioMeter::MAX_LINEAR_VALUE);

    if (peak > currentPeak[0] || peak > currentPeak[1]) {
        currentPeak[0] = currentPeak[1] = peak;
        if (peak > maxPeak[0] || peak > maxPeak[1]) {
            maxPeak[0] = maxPeak[1] = peak;
            lastMaxPeakTime[0] = lastMaxPeakTime[1] = QDateTime::currentMSecsSinceEpoch();
        }
    }

    if (rms > currentRms[0] || rms > currentRms[1])
        currentRms[0] = currentRms[1] = rms;

    update();
}


void AudioMeter::setPeak(float leftPeak, float rightPeak, float leftRms, float rightRms)
{
    leftPeak = limitFloatValue(leftPeak, 0.0f, AudioMeter::MAX_LINEAR_VALUE);
    rightPeak = limitFloatValue(rightPeak, 0.0f, AudioMeter::MAX_LINEAR_VALUE);

    leftRms = limitFloatValue(leftRms, 0.0f, AudioMeter::MAX_LINEAR_VALUE);
    rightRms = limitFloatValue(rightRms, 0.0f, AudioMeter::MAX_LINEAR_VALUE);

    float peaks[2] = {leftPeak, rightPeak};
    for (int i = 0; i < 2; ++i) {
        if (peaks[i] > currentPeak[i]) {
            currentPeak[i] = peaks[i];
            if (peaks[i] > maxPeak[i]) {
                maxPeak[i] = peaks[i];
                lastMaxPeakTime[i] = QDateTime::currentMSecsSinceEpoch();
            }
        }
    }

    float rms[2] = {leftRms, rightRms};
    for (int i = 0; i < 2; ++i) {
        if (rms[i] > currentRms[i])
            currentRms[i] = rms[i];
    }

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
        paintSegments(painter, rect(), value, colors);
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
