#include "PeakMeter.h"
#include "Utils.h"
#include <cmath>
#include <QDebug>
#include <QResizeEvent>
#include <QStyleOption>
#include <QDateTime>
#include <QPainter>

const int PeakMeter::LINES_MARGIN = 3;
const QColor PeakMeter::GRADIENT_FIRST_COLOR = Qt::red;
const QColor PeakMeter::GRADIENT_MIDDLE_COLOR = QColor(0, 200, 0);
const QColor PeakMeter::GRADIENT_LAST_COLOR = QColor(Qt::darkGreen) ;

//const QColor PeakMeter::HORIZONTAL_LINES_COLOR = QColor(255,255,255, 50);
const QColor PeakMeter::MAX_PEAK_COLOR = QColor(255, 255, 255, 120);

PeakMeter::PeakMeter(QWidget *)
    :   currentPeak(0),
        maxPeak(0),
        lastUpdate(QDateTime::currentMSecsSinceEpoch()),
        usingGradient(true),
        paintingMaxPeak(true),
        decayTime(DEFAULT_DECAY_TIME),
        orientation(VERTICAL)
{
    gradient = createGradient();
    setAttribute(Qt::WA_NoBackground);
    update();
}

QSize PeakMeter::minimumSizeHint() const{
    int w = (orientation == VERTICAL) ? 2 : width();
    int h = orientation == VERTICAL ? height() : 2;
    return QSize(w, h);
}

void PeakMeter::setOrientation(PeakMeterOrientation orientation){
    this->orientation = orientation;
    this->gradient = createGradient();
    style()->unpolish(this);
    style()->polish(this);
    update();
}


void PeakMeter::resizeEvent ( QResizeEvent * /*ev*/)  {
    gradient = createGradient();

}


QLinearGradient PeakMeter::createGradient(){
    int x1 = orientation == VERTICAL ? 0 : width()-1;
    int y1 = 0;
    int x2 = orientation == VERTICAL ? 0 : 0;
    int y2 = orientation == VERTICAL ? height()-1 : 0;
    QLinearGradient linearGradient( x1, y1, x2, y2);
    linearGradient.setColorAt(0, GRADIENT_FIRST_COLOR);
    linearGradient.setColorAt(0.4, GRADIENT_MIDDLE_COLOR);
    linearGradient.setColorAt(0.8, GRADIENT_LAST_COLOR);
    return linearGradient;
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

#include "Utils.h"
void PeakMeter::paintEvent(QPaintEvent *){
    QPainter painter(this);

    //to allow stylesheets
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    //meter
    if(isEnabled()){
        int value = Utils::poweredGainToLinear(currentPeak) * ((orientation == VERTICAL) ? height() : width());
        QRect rect;
        rect.setLeft(1);
        rect.setRight( orientation == VERTICAL ? width()-2 : value);
        rect.setTop( orientation == VERTICAL ? height() - value : 0 );
        rect.setBottom( height()-1 );
        painter.fillRect(rect, usingGradient ? gradient : QBrush(solidColor) );

        //draw max peak marker
        if(maxPeak > 0 && paintingMaxPeak){
            float linearPeak = Utils::poweredGainToLinear(maxPeak);
            int peakY = orientation == VERTICAL ? (height() - linearPeak * height()) : 0;
            int peakX = orientation == VERTICAL ? 0 : (linearPeak * width());
            int peakWidth = orientation == VERTICAL ? width() : 3;
            int peakHeight = orientation == VERTICAL ? 3 : height();
            painter.fillRect(peakX, peakY, peakWidth, peakHeight, MAX_PEAK_COLOR);
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
