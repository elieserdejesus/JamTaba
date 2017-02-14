#ifndef _LOOPER_WAVE_PANEL_
#define _LOOPER_WAVE_PANEL_

#include "WavePeakPanel.h"
#include <QPainter>

class LooperWavePanel : public WavePeakPanel
{

public:
    LooperWavePanel(uint bpi, uint samplesPerInterval);
    void setBeatsPerInteval(uint bpi, uint samplesPerInterval);

    void addPeak(float peak, uint samples) override;

protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    uint beatsPerInterval;
    uint samplesPerInterval;
    uint samplesPerPixel;

    void drawBpiVerticalLines(QPainter &painter);

    float lastMaxPeak;
    uint accumulatedSamples;
};

#endif
