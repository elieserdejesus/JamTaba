#ifndef PEAK_METER_H
#define PEAK_METER_H

#include <QFrame>

class PeakMeter : public QFrame
{
    Q_OBJECT

public:

    explicit PeakMeter(QWidget *parent = 0);
    virtual ~PeakMeter();

    void setPeak(float, float rms);
    void setSolidColor(const QColor &color);

    static void setPaintMaxPeakMarker(bool paintMaxPeak);
    static void setPaintingPeaks(bool paintPeaks);
    static void setPaintingRMS(bool paintRMS);

    void setDecayTime(quint32 decayTimeInMiliseconds);
    void setOrientation(Qt::Orientation orientation);
    QSize minimumSizeHint() const override;

    inline static bool isPaintintMaxPeakMarker() { return paintingMaxPeakMarker; }
    inline static bool isPaintingRMS() { return paintingRMS; }
    inline static bool isPaintingPeaks() { return paintingPeaks; }

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *);

private:

    inline bool isVertical() const
    {
        return orientation == Qt::Vertical;
    }

    QLinearGradient gradient;// used when meter is configured to fill using a gradient (default)

    float currentPeak;
    float maxPeak;

    float currentRms;

    qint64 lastMaxPeakTime;
    qint64 lastUpdate;

    int decayTime;

    bool usingGradient;
    QColor solidColor;// used when setSolidColor is called to disable gradient painting

    //static painting flags. Turning on/off will affect all meters.
    static bool paintingMaxPeakMarker;
    static bool paintingPeaks;
    static bool paintingRMS;

    Qt::Orientation orientation;

    QLinearGradient createGradient();

    QRectF getPaintRect(float peakValue) const;

    static float limitFloatValue(float value, float minValue = 0.0f, float maxValue = 1.0f);

    static const QColor GRADIENT_FIRST_COLOR;
    static const QColor GRADIENT_MIDDLE_COLOR;
    static const QColor GRADIENT_LAST_COLOR;

    static const QColor RMS_COLOR;

    static const int LINES_MARGIN;
    static const int MIN_SIZE;

    static const int DEFAULT_DECAY_TIME;
    static const int MAX_PEAK_SHOW_TIME;
    static const QColor MAX_PEAK_COLOR;
    static const int MAX_PEAK_MARKER_SIZE;
};

#endif
