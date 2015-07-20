#include "CircularIntervalProgressDisplay.h"

#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFontMetrics>
#include <cmath>

CircularIntervalProgressDisplay::CircularIntervalProgressDisplay(QWidget*parent)
    :QWidget(parent){
    setBeatsPerInterval(33);

    setBeatsPerAccent(4);
    //setShowAccents(false);

    initialize();
}

void CircularIntervalProgressDisplay::resizeEvent(QResizeEvent * event){
    Q_UNUSED(event);
    initialize();
}


void CircularIntervalProgressDisplay::setShowAccents(bool showAccents) {
    this->showAccents = showAccents;
    update();
}


void CircularIntervalProgressDisplay::setCurrentBeat(int beat) {
    if (beat != currentBeat) {
        currentBeat = beat % beats;
        update();
    }
}

void CircularIntervalProgressDisplay::setBeatsPerAccent(int beats) {
    beatsPerAccent = beats;
    update();
}

void CircularIntervalProgressDisplay::setBeatsPerInterval(int beats) {
    if (beats > 0 && beats <= 64) {
        this->beats = beats;
        if (beatsPerAccent <= 0) {
            beatsPerAccent = beats / 2;
        }
        initialize();
    }
}

void CircularIntervalProgressDisplay::paintEvent(QPaintEvent* e){
    Q_UNUSED(e);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    if (beats <= 32) {
        drawCircles(&p, radius, beats, 0);
    } else {
        int elementsInFirstCircle = beats / 2;
        int elementsInSecondCircle = beats - elementsInFirstCircle;
        drawCircles(&p, radius, elementsInFirstCircle, 0);
        drawCircles(&p, (int) (radius * 0.75), elementsInSecondCircle, elementsInFirstCircle);
    }

    //draw current beat in center
    QString numberString( QString::number(currentBeat + 1) + "/" + QString::number(beats));
    int strWidth = fontMetrics().width(numberString);
    p.drawText(centerX - strWidth / 2, centerY, numberString);

}


void CircularIntervalProgressDisplay::setSliceNumberColor(QColor sliceNumberColor) {
    this->sliceNumberColor = sliceNumberColor;
    update();
}


void CircularIntervalProgressDisplay::drawCircles(QPainter* p, int theRadius, int circles, int offset) {
    double angle = -3.141592653589793238462643383279502884 / 2.0;
    QRadialGradient bgRadial(centerX, centerY, radius * 2);

    const float FIRST_COLOR_POSITION = 0.44f;
    const float SECOND_COLOR_POSITION = 0.6f;

    for (int i = 0; i < circles; i++) {
        //QBrush brush(Qt::gray);// ( (i + startIterval) <= currentBeat ? Qt::green : Qt::gray );
        //QColor penColor( (i + offset) <= currentBeat ? (Qt::black) : (Qt::lightGray) ) ;

        int x = centerX + (theRadius * std::cos(angle));// - OVAL_SIZE/2;
        int y = (centerY + (theRadius * std::sin(angle)));// - OVAL_SIZE/2;


        //beat not played yet
        bgRadial.setColorAt(FIRST_COLOR_POSITION, Qt::darkGray);
        bgRadial.setColorAt(SECOND_COLOR_POSITION, Qt::gray);

        bool isIntervalFirstBeat = i + offset == 0;
        bool isMeasureFirstBeat = (i + offset) % beatsPerAccent == 0;
        if (i + offset == currentBeat && (isIntervalFirstBeat || isMeasureFirstBeat)) {//first beats
            if( isIntervalFirstBeat || isShowingAccents() ){
                bgRadial.setColorAt(FIRST_COLOR_POSITION, Qt::yellow);//accent beat colors
                bgRadial.setColorAt(SECOND_COLOR_POSITION, Qt::red);
            }
        }
        else{
            if((i + offset) % beatsPerAccent == 0 && (i+offset) > currentBeat){
                bgRadial.setColorAt(FIRST_COLOR_POSITION, Qt::lightGray); //accent marks
                bgRadial.setColorAt(SECOND_COLOR_POSITION, Qt::gray);
            }
            else if ( (i + offset) <= currentBeat) {
                if(i + offset < currentBeat){
                    bgRadial.setColorAt(FIRST_COLOR_POSITION, QColor(0, 0, 0, 30)); //played beats
                    bgRadial.setColorAt(SECOND_COLOR_POSITION, QColor(0, 0, 0, 60));
                }
                else{//the current beat is highlighted
                    bgRadial.setColorAt(FIRST_COLOR_POSITION, Qt::green); //played beat highlight colors
                    bgRadial.setColorAt(SECOND_COLOR_POSITION, Qt::darkGray);
                }
            }
        }

        p->setBrush(bgRadial);
        p->setPen(Qt::NoPen);
        p->drawEllipse( QPoint(x, y), OVAL_SIZE, OVAL_SIZE);

        angle -= (2 * -3.141592653589793238462643383279502884) / circles;
    }
}


void CircularIntervalProgressDisplay::initialize() {
    int size = std::min(width()-1, height()-1) - OVAL_SIZE*2;
    radius = size / 2;
    centerX = width() / 2;
    centerY = height() / 2;
}

