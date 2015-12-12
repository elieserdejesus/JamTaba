#pragma once

#include <QWidget>

class PeakMeter : public QWidget
{
    Q_OBJECT

public:

   enum PeakMeterOrientation{ VERTICAL, HORIZONTAL};

   explicit PeakMeter(QWidget *parent = 0);
   virtual ~PeakMeter(){}
   void setPeak(float) ;
   void setSolidColor(QColor color);
   void setPaintMaxPeakMarker(bool paintMaxPeak);
   void setDecayTime(quint32 decayTimeInMiliseconds);
   void setOrientation(PeakMeterOrientation orientation);
   QSize minimumSizeHint() const;
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent (QResizeEvent *);
private:
    static const int LINES_MARGIN;
    QLinearGradient gradient;//used when meter is configured to fill using a gradient (default)

    float currentPeak;
    float maxPeak;

    qint64 lastMaxPeakTime;
    qint64 lastUpdate;

    int decayTime;

    static const int DEFAULT_DECAY_TIME = 800;
    static const int MAX_PEAK_SHOW_TIME = 1500;
    //static const QColor HORIZONTAL_LINES_COLOR;
    static const QColor MAX_PEAK_COLOR;

    bool usingGradient;
    QColor solidColor;//used when setSolidColor is called to disable gradient painting

    bool paintingMaxPeak;

    PeakMeterOrientation orientation;

    QLinearGradient createGradient();

    static const QColor GRADIENT_FIRST_COLOR;
    static const QColor GRADIENT_MIDDLE_COLOR;
    static const QColor GRADIENT_LAST_COLOR;
};



