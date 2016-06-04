#ifndef PEAK_METER_H
#define PEAK_METER_H

#include <QFrame>

class BaseMeter : public QFrame
{
    Q_OBJECT

public:
    explicit BaseMeter(QWidget *parent = 0);
    virtual ~BaseMeter();
    void setDecayTime(quint32 decayTimeInMiliseconds);
    virtual void setOrientation(Qt::Orientation orientation);
    QSize minimumSizeHint() const override;

protected:
    inline bool isVertical() const { return orientation == Qt::Vertical; }

    QRectF getPaintRect(float peakValue) const;

    static float limitFloatValue(float value, float minValue = 0.0f, float maxValue = 1.0f);

    qint64 lastUpdate;

    int decayTime;

    Qt::Orientation orientation;

    static const int LINES_MARGIN;
    static const int MIN_SIZE;
    static const int DEFAULT_DECAY_TIME;
};
//========================================================================
class AudioMeter : public BaseMeter
{
public:
    AudioMeter(QWidget *parent);

    void setPeak(float, float rms);

    static void setPaintMaxPeakMarker(bool paintMaxPeak);
    static void paintRmsOnly();
    static void paintPeaksOnly();
    static void paintPeaksAndRms();
    static bool isPaintingPeaksOnly();
    static bool isPaintingRmsOnly();

    inline static bool isPaintintMaxPeakMarker() { return paintingMaxPeakMarker; }
    inline static bool isPaintingRMS() { return paintingRMS; }
    inline static bool isPaintingPeaks() { return paintingPeaks; }

    void setOrientation(Qt::Orientation orientation) override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *) override;

private:
    static const int MAX_PEAK_SHOW_TIME;
    static const QColor MAX_PEAK_COLOR;
    static const int MAX_PEAK_MARKER_SIZE;

    QLinearGradient gradient;

    QLinearGradient createGradient();

    static const QColor GRADIENT_FIRST_COLOR;
    static const QColor GRADIENT_MIDDLE_COLOR;
    static const QColor GRADIENT_LAST_COLOR;

    static const QColor RMS_COLOR;

    //static painting flags. Turning on/off will affect all audio meters.
    static bool paintingMaxPeakMarker;
    static bool paintingPeaks;
    static bool paintingRMS;

    float currentPeak;
    float maxPeak;
    float currentRms;

    qint64 lastMaxPeakTime;

};

class MidiActivityMeter : public BaseMeter
{
public:
    MidiActivityMeter(QWidget *parent);
    void setSolidColor(const QColor &color);
    void setActivityValue(float value);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QColor solidColor;
    float activityValue;
};

#endif
