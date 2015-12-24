#include "MarqueeLabel.h"

#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QStyleOption>
#include <QDateTime>
#include <QtMath>

const int MarqueeLabel::MARQUEE_SPEED = 20;//pixels per second
const int MarqueeLabel::TIME_BETWEEN_ANIMATIONS = 10000;

//+++++++++++++++++++++++++++++++++++++++++++++

MarqueeLabel::MarqueeLabel(QWidget *parent)
    :QLabel(parent), textX(0), textY(0), textLength(0),
      lastUpdate(0), animating(false), timeSinceLastAnimation(0), speedDecay(1)
{
    setAttribute(Qt::WA_NoBackground);
    lastUpdate = QDateTime::currentMSecsSinceEpoch();//avoid start a animation when the widget is created
}

void MarqueeLabel::setText(const QString & newText){
    //using the elided text when 'newText' is big
    QLabel::setText(fontMetrics().elidedText(newText, Qt::ElideRight, width()));
    originalText = newText;
    reset();
}

void MarqueeLabel::resetAnimationProperties(){
    animating = false;
    timeSinceLastAnimation = 0;
    lastUpdate = QDateTime::currentMSecsSinceEpoch();
    speedDecay = 1;
    textX = needAnimation() ? 0 : width()/2 -  fontMetrics().width(text())/2; //draw text in center if the text is small
}

void MarqueeLabel::reset(){
    resetAnimationProperties();
    textY = height() -  (height() - fontInfo().pointSize())/2;
    textLength = fontMetrics().width(originalText + " ");//adding one blank space to easy read the text in the animation end
}

void MarqueeLabel::setFont(const QFont &font){
    QLabel::setFont(font);
    reset();
}

bool MarqueeLabel::needAnimation() const{
    return textLength > width();
}

void MarqueeLabel::updateMarquee(){
    if(!needAnimation()){
        return;
    }
    quint64 timeEllapsed = QDateTime::currentMSecsSinceEpoch() - lastUpdate;
    if(!animating){
        timeSinceLastAnimation += timeEllapsed;
        if(timeSinceLastAnimation >= TIME_BETWEEN_ANIMATIONS){//time to start a new marquee animation?
            reset();
            animating = true;
        }
    }
    else{//animating
        int oldTextX = qRound(textX);
        textX -= (timeEllapsed/1000.0f * speedDecay) * MARQUEE_SPEED;

        int totalShiftAmount = textLength - width();//how many pixels text is shifted?
        if(totalShiftAmount > 0){//avoid zero as divider
            float currentShift = qAbs(textX)/totalShiftAmount;
            speedDecay = 1.0f - qPow(currentShift, 3);//exponentially slowing down the animation in the end
            if(speedDecay <= 0.005f){//avoid endless animation when the speedDecay value is too small
                textX -= 1.0f;//forcing the animation end
            }
        }
        else{
            speedDecay = 1.0f;
        }

        if( qRound(textX) != oldTextX){//avoid repaint the widget if the text position don't change
            if(textX + totalShiftAmount <= 0){//end of animation?
                resetAnimationProperties();
            }
            update();
        }
    }

    lastUpdate = QDateTime::currentMSecsSinceEpoch();
}

void MarqueeLabel::resizeEvent(QResizeEvent *ev){
    Q_UNUSED(ev);
    reset();
    setText(originalText);//maybe the new size need elided text
}


void MarqueeLabel::paintEvent(QPaintEvent *evt)
{
    Q_UNUSED(evt);
    QPainter p(this);

    //qt code to allow stylesheet
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setFont(font());
    p.setBrush(palette().text());//use the color defined in css file
    p.drawText( QPointF(textX, textY), animating ? originalText : text() );//text() can be elided
}
