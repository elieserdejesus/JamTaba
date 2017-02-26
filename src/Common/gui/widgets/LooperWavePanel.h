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

    uint calculateSamplesPerPixel() const;

    bool canUseHighlightPainting() const;

    float lastMaxPeak;
    uint accumulatedSamples;

    Audio::Looper *looper;

    QPainterPath miniLockIcon;  // used to draw mini lock icon in top right corner
    QPainterPath bigLockIcon;   // used to draw big lock icon in center when layer is locked
    QPainterPath discardIcon;   // used to draw the 'X' icon in bottom right corner

    QPainterPath createLockIcon(bool lockIsOpened, qreal lockHeight, qreal topMargin) const;
    static qreal getMiniLockIconHeight(bool lockOpened);
    QPainterPath createDiscardIcon(qreal topMargin, qreal iconSize) const;

    void drawLayerNumber(QPainter &painter);
    void drawBpiVerticalLines(QPainter &painter, qreal pixelsPerBeat);
    void drawCurrentBeatRect(QPainter &painter, qreal pixelsPerBeat);
    void drawBorder(QPainter &painter, const QColor &color);
    void drawRecordingRedRect(QPainter &painter, QColor redColor, const qreal pixelsPerBeat);
    void drawMiniLockIcon(QPainter &painter, const QColor &transparentColor);
    void drawBigLockIcon(QPainter &painter, const QColor &transparentColor);

    const quint8 layerID;
};

#endif
