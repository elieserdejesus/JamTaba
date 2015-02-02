#pragma once

#include <QWidget>
#include <QDateTime>
#include <QPainter>
#include <cmath>
#include <climits>
#include <QDebug>
#include <QDateTime>

namespace Gui {




class PeakMeter : public QWidget
{
    Q_OBJECT

public:
   PeakMeter(QWidget *parent = 0);



   virtual ~PeakMeter(){

   }

   void setPeak(float) ;

protected:
    void paintEvent(QPaintEvent */*event*/);
    void resizeEvent (QResizeEvent *);

private:
    QLinearGradient gradient;
    //QColor borderColor;
    //QColor bgColor;
    //QBrush brush;

    float peak;

    //static const QColor DEFAULT_BORDER_COLOR;
    //static const QColor DEFAULT_BG_COLOR;

    qint64 lastUpdate;// QDateTime::currentMSecsSinceEpoch()
    unsigned long decayTime;
};

}

