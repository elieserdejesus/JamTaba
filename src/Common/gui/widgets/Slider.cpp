#include "Slider.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QToolTip>
#include <QDateTime>
#include <QDebug>
#include <QObjectData>

const quint8 AudioSlider::SEGMENTS_SIZE = 6;

const float AudioSlider::MIN_DB_VALUE = -60.0f;
const float AudioSlider::MIN_LINEAR_VALUE = Utils::dbToLinear(AudioSlider::MIN_DB_VALUE);

const int AudioSlider::MAX_PEAK_MARKER_SIZE = 2;
const int AudioSlider::MAX_PEAK_SHOW_TIME = 1500;
const int AudioSlider::MIN_SIZE = 12;

const uint AudioSlider::DEFAULT_DECAY_TIME = 2000;

bool AudioSlider::paintingMaxPeakMarker = true;
bool AudioSlider::paintingPeaks = true;
bool AudioSlider::paintingRMS = true;

AudioSlider::AudioSlider(QWidget *parent) :
    QSlider(parent),
    lastUpdate(QDateTime::currentMSecsSinceEpoch()),
    decayTime(DEFAULT_DECAY_TIME),
    rmsColor(QColor(255, 255, 255, 200)),
    peakStartColor(Qt::darkGreen),
    peakEndColor(Qt::red),
    maxPeakColor(QColor(0, 0, 0, 80)),
    dBMarksColor(Qt::lightGray),
    stereo(true),
    paintingDbMarkers(true),
    drawSegments(true)
{
    setMaximum(120);

    connect(this, &AudioSlider::valueChanged, this, &AudioSlider::showToolTip);
}

void AudioSlider::paintRmsOnly()
{
    paintingRMS = true;
    paintingPeaks = false;
}

void AudioSlider::paintPeaksOnly()
{
    paintingRMS = false;
    paintingPeaks = true;
}

void AudioSlider::paintPeaksAndRms()
{
    paintingRMS = true;
    paintingPeaks = true;
}

bool AudioSlider::isPaintingPeaksOnly()
{
    return paintingPeaks && !paintingRMS;
}

bool AudioSlider::isPaintingRmsOnly()
{
    return paintingRMS && !paintingPeaks;
}

void AudioSlider::setPeak(float peak, float rms)
{
    auto maxLinearValue = getMaxLinearValue();

    peak = limitFloatValue(peak, 0.0f, maxLinearValue);
    rms = limitFloatValue(rms, 0.0f, maxLinearValue);

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


void AudioSlider::setPeak(float leftPeak, float rightPeak, float leftRms, float rightRms)
{
    auto maxLinearValue = getMaxLinearValue();

    leftPeak = limitFloatValue(leftPeak, 0.0f, maxLinearValue);
    rightPeak = limitFloatValue(rightPeak, 0.0f, maxLinearValue);

    leftRms = limitFloatValue(leftRms, 0.0f, maxLinearValue);
    rightRms = limitFloatValue(rightRms, 0.0f, maxLinearValue);

    float peaks[2] = {leftPeak, rightPeak};
    for (int i = 0; i < 2; ++i) {
        if (!stereo) // fixing #858
            peaks[0] = peaks[1] = qMax(leftPeak, rightPeak);

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
        if (!stereo) // fixing #858
            rms[0] = rms[1] = qMax(leftRms, rightRms);
        if (rms[i] > currentRms[i])
            currentRms[i] = rms[i];
    }

    update();
}


float AudioSlider::limitFloatValue(float value, float minValue, float maxValue)
{
    if (value < minValue)
        return minValue;

    if( value > maxValue)
        return maxValue;

    return value;
}

void AudioSlider::updateToolTipValue()
{
    qreal poweredGain = Utils::linearGainToPower(value()/100.0);
    qreal faderDb = Utils::linearToDb(poweredGain);
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

void AudioSlider::paintSliderGroove(QPainter &painter)
{
    // draw the slider handle only
    QStyleOptionSlider optHandle;
    initStyleOption(&optHandle);

    optHandle.subControls = QStyle::SC_SliderGroove;

    style()->drawComplexControl(QStyle::CC_Slider, &optHandle, &painter, this);
}

void AudioSlider::paintSliderHandler(QPainter &painter)
{
    // draw the slider handle only
    QStyleOptionSlider optHandle;
    initStyleOption(&optHandle);

    optHandle.subControls = QStyle::SC_SliderHandle; //QStyle::SC_SliderGroove | QStyle::SC_SliderHandle;

    auto handleRect = style()->subControlRect(QStyle::CC_Slider, &optHandle, QStyle::SC_SliderHandle, this);

    auto pos = mapFromGlobal(QCursor::pos());
    if (handleRect.contains(pos)) // hover ?
        optHandle.activeSubControls = QStyle::SC_SliderHandle;

    style()->drawComplexControl(QStyle::CC_Slider, &optHandle, &painter, this);
}

void AudioSlider::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    paintSliderGroove(painter);

    if (isEnabled()) {

        const uint channels = stereo ? 2 : 1;
        const qreal rectSize = isVertical() ? height() : width();

        QStyleOptionSlider opt;
        initStyleOption(&opt);
        auto grooveRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
        QRectF drawRect(isVertical() ? grooveRect.adjusted(3, 0, -2, -2) : grooveRect.adjusted(3, 3, 0, -2));

        const int parallelSegments = getParallelSegments();

        if (isVertical())
            drawRect.setWidth(drawRect.width()/static_cast<qreal>(parallelSegments));
        else
            drawRect.setHeight(drawRect.height()/static_cast<qreal>(parallelSegments));

        for (uint i = 0; i < channels; ++i) {
            if (paintingPeaks && currentPeak[i]) {
                qreal peakPosition = getPeakPosition(currentPeak[i], rectSize);
                paintSegments(painter, drawRect, peakPosition, peakColors, drawSegments);
            }

            if (paintingMaxPeakMarker && maxPeak[i]) {
                qreal maxPeakPosition = getPeakPosition(maxPeak[i], rectSize);
                paintMaxPeakMarker(painter, maxPeakPosition, drawRect);
            }

            if (paintingRMS && currentRms[i]) {
                qreal rmsPosition = getPeakPosition(currentRms[i], rectSize);

                qreal rmsXOffset = (paintingPeaks && isVertical()) ? channels * drawRect.width() : 0;
                qreal rmsYOffset = (paintingPeaks && !isVertical()) ? channels * drawRect.height() : 0;
                paintSegments(painter, drawRect.translated(rmsXOffset, rmsYOffset), rmsPosition, rmsColors, drawSegments);
            }

            if (isVertical())
                drawRect.translate(drawRect.width(), 0.0);
            else
                drawRect.translate(0.0, drawRect.height());
        }

        if (paintingDbMarkers)
            painter.drawPixmap(0.0, 0.0, dbMarkersPixmap);
    }

    drawMarker(painter);

    paintSliderHandler(painter);


    updateInternalValues(); // compute decay and max peak
}

void AudioSlider::paintMaxPeakMarker(QPainter &painter, qreal maxPeakPosition, const QRectF &rect)
{
    const bool isVerticalMeter = isVertical();
    QRectF maxPeakRect(isVerticalMeter ? rect.left() : (rect.left() + maxPeakPosition - MAX_PEAK_MARKER_SIZE),
                   isVerticalMeter ? (height() - maxPeakPosition) : rect.top(),
                   isVerticalMeter ? rect.width() - 1 : MAX_PEAK_MARKER_SIZE,
                   isVerticalMeter ? MAX_PEAK_MARKER_SIZE : rect.height());

    painter.fillRect(maxPeakRect, maxPeakColor);
}

void AudioSlider::setStereo(bool stereo)
{
    if (this->stereo != stereo) {
        this->stereo = stereo;
        update();
    }
}

uint AudioSlider::getParallelSegments() const
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

void AudioSlider::updateInternalValues()
{
    quint64 now = QDateTime::currentMSecsSinceEpoch();

    // decay
    long ellapsedTimeFromLastUpdate = now - lastUpdate;
    float deltaTime = static_cast<float>(ellapsedTimeFromLastUpdate)/decayTime;

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

void AudioSlider::recreateInterpolatedColors()
{
    // rebuild the peak and RMS colors vector
    peakColors.clear();
    rmsColors.clear();

    const quint32 size = isVertical() ? height() : width();
    const quint32 segments = size/SEGMENTS_SIZE;

    if (segments == 0) // just in case
        return;

    const int rmsInitialAlpha = rmsColor.alpha() * 0.6; // interpolate rms colors alpha from 60% to 100%

    for (quint32 i = 0; i < segments; ++i) {
        float interpolationPosition = std::pow((static_cast<float>(i)/segments), 3.0);
        peakColors.push_back(interpolateColor(peakStartColor, peakEndColor, interpolationPosition));

        QColor newRmsColor(rmsColor);
        int newAlpha = static_cast<float>(i)/segments * rmsColor.alpha();
        rmsColor.setAlpha(qMin(newAlpha + rmsInitialAlpha, 255));
        rmsColors.push_back(newRmsColor);
    }
}


QColor AudioSlider::interpolateColor(const QColor &start, const QColor &end, float ratio)
{
    int r = static_cast<int>(ratio * end.red()   + (1 - ratio) * start.red());
    int g = static_cast<int>(ratio * end.green() + (1 - ratio) * start.green());
    int b = static_cast<int>(ratio * end.blue()  + (1 - ratio) * start.blue());

    return QColor::fromRgb(r, g, b);
}

void AudioSlider::paintSegments(QPainter &painter, const QRectF &rect, float peakPosition, const std::vector<QColor> &segmentsColors, bool drawSegments)
{
    const quint32 segmentsToPaint = (quint32)peakPosition/SEGMENTS_SIZE;

    if (segmentsColors.size() < segmentsToPaint)
        return;

    const bool isVerticalMeter = isVertical();

    const qreal pad = drawSegments ? 1.0 : 0;

    qreal x = rect.left();
    qreal y = isVerticalMeter ? (rect.height() - SEGMENTS_SIZE) : rect.top();
    const qreal w = isVerticalMeter ? rect.width() - pad : SEGMENTS_SIZE - pad;
    const qreal h = isVerticalMeter ? (SEGMENTS_SIZE - pad) : rect.height() - pad;

    for (quint32 i = 0; i < segmentsToPaint; ++i) {
        painter.fillRect(x, y, w, h, segmentsColors[i]);
        if (isVerticalMeter)
            y -= SEGMENTS_SIZE;
        else
            x += SEGMENTS_SIZE;
    }
}

void AudioSlider::resizeEvent(QResizeEvent *event)
{
    QSlider::resizeEvent(event);

    rebuildDbMarkersPixmap();

    recreateInterpolatedColors();

    update();
}

void AudioSlider::rebuildDbMarkersPixmap()
{
    dbMarkersPixmap = QPixmap(width(), height());
    dbMarkersPixmap.fill(Qt::transparent);
    QPainter painter(&dbMarkersPixmap);
    drawDbMarkers(painter);
}

qreal AudioSlider::getMaxDbValue() const
{
    return Utils::linearToDb(Utils::linearGainToPower(getMaxLinearValue()));
}

std::vector<float> AudioSlider::createDBValues()
{
    std::vector<float> values;

    // values from 0 dB to MIN_DB_VALUE
    qreal db = 0;
    while (db >= AudioSlider::MIN_DB_VALUE) {
        values.push_back(db);
        db -= db <= -24 ? 12 : 6;
    }

    // values > 0 dB
    if (maximum() > 100) {
        const auto maxDbValue = getMaxDbValue();

        db = 0.0;
        do {
            db += 3.0;
            values.insert(values.begin(), db);
        }
        while (db < maxDbValue);
    }

    return values;
}

qreal AudioSlider::getPeakOffset() const
{
    return getMaxLinearValue() - Utils::dbToLinear(0.0);
}

void AudioSlider::drawDbMarkers(QPainter &painter)
{
    painter.setPen(dBMarksColor);
    painter.setFont(font());
    QFontMetrics metrics = fontMetrics();
    qreal fontHeight = metrics.height();
    qreal fontAscent = metrics.descent();
    qreal center = isVertical() ? width()/2.0 : height()/2.0;

    const bool drawTicks = !isVertical();
    const static qreal tickHeight = 1.5f;
    qreal tickY = height() - 3.0 - tickHeight;

    const auto maxDbValue = getMaxDbValue();

    static const std::vector<float> dbValues = createDBValues();
    qreal lastMarkPosition = -1;
    for (float db : dbValues) {

        // skipping the max DB value and Zero DB to keep the UI more clean
        if (db >= maxDbValue || db == 0.0) {
            continue;
        }

        QString text = QString::number(static_cast<int>(db));
        if (db > 0)
            text = "+" + text;

        int textWidth = metrics.width(text);

        qreal linearPos = 1.0 - Utils::poweredGainToLinear(Utils::dbToLinear(db - maxDbValue));

        qreal y = (isVertical() ? (linearPos * height()) : center) + fontHeight/2.0 - fontAscent;
        qreal x = (isVertical() ? center : (1.0 - linearPos) * width()) - textWidth/2;

        // check if current value can be showed or need be skiped because the widget is not big enough
        qreal currentMarkPosition = isVertical() ? y : x;
        bool skip = false;
        if (lastMarkPosition >= 0) { // not the first loop iteration?
            skip = isVertical() ?
                        ((currentMarkPosition - lastMarkPosition) <= fontHeight * 1.3) :
                        ((lastMarkPosition - currentMarkPosition) <= textWidth * 1.3);
        }

        if (skip || db == AudioSlider::MIN_DB_VALUE)
            continue;

        lastMarkPosition = currentMarkPosition;

        painter.drawText(x, y, text);

        if (drawTicks) {
            qreal tickX = (1.0 - linearPos) * width() - 1.0;
            painter.drawLine(tickX, tickY, tickX, tickY + tickHeight);
        }
    }
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

    static const QColor MARKER_COLOR(255, 255, 255, 120);
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


void AudioSlider::setDbMarksColor(const QColor &newColor)
{
    this->dBMarksColor = newColor;
    update();
}

void AudioSlider::setRmsColor(const QColor &newColor){
    this->rmsColor = newColor;
    recreateInterpolatedColors();
    update();
}

void AudioSlider::setMaxPeakColor(const QColor &newColor)
{
    this->maxPeakColor = newColor;
    recreateInterpolatedColors();
    update();
}

void AudioSlider::setPeaksStartColor(const QColor &newColor)
{
    this->peakStartColor = newColor;
    recreateInterpolatedColors();
    update();
}

void AudioSlider::setPeaksEndColor(const QColor &newColor)
{
    this->peakEndColor = newColor;
    recreateInterpolatedColors();
    update();
}

void AudioSlider::setDrawSegments(bool drawSegments)
{
    this->drawSegments = drawSegments;

    update();
}

void AudioSlider::setPaintMaxPeakMarker(bool paintMaxPeak)
{
    AudioSlider::paintingMaxPeakMarker = paintMaxPeak;
}

void AudioSlider::setPaintingDbMarkers(bool paintDbMarkers)
{
    if (paintingDbMarkers != paintDbMarkers) {
        paintingDbMarkers = paintDbMarkers;
        update();
    }
}

void AudioSlider::updateStyleSheet()
{
    rebuildDbMarkersPixmap();
    update();
    //BaseMeter::updateStyleSheet();
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
        int percent = qAbs(static_cast<float>(value())/maximum() * 100.0);
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

    //QStyleOptionSlider opt;
    //initStyleOption(&opt);
    //QRect rectHandle = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, NULL);
    //line.translate(-rectHandle.width()/2.0, 0.0);

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


