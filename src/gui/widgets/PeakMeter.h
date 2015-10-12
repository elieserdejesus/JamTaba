#pragma once

#include <QWidget>


class PeakMeter : public QWidget
{
    Q_OBJECT

public:
   explicit PeakMeter(QWidget *parent = 0);
   virtual ~PeakMeter(){

   }

   void setPeak(float) ;

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent (QResizeEvent *);

private:
    static const int LINES_MARGIN;
    QLinearGradient gradient;
    static const QColor HORIZONTAL_LINES_COLOR;
    static const QColor MAX_PEAK_COLOR;

    float currentPeak;
    float maxPeak;
    qint64 lastMaxPeakTime;

    qint64 lastUpdate;// QDateTime::currentMSecsSinceEpoch()
    static const int DECAY_TIME = 800;
    static const int MAX_PEAK_SHOW_TIME = 1500;
};



