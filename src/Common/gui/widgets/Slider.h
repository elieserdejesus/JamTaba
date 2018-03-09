#ifndef _JTBA_SLIDER_
#define _JTBA_SLIDER_

#include <QSlider>

class AudioSlider : public QSlider
{
    Q_OBJECT

    // custom properties defined in CSS files
    Q_PROPERTY(QColor rmsColor MEMBER rmsColor WRITE setRmsColor)
    Q_PROPERTY(QColor maxPeakColor MEMBER maxPeakColor WRITE setMaxPeakColor)
    Q_PROPERTY(QColor peakStartColor MEMBER peakStartColor WRITE setPeaksStartColor)
    Q_PROPERTY(QColor peakEndColor MEMBER peakEndColor WRITE setPeaksEndColor)
    Q_PROPERTY(QColor dBMarksColor MEMBER dBMarksColor WRITE setDbMarksColor)
    Q_PROPERTY(bool drawSegments MEMBER drawSegments WRITE setDrawSegments)

public:
    explicit AudioSlider(QWidget *parent = nullptr);

    void setMeterDecayTime(quint32 decayTimeInMiliseconds);

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

    inline bool isStereo() const { return stereo; }

    void setPeak(float peak, float rms);
    void setPeak(float leftPeak, float rightPeak, float leftRms, float rightRms);

    void setRmsColor(const QColor &newColor);
    void setMaxPeakColor(const QColor &newColor);
    void setPeaksStartColor(const QColor &newColor);
    void setPeaksEndColor(const QColor &newColor);
    void setDbMarksColor(const QColor &newColor);
    void setDrawSegments(bool drawSegments);

public slots:
    void setStereo(bool stereo);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void mouseDoubleClickEvent(QMouseEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;

private slots:
    void showToolTip();
    void updateToolTipValue();

private:
    void drawMarker(QPainter &painter);
    qreal getMarkerPosition() const;

    void recreateInterpolatedColors();
    QColor interpolateColor(const QColor &start, const QColor &end, float ratio);

    void paintSegments(QPainter &painter, const QRectF &rect, float rawPeakValue, const std::vector<QColor> &segmentsColors, bool drawSegments = true);

    bool isVertical() const;

    static float limitFloatValue(float value, float minValue = 0.0f, float maxValue = 1.0f);

    void rebuildDbMarkersPixmap();
    void drawDbMarkers(QPainter &painter);
    std::vector<float> createDBValues();
    //static qreal getSmoothedLinearPeakValue(qreal linearValue);

    void updateInternalValues();

    uint getParallelSegments() const;

    qreal getPeakPosition(qreal linearPeak, qreal rectSize, qreal peakValueOffset);

    void paintMaxPeakMarker(QPainter &painter, qreal maxPeakPosition, const QRectF &rect);

    void paintSliderHandler(QPainter &painter);
    void paintSliderGroove(QPainter &painter);

    qreal getMaxLinearValue() const;
    qreal getMaxDbValue() const;
    //qreal getMaxSmoothedValue() const;

    QColor rmsColor;
    QColor maxPeakColor;
    QColor peakStartColor;  // start gradient color
    QColor peakEndColor;    // end gradient color
    QColor dBMarksColor;

    bool drawSegments;

    std::vector<QColor> peakColors;
    std::vector<QColor> rmsColors;

    // static painting flags. Turning on/off will affect all audio meters.
    static bool paintingMaxPeakMarker;
    static bool paintingPeaks;
    static bool paintingRMS;

    float currentPeak[2];
    float maxPeak[2];
    float currentRms[2];

    qint64 lastMaxPeakTime[2];

    bool stereo; // draw 2 meters?

    QPixmap dbMarkersPixmap;
    bool paintingDbMarkers;

    qint64 lastUpdate;

    int decayTime;

    static const quint8 SEGMENTS_SIZE;

    static const uint LINES_MARGIN;
    static const uint DEFAULT_DECAY_TIME;

    //static const float MAX_SMOOTHED_LINEAR_VALUE;
    //static const float MIN_SMOOTHED_LINEAR_VALUE;
    //static const float MAX_LINEAR_VALUE;
    static const float MIN_LINEAR_VALUE;
    //static const float MAX_DB_VALUE;
    static const float MIN_DB_VALUE;

    //static const float RESIZE_FACTOR;

    static const int MAX_PEAK_SHOW_TIME;
    static const int MAX_PEAK_MARKER_SIZE;
    static const int MIN_SIZE;
};

inline qreal AudioSlider::getMaxLinearValue() const
{
    return maximum()/100.0;
}

inline bool AudioSlider::isVertical() const
{
    return orientation() == Qt::Vertical;
}

// -----------------------------------------------------

class PanSlider : public QSlider
{
    Q_OBJECT

public:
    explicit PanSlider(QWidget *parent = nullptr);

protected:
    void mouseDoubleClickEvent(QMouseEvent *ev) override;
    void paintEvent(QPaintEvent *ev) override;

private slots:
    void updateToolTipValue();

private:
    void drawMarker(QPainter &painter);
    void showToolTip();
};

#endif
