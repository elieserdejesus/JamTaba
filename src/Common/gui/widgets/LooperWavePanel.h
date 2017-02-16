#ifndef _LOOPER_WAVE_PANEL_
#define _LOOPER_WAVE_PANEL_

#include "WavePeakPanel.h"
#include <QPainter>

namespace Audio {
class Looper;
}

class LooperWavePanel : public WavePeakPanel
{

public:
    LooperWavePanel(Audio::Looper *looper, quint8 layerIndex);
    ~LooperWavePanel();
    void setBeatsPerInteval(uint bpi, uint samplesPerInterval);
    void setCurrentBeat(quint8 currentIntervalBeat);
    void updateDrawings();

protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    uint beatsPerInterval;
    uint samplesPerInterval;
    uint samplesPerPixel;

    quint8 currentIntervalBeat;

    void drawBpiVerticalLines(QPainter &painter);

    uint calculateSamplesPerPixel() const;

    float lastMaxPeak;
    uint accumulatedSamples;

    Audio::Looper *looper;

    const quint8 layerID;

};

#endif
