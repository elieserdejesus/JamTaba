#ifndef WAVE_PEAK_PANEL_H
#define WAVE_PEAK_PANEL_H

#include <QWidget>

class WavePeakPanel : public QWidget
{
    Q_OBJECT

    //custom properties defined in stylesheet files
    Q_PROPERTY(QColor peaksColor MEMBER peaksColor)
    Q_PROPERTY(QColor loadingColor MEMBER loadingColor)

public:
    explicit WavePeakPanel(QWidget *parent = 0);

    enum WavePeakPanelMode
    {
        SOUND_WAVE, BUILDINGS, PIXELED_SOUND_WAVE, PIXELED_BUILDINGS
    };

    void addPeak(float peak);
    void clearPeaks();

    QSize minimumSizeHint() const;

    void setBufferingPercentage(uint percentage);
    void setShowBuffering(bool setShowBuffering);
    void setDrawingMode(WavePeakPanel::WavePeakPanelMode mode);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QColor peaksColor;
    QColor loadingColor; //color for the loading circle

    bool showingBuffering;
    int bufferingPercentage;

    std::vector<float> peaksArray;

    uint maxPeaks;// change when widget resize

    int computeMaxPeaks();
    void recreatePeaksArray();

    void paintBuildings(QPainter &painter, bool pixeled);
    void paintSoundWave(QPainter &painter);
    void paintPixeledSoundWave(QPainter &painter);

    WavePeakPanelMode drawingMode;

    int getPeaksPad() const;
    int getPeaksWidth() const;
};

#endif
