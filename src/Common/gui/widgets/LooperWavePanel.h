#ifndef _LOOPER_WAVE_PANEL_
#define _LOOPER_WAVE_PANEL_

#include "WavePeakPanel.h"
#include <QPainter>
#include <QPainterPath>

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
    void updateDrawings(bool drawingLayerNumber);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *ev) override;

private slots:
    void updateMiniLockIconPainterPath();

private:
    uint beatsPerInterval;
    uint samplesPerInterval;
    uint samplesPerPixel;

    bool drawingLayerNumber;

    quint8 currentIntervalBeat;

    void drawBpiVerticalLines(QPainter &painter);

    uint calculateSamplesPerPixel() const;

    bool canUseHighlightPainting() const;

    float lastMaxPeak;
    uint accumulatedSamples;

    Audio::Looper *looper;

    QPainterPath miniLockIcon;
    QPainterPath bigClosedLockIcon;
    QPainterPath createLockIconPainterPath(bool lockIsOpened, qreal lockHeight, qreal topMargin);
    static qreal getMiniLockIconHeight(bool lockOpened);

    const quint8 layerID;
};

#endif
