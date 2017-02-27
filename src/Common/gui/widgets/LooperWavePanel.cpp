#include "LooperWavePanel.h"
#include "audio/looper/AudioLooper.h"

#include <QKeyEvent>

LooperWavePanel::LooperWavePanel(Audio::Looper *looper, quint8 layerIndex)
    : beatsPerInterval(16),
      lastMaxPeak(0),
      accumulatedSamples(0),
      samplesPerPixel(0),
      samplesPerInterval(0),
      looper(looper),
      layerID(layerIndex)
{
   setDrawingMode(WavePeakPanel::SOUND_WAVE);
   this->useAlphaInPreviousSamples = false; // all samples are painted without alpha

   const qreal rightMargin = 3.0;
   const bool lockOpened = true;

   qreal miniLockIconHeight = getMiniLockIconHeight(lockOpened);
   miniLockIcon = createLockIcon(lockOpened, miniLockIconHeight, 2.0, rightMargin);

   const qreal topMargin = 6.0;
   bigLockIcon = createLockIcon(false, height() - topMargin * 2, topMargin, rightMargin);

   const qreal discardIconTopMargin = height() - topMargin - miniLockIconHeight;

   const qreal discardIconHeight = miniLockIconHeight * 0.6;
   discardIcon = createDiscardIcon(discardIconTopMargin, rightMargin, discardIconHeight);

   connect(looper, &Audio::Looper::layerLockedStateChanged, this, &LooperWavePanel::updateMiniLockIconPainterPath);

   setMouseTracking(true);
}

LooperWavePanel::~LooperWavePanel()
{

}

qreal LooperWavePanel::getMiniLockIconHeight(bool lockOpened)
{
    return lockOpened ? 16 : 20;
}

void LooperWavePanel::updateDrawings()
{
    if (!looper)
        return;

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

    updateMiniLockIconPainterPath();

    const qreal topMargin = 6.0;
    const qreal rightMargin = 3.0;
    bigLockIcon = createLockIcon(false, height() - topMargin * 2, topMargin, rightMargin);

    qreal miniLockIconHeight = getMiniLockIconHeight(true);
    const qreal discardIconRightMargin = 3.0;
    const qreal discardIconHeight = miniLockIconHeight * 0.6;
    const qreal discardIconTopMargin = height() - topMargin - discardIconHeight;
    discardIcon = createDiscardIcon(discardIconTopMargin, discardIconRightMargin, discardIconHeight);
}

void LooperWavePanel::updateMiniLockIconPainterPath()
{
    bool lockIconIsOpened = looper && !(looper->layerIsLocked(layerID));
    const qreal topMargin = 2;
    const qreal rightMargin = 3;
    miniLockIcon = createLockIcon(lockIconIsOpened, getMiniLockIconHeight(lockIconIsOpened), topMargin, rightMargin);
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
        return drawingCurrentLayer || looper->getMode() == Audio::Looper::ALL_LAYERS;
    }
    else if (looper->isStopped()) {
        return drawingCurrentLayer;
    }

    return false;
}

void LooperWavePanel::mousePressEvent(QMouseEvent *ev)
{
    bool lockIconClicked = looper->canLockLayer(layerID) && miniLockIcon.boundingRect().contains(ev->pos());
    bool discardIconClicked = looper->canClearLayer(layerID) && discardIcon.boundingRect().contains(ev->pos());
    if (lockIconClicked) // user is clicking in lock icon?
        looper->toggleLayerLockedState(layerID);
    else if(discardIconClicked)
        looper->clearLayer(layerID);
    else
        looper->selectLayer(layerID);

    update();
}

void LooperWavePanel::mouseMoveEvent(QMouseEvent *ev)
{
    lastMousePos = ev->pos();
}

void LooperWavePanel::paintEvent(QPaintEvent *ev)
{

    if (!beatsPerInterval || (looper->isWaiting() && looper->getCurrentLayerIndex() == layerID)) {
        return;
    }

    const bool useHighlightPainting = canUseHighlightPainting();

    static const QColor redColor(255, 0, 0, 30);
    static const QColor transparentColor(0, 0, 0, 80);

    QColor previousPeakColor(peaksColor);
    if (!useHighlightPainting) // not-current layers are painted with a transparent black color
        peaksColor = QColor(0, 0, 0, 40);

    WavePeakPanel::paintEvent(ev); // parent class painting

    peaksColor = previousPeakColor;

    QPainter painter(this);

    qreal pixelsPerBeat = (width()/static_cast<qreal>(beatsPerInterval));

    if (useHighlightPainting) {
        drawBpiVerticalLines(painter, pixelsPerBeat);

        const bool isCurrentLayer = looper->getCurrentLayerIndex() == layerID;

        if (looper->isPlaying())
            drawCurrentBeatRect(painter, pixelsPerBeat);
        else if (looper->isRecording() && isCurrentLayer)
            drawRecordingRedRect(painter, redColor, pixelsPerBeat); // draw a transparent red rect from left to current interval beat

        if (isCurrentLayer)
            drawBorder(painter, redColor);
    }

    const bool layerIsValid = looper->layerIsValid(layerID);
    const bool canUseMiniLockIcon = !miniLockIcon.isEmpty() && !looper->isRecording() && layerIsValid;
    const bool canDrawBigLockIcon = looper->layerIsLocked(layerID);
    const bool canDrawLayerNumber = looper->canSelectLayers();
    const bool canUseDiscardIcon = looper->canClearLayer(layerID);

    if (canUseMiniLockIcon)
        drawMiniLockIcon(painter, (miniLockIcon.boundingRect().contains(lastMousePos)) ? peaksColor : transparentColor);

    if (canUseDiscardIcon)
        drawDiscardIcon(painter, (discardIcon.boundingRect().contains(lastMousePos)) ? peaksColor : transparentColor);

    if (canDrawBigLockIcon)
        drawBigLockIcon(painter, transparentColor);

    if (canDrawLayerNumber)
        drawLayerNumber(painter);
}

void LooperWavePanel::drawDiscardIcon(QPainter &painter, const QColor &color)
{
    QPen pen(QBrush(color), 2.0, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(pen);
    painter.drawPath(discardIcon);
}

void LooperWavePanel::drawBigLockIcon(QPainter &painter, const QColor &transparentColor)
{
    qreal xOffset = width()/2.0 - bigLockIcon.boundingRect().width()/2.0;
    painter.translate(-xOffset, 0.0);
    painter.fillPath(bigLockIcon, transparentColor);
    painter.setTransform(QTransform());
}

void LooperWavePanel::drawMiniLockIcon(QPainter &painter, const QColor &transparentColor)
{
    painter.setRenderHint(QPainter::Antialiasing);

    bool layerIsLocked = looper->layerIsLocked(layerID);
    QColor fillColor = layerIsLocked ? peaksColor : transparentColor;
    painter.fillPath(miniLockIcon, fillColor);
}

void LooperWavePanel::drawRecordingRedRect(QPainter &painter, QColor redColor, const qreal pixelsPerBeat)
{
    qreal redRectWidth = (currentIntervalBeat * pixelsPerBeat) + pixelsPerBeat;
    painter.fillRect(QRectF(0, 0, redRectWidth, height()), redColor);
}

void LooperWavePanel::drawBorder(QPainter &painter, const QColor &color)
{
    painter.setPen(color);
    painter.drawRect(QRectF(0, 0, width() -1, height() -1));
}

void LooperWavePanel::drawCurrentBeatRect(QPainter &painter, qreal pixelsPerBeat)
{
    qreal x = currentIntervalBeat * pixelsPerBeat;
    QColor color(peaksColor);
    color.setAlpha(30);
    painter.fillRect(QRectF(x, 0, pixelsPerBeat, height()), color);
}

void LooperWavePanel::drawBpiVerticalLines(QPainter &painter, qreal pixelsPerBeat)
{
    static const QPen dotPen(QColor(0, 0, 0, 60), 1.0, Qt::DotLine);
    for (uint beat = 0; beat < beatsPerInterval; ++beat) {
        const qreal x = beat * pixelsPerBeat;
        painter.setPen(dotPen);
        painter.drawLine(QPointF(x, 0), QPointF(x, height()));
    }
}

void LooperWavePanel::drawLayerNumber(QPainter &painter)
{
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

QPainterPath LooperWavePanel::createDiscardIcon(qreal topMargin, qreal rightMargin, qreal iconSize) const
{
    QPainterPath path;
    qreal right = width() - 1 - rightMargin;
    path.moveTo(QPointF(right - iconSize, topMargin));
    path.lineTo(QPointF(right, topMargin + iconSize));

    path.moveTo(QPointF(right, topMargin));
    path.lineTo(QPointF(right - iconSize, topMargin + iconSize));

    return path;
}

QPainterPath LooperWavePanel::createLockIcon(bool lockIsOpened, qreal lockHeight, qreal topMargin, qreal rightMargin) const
{
    const qreal vOffset = topMargin; // top margim

    const qreal baseRectHeight = lockHeight * 0.55; // lock base rect
    const qreal baseRectTop = lockHeight - baseRectHeight + vOffset;
    const qreal baseRectWidth = lockHeight * 0.7;
    const qreal margin = baseRectWidth * 0.1;
    const qreal baseRectLeft = width() - 1 - baseRectWidth - rightMargin;
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
