#include "LooperWavePanel.h"
#include "audio/AudioLooper.h"

LooperWavePanel * LooperWavePanel::currentWavePanel = nullptr;

LooperWavePanel::LooperWavePanel(uint bpi, uint samplesPerInterval, Audio::Looper *looper)
    : beatsPerInterval(16),
      lastMaxPeak(0),
      accumulatedSamples(0),
      samplesPerPixel(0),
      samplesPerInterval(0),
      looper(looper)

{
   setBeatsPerInteval(bpi, samplesPerInterval);

   setDrawingMode(WavePeakPanel::SOUND_WAVE);

   this->useAlphaInPreviousSamples = false; // all samples are painted without alpha
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

void LooperWavePanel::setCurrentIntervalBeat(quint8 currentIntervalBeat)
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

        // draw a transparent red rect from left to current interval beat
        if (currentWavePanel == this) {
            if (looper->isRecording()) {
                static const QColor redColor(255, 0, 0, 15);
                uint width = (currentIntervalBeat * pixelsPerBeat) + pixelsPerBeat;
                painter.fillRect(0, 0, width, height(), redColor);
            }
            else if (looper->isWaiting()) {
                QString text = tr("wait ...");
                uint textWidth = fontMetrics().width(text);
                painter.drawText(width()/2 - textWidth/2, height()/2, text);
            }
        }
    }
}
