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

    virtual void recreateInterpolatedColors() = 0;

    void resizeEvent(QResizeEvent *) override;

    void paintSegments(QPainter &painter, const QRect &rect, float rawPeakValue, const std::vector<QColor> &segmentsColors);

    inline bool isVertical() const { return orientation == Qt::Vertical; }

    static float limitFloatValue(float value, float minValue = 0.0f, float maxValue = 1.0f);

    qint64 lastUpdate;

    int decayTime;

    Qt::Orientation orientation;

    static const quint8 SEGMENTS_SIZE;

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
    Q_PROPERTY(QColor maxPeakColor MEMBER maxPeakColor WRITE setMaxPeakColor)
    Q_PROPERTY(QColor peakStartColor MEMBER peakStartColor WRITE setPeaksStartColor)
    Q_PROPERTY(QColor peakEndColor MEMBER peakEndColor WRITE setPeaksEndColor)
    Q_PROPERTY(QColor dBMarksColor MEMBER dBMarksColor WRITE setDbMarksColor)

public:
    AudioMeter(QWidget *parent);

    void setPeak(float peak, float rms);
    void setPeak(float leftPeak, float rightPeak, float leftRms, float rightRms);

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
    void setMaxPeakColor(const QColor &newColor);
    void setPeaksStartColor(const QColor &newColor);
    void setPeaksEndColor(const QColor &newColor);
    void setDbMarksColor(const QColor &newColor);

public slots:
    void setStereo(bool stereo);

protected:
    void paintEvent(QPaintEvent *event) override;

    void recreateInterpolatedColors() override;

private:
    static const int MAX_PEAK_SHOW_TIME;
    static const int MAX_PEAK_MARKER_SIZE;

    QColor rmsColor;
    QColor maxPeakColor;
    QColor peakStartColor;  // start gradient color
    QColor peakEndColor;    // end gradient color
    QColor dBMarksColor;

    std::vector<QColor> peakColors;
    std::vector<QColor> rmsColors;

    //static painting flags. Turning on/off will affect all audio meters.
    static bool paintingMaxPeakMarker;
    static bool paintingPeaks;
    static bool paintingRMS;

    float currentPeak[2];
    float maxPeak[2];
    float currentRms[2];

    qint64 lastMaxPeakTime[2];

    bool stereo; // draw 2 meters?

    static const float MAX_LINEAR_VALUE;
    static const float MAX_DB_VALUE;

    void paintMaxPeakMarker(QPainter &painter, float maxLinearPeak, const QRect &rect);

    void updateInternalValues();

    uint getParallelSegments() const;

    QColor interpolateColor(const QColor &start, const QColor &end, float ratio);

    float getSmoothedLinearPeakValue(float linearValue) const;

    void drawDbMarkers(QPainter &painter);
};

inline float AudioMeter::getSmoothedLinearPeakValue(float linearValue) const
{
    const static float smoothFactor = 1/7.0f;

    return std::pow(linearValue, smoothFactor);
}

class MidiActivityMeter : public BaseMeter
{
public:
    MidiActivityMeter(QWidget *parent);
    void setSolidColor(const QColor &color);
    void setActivityValue(float value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void recreateInterpolatedColors() override;
private:
    QColor midiActivityColor;
    std::vector<QColor> colors;
    float activityValue;

    void updateInternalValues();
};

#endif
