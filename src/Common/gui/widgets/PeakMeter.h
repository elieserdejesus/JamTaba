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
    Q_OBJECT


    // custom properties defined in CSS files
    Q_PROPERTY(QColor rmsColor MEMBER rmsColor WRITE setRmsColor)
    Q_PROPERTY(QColor peakStartColor MEMBER peakStartColor WRITE setPeaksStartColor)
    Q_PROPERTY(QColor peakEndColor MEMBER peakEndColor WRITE setPeaksEndColor)

public:
    AudioMeter(QWidget *parent);

    void setPeak(float, float rms);

    // these functions will affect all meters
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

    void setRmsColor(const QColor &newColor);
    void setPeaksStartColor(const QColor &newColor);
    void setPeaksEndColor(const QColor &newColor);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *) override;

private:
    static const int MAX_PEAK_SHOW_TIME;
    static const QColor MAX_PEAK_COLOR;
    static const int MAX_PEAK_MARKER_SIZE;

    QColor rmsColor;
    QColor peakStartColor;  // start gradient color
    QColor peakEndColor;    // end gradient color

    std::vector<QColor> peakColors;
    std::vector<QColor> rmsColors;

    //static painting flags. Turning on/off will affect all audio meters.
    static bool paintingMaxPeakMarker;
    static bool paintingPeaks;
    static bool paintingRMS;

    float currentPeak;
    float maxPeak;
    float currentRms;

    qint64 lastMaxPeakTime;

    static const quint8 segmentsSize;

    void paintSegments(QPainter &painter, float rawPeakValue, const std::vector<QColor> &segmentsColors, int offset, bool halfSize);

    void paintMaxPeakMarker(QPainter &painter, bool halfSize);

    void updateInternalValues();

    QColor interpolateColor(const QColor &start, const QColor &end, float ratio);

    void recreateInterpolatedColors();

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
