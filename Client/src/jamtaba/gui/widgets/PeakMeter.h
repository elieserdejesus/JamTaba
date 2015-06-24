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
    void paintEvent(QPaintEvent */*event*/);
    void resizeEvent (QResizeEvent *);

private:
    static const int LINES_MARGIN;
    QLinearGradient gradient;
    QColor horizontalLinesColor;

    float peak;

    qint64 lastUpdate;// QDateTime::currentMSecsSinceEpoch()
    unsigned long decayTime;
};



