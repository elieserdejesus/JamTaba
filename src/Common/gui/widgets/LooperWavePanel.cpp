#include "LooperWavePanel.h"
#include "audio/AudioLooper.h"

#include <QKeyEvent>

LooperWavePanel::LooperWavePanel(Audio::Looper *looper, quint8 layerIndex)
    : beatsPerInterval(16),
      lastMaxPeak(0),
      accumulatedSamples(0),
      samplesPerPixel(0),
      samplesPerInterval(0),
      looper(looper),
      layerID(layerIndex),
      drawingLayerNumber(false)
{
   setDrawingMode(WavePeakPanel::SOUND_WAVE);
   this->useAlphaInPreviousSamples = false; // all samples are painted without alpha

   lockIconPainterPath = createLockIconPainterPath(true);

   connect(looper, &Audio::Looper::layerLockedStateChanged, this, &LooperWavePanel::updateLockIconPainterPath);
}

LooperWavePanel::~LooperWavePanel()
{

}

void LooperWavePanel::updateDrawings(bool drawLayerNumber)
{
    if (!looper)
        return;

    this->drawingLayerNumber = drawLayerNumber;

    peaksArray = looper->getLayerPeaks(layerID, samplesPerPixel);

    update();
}

void LooperWavePanel::setCurrentBeat(quint8 currentIntervalBeat)
{
    this->currentIntervalBeat = currentIntervalBeat;
}

void LooperWavePanel::resizeEvent(QResizeEvent *event)
{
    WavePeakPanel::resizeEvent(event);

    samplesPerPixel = calculateSamplesPerPixel();

    updateLockIconPainterPath();
}

void LooperWavePanel::updateLockIconPainterPath()
{
    bool lockIconIsClosed = looper && looper->layerIsLocked(layerID);
    lockIconPainterPath = createLockIconPainterPath(!lockIconIsClosed);
}

uint LooperWavePanel::calculateSamplesPerPixel() const
{
    uint pixelWidth = getPeaksWidth() + getPeaksPad();
    return samplesPerInterval/(width()/pixelWidth);
}

void LooperWavePanel::setBeatsPerInteval(uint bpi, uint samplesPerInterval)
{
    this->beatsPerInterval = bpi;
    this->samplesPerInterval = samplesPerInterval;
    this->samplesPerPixel = calculateSamplesPerPixel();
    update();
}

bool LooperWavePanel::canUseHighlightPainting() const
{
    const bool drawingCurrentLayer = looper->getCurrentLayerIndex() == layerID;

    if (looper->isPlaying() || looper->isRecording()) {
        return drawingCurrentLayer || looper->getPlayMode() == Audio::Looper::ALL_LAYERS;
    }
    else if (looper->isStopped()) {
        return drawingCurrentLayer;
    }

    return false;
}

void LooperWavePanel::mousePressEvent(QMouseEvent *ev)
{
    if (lockIconPainterPath.boundingRect().contains(ev->pos())) // use is clicking in lock icon?
        looper->toggleLayerLockedState(layerID);
    else
        looper->selectLayer(this->layerID);

    update();
}

void LooperWavePanel::paintEvent(QPaintEvent *ev)
{

    if (!beatsPerInterval || (looper->isWaiting() && looper->getCurrentLayerIndex() == layerID)) {
        return;
    }

    const bool useHighlightPainting = canUseHighlightPainting();

    QColor previousPeakColor(peaksColor);

    if (!useHighlightPainting) // not-current layers are painted with a transparent black color
        peaksColor = QColor(0, 0, 0, 40);

    WavePeakPanel::paintEvent(ev);

    peaksColor = previousPeakColor;

    QPainter painter(this);

    qreal pixelsPerBeat = (width()/static_cast<qreal>(beatsPerInterval));

    if (useHighlightPainting) {
        static const QPen dotPen(QColor(0, 0, 0, 60), 1.0, Qt::DotLine);
        for (uint beat = 0; beat < beatsPerInterval; ++beat) {
            const qreal x = beat * pixelsPerBeat;
            painter.setPen(dotPen);
            painter.drawLine(QPointF(x, 0), QPointF(x, height()));
        }

        if (looper->isPlaying()) {
            qreal x = currentIntervalBeat * pixelsPerBeat;
            QColor color(peaksColor);
            color.setAlpha(30);
            painter.fillRect(QRectF(x, 0, pixelsPerBeat, height()), color);
        }
        else if (looper->isRecording()) {     // draw a transparent red rect from left to current interval beat
            static const QColor redColor(255, 0, 0, 30);
            qreal redRectWidth = (currentIntervalBeat * pixelsPerBeat) + pixelsPerBeat;
            painter.fillRect(QRectF(0, 0, redRectWidth, height()), redColor);

            painter.setPen(redColor);
            painter.drawRect(QRectF(0, 0, width() -1, height() -1));
        }
    }

    bool layerIsValid = looper->layerIsValid(layerID);
    bool canDrawLockIcon = !lockIconPainterPath.isEmpty() && layerIsValid;
    if (canDrawLockIcon) {
        painter.setRenderHint(QPainter::Antialiasing);
        static const QColor transparentColor(0, 0, 0, 80);

        bool layerIsLocked = looper->layerIsLocked(layerID);
        QColor fillColor = layerIsLocked ? peaksColor : transparentColor;
        painter.fillPath(lockIconPainterPath, fillColor);
    }

    bool canDrawLayerNumber = drawingLayerNumber && (looper->isPlaying() || looper->isStopped());
    if (canDrawLayerNumber) {

        painter.setRenderHint(QPainter::Antialiasing);

        static QColor color(0, 0, 0, 20);
        painter.setBrush(color);

        QString text = QString::number(layerID + 1);
        qreal textWidth = fontMetrics().width(text);
        qreal textHeight = fontMetrics().height();

        QRectF textRect(3, 3, textWidth * 2, textHeight);
        painter.setPen(Qt::NoPen);
        painter.drawRect(textRect);

        textRect.translate(textWidth/2.0, 0);


        painter.setPen(peaksColor);
        painter.setCompositionMode(QPainter::CompositionMode_Difference);
        painter.drawText(textRect, text);
    }
}

QPainterPath LooperWavePanel::createLockIconPainterPath(bool lockIsOpened)
{
    const qreal lockHeight = lockIsOpened ? 16 : 20;

    const qreal vOffset = 2; // top margim

    const qreal baseRectHeight = lockHeight * 0.55; // lock base rect
    const qreal baseRectTop = lockHeight - baseRectHeight + vOffset;
    const qreal baseRectWidth = lockHeight * 0.7;
    const qreal margin = baseRectWidth * 0.1;
    const qreal baseRectLeft = width()-1 - baseRectWidth;// - margin;
    QRectF baseRect(baseRectLeft, baseRectTop, baseRectWidth, baseRectHeight);

    const qreal arcLeft = baseRectLeft + margin;
    const qreal arcRight = baseRectLeft + baseRectWidth - margin;
    const qreal arcHeight = baseRectTop - vOffset;
    const qreal arcWidth = arcRight - arcLeft;
    const qreal arcTop = vOffset;

    QPainterPath finalPath;
    finalPath.addRect(baseRect);

    const int startAngle = 0;
    const int spanAngle = 180;
    QPainterPath arcPath(QPointF(arcRight, baseRectTop - (lockIsOpened ? (arcHeight * 0.5) : 0.0)));
    arcPath.lineTo(arcRight, vOffset + arcHeight * 0.5); // outer right vertical line
    arcPath.arcTo(QRectF(arcLeft, arcTop, arcWidth, arcHeight), startAngle, spanAngle);
    arcPath.lineTo(arcLeft, baseRectTop); // outer left vertical line

    arcPath.lineTo(arcLeft + margin, baseRectTop); //small bottom horizontal line connect outer and inner arc

    arcPath.lineTo(arcLeft + margin, vOffset + arcHeight * 0.5); // inner left vertical line
    arcPath.arcTo(QRectF(arcLeft + margin, arcTop + margin, arcWidth - margin*2, arcHeight - margin*2), 180, -spanAngle);
    if (!lockIsOpened)
        arcPath.lineTo(arcRight - margin, baseRectTop); // right vertical line
    arcPath.closeSubpath();

    finalPath.addPath(arcPath);

    QPainterPath keyHoleCirclePath;
    const qreal keyHoleSize = baseRectWidth/4.0;
    const qreal keyHoleCircleLeft = baseRectLeft + baseRectWidth/2.0 - keyHoleSize/2.0;
    const qreal keyHoleCircleTop = baseRectTop + baseRectHeight/6.0;
    keyHoleCirclePath.addEllipse(QRectF(keyHoleCircleLeft, keyHoleCircleTop, keyHoleSize, keyHoleSize));

    QPainterPath keyHoleRectPath;
    const qreal keyHoleRectWidth = keyHoleSize * 0.7;
    QRectF keyHoleRect(baseRect.center().x() - keyHoleRectWidth/2.0, keyHoleCircleTop + keyHoleSize, keyHoleRectWidth, keyHoleSize * 0.7);
    keyHoleRectPath.addRect(keyHoleRect);

    return finalPath.subtracted(keyHoleCirclePath.united(keyHoleRectPath));
}
