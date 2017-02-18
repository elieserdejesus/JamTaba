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
    if (looper->isPlaying() || looper->isRecording()) {
        const bool drawingCurrentLayer = looper->getCurrentLayerIndex() == layerID;
        return drawingCurrentLayer || looper->getPlayMode() == Audio::Looper::ALL_LAYERS;
    }

    return false;
}

void LooperWavePanel::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
    looper->selectLayer(this->layerID);
    update();
}

void LooperWavePanel::paintEvent(QPaintEvent *ev)
{

    if (!beatsPerInterval || looper->isWaiting()) {
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
            qreal width = (currentIntervalBeat * pixelsPerBeat) + pixelsPerBeat;
            painter.fillRect(QRectF(0, 0, width, height()), redColor);
        }
    }

    if (looper->isPlaying() && drawingLayerNumber) {

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
