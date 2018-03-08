#include "Slider.h"
#include "Utils.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QToolTip>
#include <QDateTime>

const quint8 AudioSlider::SEGMENTS_SIZE = 6;

const float AudioSlider::MAX_DB_VALUE = 0.0f;
const float AudioSlider::MIN_DB_VALUE = -60.0f;
const float AudioSlider::MAX_LINEAR_VALUE = Utils::dbToLinear(AudioSlider::MAX_DB_VALUE);
const float AudioSlider::MIN_LINEAR_VALUE = Utils::dbToLinear(AudioSlider::MIN_DB_VALUE);
const float AudioSlider::MIN_SMOOTHED_LINEAR_VALUE = AudioSlider::getSmoothedLinearPeakValue(AudioSlider::MIN_LINEAR_VALUE);
const float AudioSlider::MAX_SMOOTHED_LINEAR_VALUE = AudioSlider::getSmoothedLinearPeakValue(AudioSlider::MAX_LINEAR_VALUE);

const int AudioSlider::MAX_PEAK_MARKER_SIZE = 2;
const int AudioSlider::MAX_PEAK_SHOW_TIME = 1500;
const int AudioSlider::MIN_SIZE = 12;

const float AudioSlider::RESIZE_FACTOR = 1.0f/AudioSlider::MIN_SMOOTHED_LINEAR_VALUE;

bool AudioSlider::paintingMaxPeakMarker = true;
bool AudioSlider::paintingPeaks = true;
bool AudioSlider::paintingRMS = true;

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

        const static qreal peakValuesOffset = MAX_SMOOTHED_LINEAR_VALUE - 1.0f; // peaks are not starting at 0dB, so we need a offset

        if (isEnabled()) {

            const uint channels = stereo ? 2 : 1;
            const qreal rectSize = isVertical() ? height() : width();
            QRectF drawRect(rect().adjusted(1, 1, 0, 0));

            const int parallelSegments = getParallelSegments();

            if (isVertical())
                drawRect.setWidth(drawRect.width()/static_cast<qreal>(parallelSegments));
            else
                drawRect.setHeight(drawRect.height()/static_cast<qreal>(parallelSegments));

            for (uint i = 0; i < channels; ++i) {
                if (paintingPeaks && currentPeak[i]) {
                    qreal peakPosition = getPeakPosition(currentPeak[i], rectSize, peakValuesOffset);
                    paintSegments(painter, drawRect, peakPosition, peakColors, drawSegments);
                }

                if (paintingMaxPeakMarker && maxPeak[i]) {
                    qreal maxPeakPosition = getPeakPosition(maxPeak[i], rectSize, peakValuesOffset);
                    paintMaxPeakMarker(painter, maxPeakPosition, drawRect);
                }

                if (paintingRMS && currentRms[i]) {
                    qreal rmsPosition = getPeakPosition(currentRms[i], rectSize, peakValuesOffset);

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

        updateInternalValues(); // compute decay and max peak
    }

    QSlider::paintEvent(ev);
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

qreal AudioSlider::getPeakPosition(qreal linearPeak, qreal rectSize, qreal peakValueOffset)
{
    const qreal smoothedPeakValue = getSmoothedLinearPeakValue(linearPeak) - peakValueOffset;
    const qreal peakPosition = (1 - ((MAX_SMOOTHED_LINEAR_VALUE - smoothedPeakValue) * RESIZE_FACTOR)) * rectSize;

    return peakPosition;
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

    update();
}

void AudioSlider::rebuildDbMarkersPixmap()
{
    dbMarkersPixmap = QPixmap(width(), height());
    dbMarkersPixmap.fill(Qt::transparent);
    QPainter painter(&dbMarkersPixmap);
    drawDbMarkers(painter);
}

std::vector<float> AudioSlider::createDBValues()
{
    std::vector<float> values;
    qreal db = AudioSlider::MAX_DB_VALUE;
    while (db >= AudioSlider::MIN_DB_VALUE) {
        values.push_back(db);
        db -= db <= -24 ? 12 : 6;
    }
    return values;
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
    const static qreal tickHeight = 2.5f;
    qreal tickY = height() - 1.0 - tickHeight;

    static const std::vector<float> dbValues = createDBValues();
    qreal lastMarkPosition = -1;
    for (float db : dbValues) {

        // skipping the first DB value (Zero DB) to keep the UI more clean
        if (db == AudioSlider::MAX_DB_VALUE) {
            continue;
        }

        QString text = QString::number(qAbs(db));
        int textWidth = metrics.width(text);

        qreal linearValue = getSmoothedLinearPeakValue(Utils::dbToLinear(db));

        qreal y = (isVertical() ? (MAX_SMOOTHED_LINEAR_VALUE - linearValue) * height() * RESIZE_FACTOR : center) + fontHeight/2.0 - fontAscent;
        qreal x = (isVertical() ? center : (1 - ((MAX_SMOOTHED_LINEAR_VALUE - linearValue) * RESIZE_FACTOR)) * width()) - textWidth/2;

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
            float tickX = (1 - ((MAX_SMOOTHED_LINEAR_VALUE - linearValue) * RESIZE_FACTOR)) * width() - 1;
            painter.drawLine(tickX, tickY, tickX, tickY + tickHeight);
        }
    }
}

qreal AudioSlider::getSmoothedLinearPeakValue(qreal linearValue)
{
    const static qreal smoothFactor = 1.0/10.0f; // used to get more equally spaced markers

    return std::pow(linearValue, smoothFactor);
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
