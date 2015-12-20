#include "PeakMeter.h"
#include "Utils.h"
#include <cmath>
#include <QDebug>
#include <QResizeEvent>
#include <QStyleOption>
#include <QDateTime>
#include <QPainter>

const int PeakMeter::LINES_MARGIN = 3;
const int PeakMeter::MIN_SIZE = 1;

const QColor PeakMeter::GRADIENT_FIRST_COLOR = Qt::red;
const QColor PeakMeter::GRADIENT_MIDDLE_COLOR = QColor(0, 200, 0);
const QColor PeakMeter::GRADIENT_LAST_COLOR = QColor(Qt::darkGreen) ;
const QColor PeakMeter::MAX_PEAK_COLOR = QColor(0, 0, 0, 80);

const int PeakMeter::MAX_PEAK_MARKER_SIZE = 2;
const int PeakMeter::DEFAULT_DECAY_TIME = 2000;
const int PeakMeter::MAX_PEAK_SHOW_TIME = 1500;

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
    bool isVerticalMeter = isVertical();
    int w = isVerticalMeter ? MIN_SIZE : width();
    int h = isVerticalMeter ? height() : MIN_SIZE;
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
    int x1 = isVertical() ? 0 : width()-1;
    int y1 = 0;
    int x2 = isVertical() ? 0 : 0;
    int y2 = isVertical() ? height()-1 : 0;
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

void PeakMeter::paintEvent(QPaintEvent *){
    QPainter painter(this);

    //to allow stylesheets
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    //meter
    if(isEnabled()){
        bool isVerticalMeter = isVertical();
        int value = Utils::poweredGainToLinear(currentPeak) * (isVerticalMeter ? height() : width());
        QRect rect(1,
                   isVerticalMeter ? height() - value : 0,
                   isVerticalMeter ? width()-2 : value,
                   height()-1 );
        painter.fillRect(rect, usingGradient ? gradient : QBrush(solidColor) );

        //draw max peak marker
        if(maxPeak > 0 && paintingMaxPeak){
            float linearPeak = Utils::poweredGainToLinear(maxPeak);
            QRect peakRect( isVerticalMeter ? 0 : (linearPeak * width()),
                            isVerticalMeter ? (height() - linearPeak * height()) : 0,
                            isVerticalMeter ? width() : MAX_PEAK_MARKER_SIZE,
                            isVerticalMeter ? MAX_PEAK_MARKER_SIZE : height() );
            painter.fillRect(peakRect, MAX_PEAK_COLOR);
        }
    }

    quint64 now = QDateTime::currentMSecsSinceEpoch();

    //decay
    long ellapsedTimeFromLastUpdate = now - lastUpdate;
    currentPeak -= (float)ellapsedTimeFromLastUpdate/decayTime;
    if(currentPeak < 0){
        currentPeak = 0;
    }
    lastUpdate = now;

    //max peak
    long ellapsedTimeFromLastMaxPeak = now - lastMaxPeakTime;
    if(ellapsedTimeFromLastMaxPeak >= MAX_PEAK_SHOW_TIME){
        maxPeak = 0;
    }
}
