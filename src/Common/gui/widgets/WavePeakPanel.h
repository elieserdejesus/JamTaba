#ifndef WAVE_PEAK_PANEL_H
#define WAVE_PEAK_PANEL_H

#include <QWidget>

class WavePeakPanel : public QWidget
{
    Q_OBJECT

    // custom properties defined in stylesheet files
    Q_PROPERTY(QColor peaksColor MEMBER peaksColor WRITE setPeaksColor)
    Q_PROPERTY(QColor loadingColor MEMBER loadingColor)

public:
    explicit WavePeakPanel(QWidget *parent = 0);
    virtual ~WavePeakPanel();

    enum WaveDrawingMode
    {
        SOUND_WAVE,
        BUILDINGS,
        PIXELED_SOUND_WAVE,
        PIXELED_BUILDINGS
    };

    void addPeak(float peak);
    void clearPeaks();

    QSize minimumSizeHint() const;

    void setBufferingPercentage(uint percentage);
    void setShowBuffering(bool setShowBuffering);
    void setDrawingMode(WavePeakPanel::WaveDrawingMode mode);

    virtual void setPeaksColor(const QColor &color);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

    virtual int getPeaksPad() const;
    virtual int getPeaksWidth() const;

    bool useAlphaInPreviousSamples;

    std::vector<float> peaksArray;

    QColor peaksColor;

private:
    QColor loadingColor; // color for the loading circle

    bool showingBuffering;
    int bufferingPercentage;

    uint maxPeaks; // change when widget resize

    int computeMaxPeaks();
    void recreatePeaksArray();

    void paintBuildings(QPainter &painter, bool pixeled, bool useAlpha);
    void paintSoundWave(QPainter &painter, bool useAlpha);
    void paintPixeledSoundWave(QPainter &painter);

    WaveDrawingMode drawingMode;

};

#endif
