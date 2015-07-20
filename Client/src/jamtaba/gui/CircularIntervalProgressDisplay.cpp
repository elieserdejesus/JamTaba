#include "CircularIntervalProgressDisplay.h"

#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QStyleOption>
#include <cmath>

//++++++++++++++++++++++++++++++++++++++++++++++++++
CircularIntervalProgressDisplay::CircularIntervalProgressDisplay(QWidget*parent)
    :QWidget(parent), ovalSize(PREFERRED_OVAL_SIZE){
    setBeatsPerInterval(64);

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
        if(beats <= 16){
            ovalSize = PREFERRED_OVAL_SIZE;
        }
        else{
            ovalSize = PREFERRED_OVAL_SIZE - beats/64.0 * 2;
        }
        initialize();
    }
}

void CircularIntervalProgressDisplay::paintEvent(QPaintEvent* e){
    Q_UNUSED(e);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QColor textColor( palette().color(QPalette::WindowText));

    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    if (beats <= 32) {
        if(beats > 24){
            drawCircles(&p, horizontalRadius, verticalRadius, beats, 0);//draw a ellipse
        }
        else{
            int radius = std::min(horizontalRadius, verticalRadius);
            drawCircles(&p, radius, radius, beats, 0);//draw a circle
        }
    } else {
        int elementsInFirstCircle = beats / 2;
        int elementsInSecondCircle = beats - elementsInFirstCircle;
        drawCircles(&p, horizontalRadius, verticalRadius, elementsInFirstCircle, 0);
        drawCircles(&p, (int) (horizontalRadius * 0.75), (int) (verticalRadius * 0.75), elementsInSecondCircle, elementsInFirstCircle);
    }
    //drawCircles(&p, radius, beats, 0);

    //draw current beat text in center
    p.setPen(textColor);//use text color form pallete, allowing style sheet
    QString numberString( QString::number(currentBeat + 1) + " / " + QString::number(beats));
    if(currentBeat + 1 < 10){
        numberString.insert(0, ' ');
    }
    int strWidth = fontMetrics().width(numberString);
    p.drawText(centerX - strWidth / 2, centerY, numberString);
    //p.drawText(5, fontMetrics().height(), numberString);
}


void CircularIntervalProgressDisplay::setSliceNumberColor(QColor sliceNumberColor) {
    this->sliceNumberColor = sliceNumberColor;
    update();
}


void CircularIntervalProgressDisplay::drawCircles(QPainter* p, int hRadius, int vRadius, int circles, int offset) {
    double angle = -3.141592653589793238462643383279502884 / 2.0;
    //QRadialGradient bgRadial(centerX, centerY, radius * 2);

//    const float FIRST_COLOR_POSITION = 0.44f;
//    const float SECOND_COLOR_POSITION = 0.6f;
      const float FIRST_COLOR_POSITION = 0.0f;
      const float SECOND_COLOR_POSITION = 0.99f;

    //float hRadius = width()/2;
    //float vRadius = height()/2;

    for (int i = 0; i < circles; i++) {

        int x = centerX + (hRadius  * std::cos(angle));// - ovalSize /2;
        int y = (centerY + (vRadius * std::sin(angle))) + ovalSize/2;

        QRadialGradient brush(x + ovalSize/2, y, ovalSize*2);
        QPen pen(Qt::NoPen);

        //p->drawLine(0, centerY, width(), centerY);

        //beat not played yet
        brush.setColorAt(FIRST_COLOR_POSITION, Qt::gray);
        brush.setColorAt(SECOND_COLOR_POSITION, Qt::black);

        bool isIntervalFirstBeat = i + offset == 0;
        bool isMeasureFirstBeat = (i + offset) % beatsPerAccent == 0;
        if (i + offset == currentBeat && (isIntervalFirstBeat || isMeasureFirstBeat)) {//first beats
            if( isIntervalFirstBeat || isShowingAccents() ){
                brush.setColorAt(FIRST_COLOR_POSITION, QColor(255, 100, 100));//accent beat colors
                brush.setColorAt(SECOND_COLOR_POSITION, Qt::red);
            }
            else{
                brush.setColorAt(FIRST_COLOR_POSITION, Qt::green); //playing beat highlight colors
                brush.setColorAt(SECOND_COLOR_POSITION, Qt::darkGreen);
            }
            pen.setColor(Qt::darkGray);
            pen.setStyle(Qt::SolidLine);
        }
        else{
            if((i + offset) % beatsPerAccent == 0 && (i+offset) > currentBeat){
                if(isShowingAccents()){
                    brush.setColorAt(FIRST_COLOR_POSITION, Qt::white); //accent marks
                    brush.setColorAt(SECOND_COLOR_POSITION, Qt::gray);

                    pen.setColor(Qt::gray);
                    pen.setStyle(Qt::SolidLine);
                }
            }
            else if ( (i + offset) <= currentBeat) {
                if(i + offset < currentBeat){
                    brush.setColorAt(FIRST_COLOR_POSITION, QColor(0, 0, 0, 20)); //played beats
                    brush.setColorAt(SECOND_COLOR_POSITION, QColor(0, 0, 0, 35));
                }
                else{//the current beat is highlighted
                    brush.setColorAt(FIRST_COLOR_POSITION, Qt::green); //playing beat highlight colors
                    brush.setColorAt(SECOND_COLOR_POSITION, Qt::darkGreen);

                    pen.setColor(Qt::darkGreen);
                    pen.setStyle(Qt::SolidLine);
                }
            }
        }

        p->setBrush(brush);
        p->setPen( pen );
        int size = (i + offset) == currentBeat ? ovalSize + 1 : ovalSize;
        p->drawEllipse( QPoint(x, y), size, size);

        if(offset > 0 && currentBeat < offset){//is drawing the first circles?
            p->setBrush(QColor(255,255, 255, 200)); //the internal circles are drawed transparent
            p->drawEllipse( QPoint(x, y), size, size);
        }

        float spirals = 1;// (beats <= 16) ? 1 :  (1 + beats/64.0 * 2);
        angle -= (2 * -3.141592653589793238462643383279502884 * spirals) / circles;

        //float radiusReductionFactor = (beats <= 16) ? 0 :  (64.0/beats * 0.01);
        //theRadius -= radius * radiusReductionFactor;//[0.01, 0.02]   64 = 0.01    32 = 0.02
    }
}


void CircularIntervalProgressDisplay::initialize() {
    horizontalRadius = width()/2 - ovalSize*2;
    verticalRadius = height()/2 - ovalSize*2;
    centerX = width() / 2;
    centerY = height() / 2;
}

