#include "MarqueeHelper.h"

#include <QEvent>
#include <QDateTime>

const int MarqueeHelper::MARQUEE_SPEED = 15;//pixels per second
const int MarqueeHelper::TIME_BETWEEN_ANIMATIONS = 8000;

MarqueeHelper::MarqueeHelper(QWidget *widget)
    :widget(widget), textX(0), textY(0), textLength(0),
            lastUpdate(0), animating(false), timeSinceLastAnimation(0)
{
    widget->installEventFilter(this);
}

bool MarqueeHelper::eventFilter(QObject *source, QEvent *ev){
    if(ev->type() == QEvent::Resize || ev->type() == QEvent::FocusOut){
        reset();
    }
    return QObject::eventFilter(source, ev);
}

void MarqueeHelper::updateWidgetTextAlignment(){
    setWidgetTextAlignment( textLength > widget->width() ? Qt::AlignLeft : Qt::AlignCenter);
}

void MarqueeHelper::updateMarquee(){

    if(!needAnimation()){
        return;
    }

    quint64 timeEllapsed = QDateTime::currentMSecsSinceEpoch() - lastUpdate;

    if(!animating){
        timeSinceLastAnimation += timeEllapsed;

        if(timeSinceLastAnimation >= TIME_BETWEEN_ANIMATIONS){
            animating = true;
        }
    }
    else{//animating
        int oldTextX = (int)textX;
        textX -= timeEllapsed/1000.0 * MARQUEE_SPEED;

        if( (int)textX != oldTextX){//avoid repaint the widget if the text position don't change
            if(textX < (widget->width() - textLength) * 2){
                textX = 0;
                animating = false;
                timeSinceLastAnimation = 0;
            }
            widget->update();
        }
    }

    lastUpdate = QDateTime::currentMSecsSinceEpoch();
}

bool MarqueeHelper::needAnimation() const{
    return textLength > widget->width();
}

void MarqueeHelper::reset(){
    animating = false;
    timeSinceLastAnimation = 0;
    lastUpdate = 0;

    textY = widget->height() -  (widget->height() - widget->fontInfo().pointSize())/2;

    QString text = getTextFromWidget();
    textLength = widget->fontMetrics().boundingRect(text).width();

    updateWidgetTextAlignment();
}
