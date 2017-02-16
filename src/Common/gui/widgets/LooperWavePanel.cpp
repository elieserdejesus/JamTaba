#include "LooperWavePanel.h"

LooperWavePanel * LooperWavePanel::currentWavePanel = nullptr;

LooperWavePanel::LooperWavePanel(uint bpi, uint samplesPerInterval)
    : beatsPerInterval(16),
      lastMaxPeak(0),
      accumulatedSamples(0),
      samplesPerPixel(0),
      samplesPerInterval(0)

{
   setDrawingMode(WavePeakPanel::PIXELED_SOUND_WAVE);

   setBeatsPerInteval(bpi, samplesPerInterval);
}

LooperWavePanel::~LooperWavePanel()
{
    if (LooperWavePanel::currentWavePanel == this) {
        LooperWavePanel::currentWavePanel = nullptr;
    }
}

void LooperWavePanel::addPeak(float peak, uint samples)
{
    if (LooperWavePanel::currentWavePanel != this) {
        if (LooperWavePanel::currentWavePanel)
            LooperWavePanel::currentWavePanel->update(); // repaint and erase the last rect

        LooperWavePanel::currentWavePanel = this;
    }

    if (peak > lastMaxPeak) {
        lastMaxPeak = peak;
    }

    accumulatedSamples += samples;
    if (samplesPerPixel) { // avoid division by zero
        if (accumulatedSamples >= samplesPerPixel) {
            WavePeakPanel::addPeak(lastMaxPeak);

            accumulatedSamples = accumulatedSamples % samplesPerPixel;
            lastMaxPeak = 0;
        }
    }
}

void LooperWavePanel::setCurrentIntervalBet(quint8 currentIntervalBeat)
{
    this->currentIntervalBeat = currentIntervalBeat;
}

void LooperWavePanel::resizeEvent(QResizeEvent *event)
{
    WavePeakPanel::resizeEvent(event);

    samplesPerPixel = calculateSamplePerPixel();
}

uint LooperWavePanel::calculateSamplePerPixel() const
{
    uint pixelWidth = getPeaksWidth() + getPeaksPad();
    return samplesPerInterval/(width()/pixelWidth);
}

void LooperWavePanel::setBeatsPerInteval(uint bpi, uint samplesPerInterval)
{
    this->beatsPerInterval = bpi;
    this->samplesPerInterval = samplesPerInterval;
    this->samplesPerPixel = calculateSamplePerPixel();
    update();
}

void LooperWavePanel::paintEvent(QPaintEvent *ev)
{
    WavePeakPanel::paintEvent(ev);

    if (beatsPerInterval) {
        QPainter painter(this);

        static const QPen pen(QColor(0, 0, 0, 60), 1.0, Qt::DotLine);
        painter.setPen(pen);

        int pixelsPerBeat = width()/beatsPerInterval;

        const int top = 0;
        const int bottom = height();
        for (uint beat = 0; beat < beatsPerInterval; ++beat) {
            const int x = beat * pixelsPerBeat;
            painter.drawLine(x, top, x, bottom);
        }

        // draw a transparent red rect in current interval beat
        if (currentWavePanel == this) {
            static const QColor rectColor(255, 0, 0, 30);
            uint x = currentIntervalBeat * pixelsPerBeat;
            painter.fillRect(x, 0, pixelsPerBeat, height(), rectColor);
        }
    }
}
