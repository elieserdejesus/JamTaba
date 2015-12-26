#pragma once

#include <QWidget>

class PeakMeter : public QWidget
{
    Q_OBJECT

public:

    enum PeakMeterOrientation {
        VERTICAL, HORIZONTAL
    };

    explicit PeakMeter(QWidget *parent = 0);
    virtual ~PeakMeter()
    {
    }

    void setPeak(float);
    void setSolidColor(QColor color);
    void setPaintMaxPeakMarker(bool paintMaxPeak);
    void setDecayTime(quint32 decayTimeInMiliseconds);
    void setOrientation(PeakMeterOrientation orientation);
    QSize minimumSizeHint() const;
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *);
private:

    inline bool isVertical() const
    {
        return orientation == VERTICAL;
    }

    QLinearGradient gradient;// used when meter is configured to fill using a gradient (default)

    float currentPeak;
    float maxPeak;

    qint64 lastMaxPeakTime;
    qint64 lastUpdate;

    int decayTime;

    bool usingGradient;
    QColor solidColor;// used when setSolidColor is called to disable gradient painting

    bool paintingMaxPeak;

    PeakMeterOrientation orientation;

    QLinearGradient createGradient();

    static const QColor GRADIENT_FIRST_COLOR;
    static const QColor GRADIENT_MIDDLE_COLOR;
    static const QColor GRADIENT_LAST_COLOR;

    static const int LINES_MARGIN;
    static const int MIN_SIZE;

    static const int DEFAULT_DECAY_TIME;
    static const int MAX_PEAK_SHOW_TIME;
    static const QColor MAX_PEAK_COLOR;
    static const int MAX_PEAK_MARKER_SIZE;
};
