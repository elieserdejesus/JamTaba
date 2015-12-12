#include "IntervalProgressDisplay.h"

#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QDebug>
#include <QStyleOption>
#include <cmath>

QColor IntervalProgressDisplay::PLAYED_BEATS_FIRST_COLOR = QColor(200, 200, 200);
QColor IntervalProgressDisplay::PLAYED_BEATS_SECOND_COLOR = QColor(180, 180, 180);
QColor IntervalProgressDisplay::PATH_COLOR = QColor(0,0,0, 20);

const double IntervalProgressDisplay::PI = 3.141592653589793238462643383279502884;

//LINEAR PAINTING CONSTANTS
const QColor IntervalProgressDisplay::LINEAR_SLICE_NUMBER_COLOR = QColor(120, 120, 120);
const QColor IntervalProgressDisplay::LINEAR_BG_COLOR = QColor(230, 230, 230);
const QColor IntervalProgressDisplay::LINEAR_BG_SECOND_COLOR = QColor(215, 215, 215);
const QColor IntervalProgressDisplay::LINEAR_BORDER_COLOR = QColor(200, 200, 200);
const QColor IntervalProgressDisplay::LINEAR_TEXT_COLOR = QColor(180, 180, 180);// Color.LIGHT_GRAY;
const QColor IntervalProgressDisplay::LINEAR_ACCENT_BORDER_COLOR = QColor(160, 160, 160);

//const QFont IntervalProgressDisplay::SMALL_FONT = QFont("Verdana", 7);
//const QFont IntervalProgressDisplay::BIG_FONT = QFont("Verdana", 10);

//++++++++++++++++++++++++++++++++++++++++++++++++++
IntervalProgressDisplay::IntervalProgressDisplay(QWidget*parent)
    :QWidget(parent),
      SMALL_FONT("Verdana, 7"),
      BIG_FONT("Verdana, 10"),
      ovalSize(PREFERRED_OVAL_SIZE),
      paintMode(PaintMode::LINEAR),
      showAccents(false){

    setAttribute(Qt::WA_NoBackground);

    setBeatsPerInterval(32);

    setShowAccents(false);

    initialize();
}

IntervalProgressDisplay::~IntervalProgressDisplay(){
    //delete comboBoxPaintMode;
}

void IntervalProgressDisplay::resizeEvent(QResizeEvent * event){
    Q_UNUSED(event);
    initialize();
}


void IntervalProgressDisplay::setShowAccents(bool showAccents) {
    this->showAccents = showAccents;
    update();
}

void IntervalProgressDisplay::setCurrentBeat(int beat) {
    if (beat != currentBeat) {
        currentBeat = beat % beats;
        update();
    }
}

void IntervalProgressDisplay::setBeatsPerAccent(int beats) {
    beatsPerAccent = beats;
    if(!isShowingAccents() && beatsPerAccent > 1){
        this->showAccents = true;
    }
    update();
}

void IntervalProgressDisplay::setBeatsPerInterval(int beats) {
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



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void IntervalProgressDisplay::setPaintMode(PaintMode mode){
    if(mode != this->paintMode){
        this->paintMode = mode;
        updateGeometry();
        repaint();
    }
}

//++++++++++++++++++++++++++  ELLIPSE PAINTING ++++++++++++++++++++++++++++++++++++++
void IntervalProgressDisplay::paintCircular(QPainter &p, QColor textColor){
    int radius = std::min(horizontalRadius, verticalRadius);
    paintElliptical(p, textColor, radius , radius);
}

void IntervalProgressDisplay::paintEllipticalPath(QPainter &p, int hRadius, int vRadius){
    QPen pen(Qt::SolidLine);
    pen.setColor(PATH_COLOR);
    p.setPen(pen);
    p.setBrush(Qt::BrushStyle::NoBrush);
    p.drawEllipse(QPoint(width()/2, height()/2 + 2), hRadius, vRadius);
}

void IntervalProgressDisplay::paintElliptical(QPainter &p, QColor textColor, int hRadius, int vRadius){
    if (beats <= 32) {
        drawBeatCircles(p, hRadius, vRadius, beats, 0);//draw a ellipse
    } else {
        int elementsInOuterEllipse= beats / 2;
        int elementsInNestedEllipse = beats - elementsInOuterEllipse;
        drawBeatCircles(p, hRadius, vRadius, elementsInOuterEllipse, 0);
        drawBeatCircles(p, (int) (hRadius * 0.75), (int) (vRadius * 0.75), elementsInNestedEllipse, elementsInOuterEllipse);
    }
    //drawCircles(&p, radius, beats, 0);

    //draw current beat text in center
    p.setPen(textColor);//use text color form pallete, allowing style sheet
    QString numberString( QString::number(currentBeat + 1) + " / " + QString::number(beats));
//    if(currentBeat + 1 < 10){
//        numberString.insert(0, ' ');
//    }
    int strWidth = fontMetrics().width(numberString);
    p.drawText(centerX - strWidth / 2, centerY + fontMetrics().height()/2 , numberString);
    //p.drawText(5, fontMetrics().height(), numberString);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void IntervalProgressDisplay::paintEvent(QPaintEvent* e){
    Q_UNUSED(e);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QColor textColor( palette().color(QPalette::WindowText));

    QStyleOption opt;
    opt.init(this); //allow style sheet
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    switch (paintMode) {
    case ELLIPTICAL:
        paintElliptical(p, textColor, std::min(verticalRadius * 3, horizontalRadius), verticalRadius);
        break;
    case CIRCULAR:
        paintCircular(p, textColor);
        break;
    case LINEAR:
        drawPoints(&p, height() / 2, 0, this->beats);
        break;
    default:
        paintElliptical(p, textColor, horizontalRadius, verticalRadius);
        break;
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void IntervalProgressDisplay::setSliceNumberColor(QColor sliceNumberColor) {
    this->sliceNumberColor = sliceNumberColor;
    update();
}


void IntervalProgressDisplay::drawBeatCircles(QPainter& p, int hRadius, int vRadius, int beatCircles, int offset) {
    paintEllipticalPath(p, hRadius, vRadius);

    double angle = -PI / 2.0;

    const float FIRST_COLOR_POSITION = 0.5f;
    const float SECOND_COLOR_POSITION = 0.7f;

    //float hRadius = width()/2;
    //float vRadius = height()/2;

    for (int i = 0; i < beatCircles; i++) {

        int x = centerX + (hRadius  * std::cos(angle));// - ovalSize /2;
        int y = (centerY + (vRadius * std::sin(angle))) + ovalSize/2;

        QRadialGradient brush(x - ovalSize/3, y - ovalSize/3, ovalSize*2);
        QPen pen(Qt::NoPen);

        //p->drawLine(0, centerY, width(), centerY);

        //beat not played yet
        //brush.setColorAt(FIRST_COLOR_POSITION, Qt::gray);
        //brush.setColorAt(SECOND_COLOR_POSITION, Qt::black);

        bool isIntervalFirstBeat = i + offset == 0;
        bool isMeasureFirstBeat = (i + offset) % beatsPerAccent == 0;
        if (i + offset == currentBeat && (isIntervalFirstBeat || isMeasureFirstBeat)) {//first beats
            if( isIntervalFirstBeat || isShowingAccents() ){
                brush.setColorAt(FIRST_COLOR_POSITION, Qt::green);//accent beat colors
                brush.setColorAt(SECOND_COLOR_POSITION, Qt::darkGreen);
            }
            else{
                brush.setColorAt(0.1f, Qt::white ); //playing beat highlight colors
                brush.setColorAt(0.8f, Qt::lightGray);
            }
            pen.setColor(Qt::darkGray);
            pen.setStyle(Qt::SolidLine);
        }
        else{
            if((i + offset) % beatsPerAccent == 0 && (i+offset) > currentBeat){
                if(isShowingAccents()){
                    brush.setColorAt(FIRST_COLOR_POSITION, Qt::lightGray); //accent marks
                    brush.setColorAt(SECOND_COLOR_POSITION, Qt::gray);

                    pen.setColor(Qt::gray);
                    pen.setStyle(Qt::SolidLine);
                }
            }
            else{
                if ( (i + offset) <= currentBeat) {
                    if(i + offset < currentBeat){
                        brush.setColorAt(FIRST_COLOR_POSITION, PLAYED_BEATS_FIRST_COLOR); //played beats
                        brush.setColorAt(SECOND_COLOR_POSITION, PLAYED_BEATS_SECOND_COLOR);
                    }
                    else{//the current beat is highlighted
                        brush.setColorAt(0.1f, Qt::white ); //playing beat highlight colors
                        brush.setColorAt(0.8f, Qt::darkGray);

                        pen.setColor(Qt::darkGray);
                        pen.setStyle(Qt::SolidLine);
                    }
                }
                else{
                    //non played yet
                    brush.setColorAt(0.0f, Qt::darkGray);
                    brush.setColorAt(0.8f, Qt::gray);
                }
            }
        }

        p.setBrush(brush);
        p.setPen( pen );
        int size = (i + offset) == currentBeat ? ovalSize + 1 : ovalSize;
        p.drawEllipse( QPoint(x, y), size, size);

        if(offset > 0 && currentBeat < offset){//is drawing the first circles?
            p.setBrush(QColor(255,255, 255, 200)); //the internal circles are drawed transparent
            p.drawEllipse( QPoint(x, y), size, size);
        }

        angle -= 2 * -PI / beatCircles;

    }
}

//+++++++++++++++++++++++++ LINEAR PAINTING +++++++++++
void IntervalProgressDisplay::drawPoint(int x, int y, int size, QPainter* painter, int value, QBrush bgPaint, QColor border, bool small) {
    painter->setBrush(bgPaint);
    painter->drawEllipse( QPoint(x, y), size/2, size/2);

    painter->setPen(QPen(border));
    painter->drawEllipse(QPoint(x, y), size/2, size/2);

    QString valueString = QString::number(value);

    painter->setFont(small ? SMALL_FONT : BIG_FONT);
    int valueStringWidth = painter->fontMetrics().width(valueString);
    //painter->setPen(QPen(textPaint));
    int textX = (int)(x - valueStringWidth / 2.0) -1;
    int textY = y + painter->fontMetrics().descent() + 2;
    if(!small){
        painter->setPen(Qt::black);
    }
    painter->drawText(textX, textY, valueString );
    //painter->drawText(QRect(textX, textY, size, size), Qt::AlignCenter, valueString );
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


float IntervalProgressDisplay::getHorizontalSpace(int totalPoinstToDraw, int initialXPos) const {
    return (float) (width() - initialXPos - LINEAR_PAINT_MODE_OVAL_SIZE/2) / (totalPoinstToDraw - 1);
}


void IntervalProgressDisplay::drawPoints(QPainter* painter, int yPos, int startPoint, int totalPoinstToDraw) {
    int initialXPos = 0 + LINEAR_PAINT_MODE_OVAL_SIZE / 2 + 1;
    float xSpace = getHorizontalSpace(totalPoinstToDraw, initialXPos+1);

    //draw the background line
    painter->setPen(QPen(LINEAR_BORDER_COLOR,1));
    painter->drawLine(initialXPos + 1, yPos, (int) (initialXPos + (totalPoinstToDraw - 1) * xSpace), yPos);

    float xPos = initialXPos;
    //draw all backgrounds first
    for (int i = startPoint; i < totalPoinstToDraw; i++) {
        bool canDraw = xSpace >= LINEAR_PAINT_MODE_OVAL_SIZE || (i % 2 == 0);
        if (canDraw) {
            QColor border = LINEAR_BORDER_COLOR;// (isShowingAccents() && isMeasureFirstBeat) ? accentBorderColor : borderPaint;
            int size = (int) (LINEAR_PAINT_MODE_OVAL_SIZE * 0.7f);// (i < currentInterval) ? ((int) (OVAL_SIZE * 0.7)) : ((int) (OVAL_SIZE * 0.85));
            QColor bg = (i < currentBeat) ? LINEAR_BG_COLOR : LINEAR_BG_SECOND_COLOR;
            drawPoint((int) xPos, yPos, size, painter, (i + 1), bg, border, true);
        }
        xPos += xSpace;
    }

    //draw accents
    if (isShowingAccents()) {
        xPos = initialXPos;
        int size = (int)(LINEAR_PAINT_MODE_OVAL_SIZE * 0.9f);
        for (int i = 0; i < totalPoinstToDraw; i += beatsPerAccent) {
            QColor bg = (i < currentBeat) ? LINEAR_BG_COLOR : LINEAR_BG_SECOND_COLOR;
            drawPoint((int) xPos, yPos, size, painter, (i + 1), bg, LINEAR_ACCENT_BORDER_COLOR, true);
            xPos += xSpace * beatsPerAccent;
        }
    }
    //draw current beat
    xPos = initialXPos + (currentBeat * xSpace);
    QRadialGradient bgRadial(xPos-5, yPos-5, LINEAR_PAINT_MODE_OVAL_SIZE*2);//, new float[]{0.1f, 0.8f},
    bgRadial.setColorAt(0.1f, Qt::white);
    bgRadial.setColorAt(0.8f, Qt::black);
    drawPoint((int) xPos, yPos, LINEAR_PAINT_MODE_OVAL_SIZE, painter, (currentBeat + 1), bgRadial, Qt::darkGray, false);


}
//+++++++++++++++++++++++++++++++++
void IntervalProgressDisplay::initialize() {
    horizontalRadius = width()/2 - ovalSize*2;
    verticalRadius = height()/2 - ovalSize*2;
    centerX = width() / 2;
    centerY = height() / 2;
}

//++++++++++=
QSize IntervalProgressDisplay::minimumSizeHint() const{
    QSize size = QWidget::minimumSizeHint();
    switch(paintMode){
    case CIRCULAR:
        size.setWidth(height());
        break;
    case ELLIPTICAL:
        size.setWidth(height() * 2);
        break;
    case LINEAR:
        size.setWidth(height() * 5);
        break;
    }
    return size;
}

