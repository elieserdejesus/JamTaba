#include "PeakMeter.h"
#include <cmath>
#include <QDebug>
#include <QResizeEvent>
#include <QStyleOption>
#include <QDateTime>
#include <QPainter>

const int PeakMeter::LINES_MARGIN = 3;


//const QColor PeakMeter::HORIZONTAL_LINES_COLOR = QColor(255,255,255, 50);
const QColor PeakMeter::MAX_PEAK_COLOR = QColor(255, 255, 255, 120);

PeakMeter::PeakMeter(QWidget *)
    :   currentPeak(0),
        maxPeak(0),
        lastUpdate(QDateTime::currentMSecsSinceEpoch()),
        usingGradient(true),
        paintingMaxPeak(true),
        decayTime(DEFAULT_DECAY_TIME)
{
    gradient = QLinearGradient(0, 0, 0, height()-1);
    gradient.setColorAt(0, Qt::red);
    gradient.setColorAt(0.4, QColor(0, 200, 0));
    gradient.setColorAt(0.8, Qt::darkGreen);

    setAttribute(Qt::WA_NoBackground);

    update();
}

void PeakMeter::setDecayTime(quint32 decayTimeInMiliseconds){
    this->decayTime = decayTimeInMiliseconds;
    update();
}

void PeakMeter::setPaintMaxPeakMarker(bool paintMaxPeak){
    this->paintingMaxPeak = paintMaxPeak;
    update();
}

void PeakMeter::setSolidColor(QColor color){
    this->usingGradient = false;
    this->solidColor = color;
    update();
}

void PeakMeter::setPeak(float peak){
    if(peak < 0){
        peak = 0;
    }
    if(peak > this->currentPeak){
        this->currentPeak = peak;
        if(peak > maxPeak){
            maxPeak = peak;
            lastMaxPeakTime = QDateTime::currentMSecsSinceEpoch();
        }
    }
    update();
}

void PeakMeter::paintEvent(QPaintEvent *){
    QPainter painter(this);

    //to allow stylesheets
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    //meter
    if(isEnabled()){
        //draw bottom to up
        painter.fillRect(1, height()-1 , width()-2, -currentPeak * height(), usingGradient ? gradient : QBrush(solidColor) );

        //draw max peak marker
        if(maxPeak > 0 && paintingMaxPeak){
            int peakY = height() - maxPeak * height();
            painter.fillRect(0, peakY, width(), 3, MAX_PEAK_COLOR);
        }
    }

    //decay
    long ellapsedTimeFromLastUpdate = QDateTime::currentMSecsSinceEpoch() - lastUpdate;
    currentPeak -= (float)ellapsedTimeFromLastUpdate/decayTime;
    if(currentPeak < 0){
        currentPeak = 0;
    }
    lastUpdate = QDateTime::currentMSecsSinceEpoch();

    //max peak
    long ellapsedTimeFromLastMaxPeak = QDateTime::currentMSecsSinceEpoch() - lastMaxPeakTime;
    if(ellapsedTimeFromLastMaxPeak >= MAX_PEAK_SHOW_TIME){
        maxPeak = 0;
    }
}

void PeakMeter::resizeEvent ( QResizeEvent * /*ev*/)  {
    gradient.setFinalStop(0, height()-1);
}
