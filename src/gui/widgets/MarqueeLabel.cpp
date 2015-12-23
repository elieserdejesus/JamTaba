#include "MarqueeLabel.h"

#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QStyleOption>
#include "MarqueeHelper.h"

class MarqueeLabel::MarqueeLabelHelper : public MarqueeHelper{
public:
    MarqueeLabelHelper(QWidget* widget)
        :MarqueeHelper(widget){

    }
protected:
    QString getTextFromWidget() const{
        return dynamic_cast<QLabel*>(widget)->text();
    }

    void setWidgetTextAlignment(Qt::Alignment alignment){
        dynamic_cast<QLabel*>(widget)->setAlignment(alignment);
    }
};

//+++++++++++++++++++++++++++++++++++++++++++++

MarqueeLabel::MarqueeLabel(QWidget *parent)
    :QLabel(parent), helper(new MarqueeLabelHelper(this) )
{
    setAttribute(Qt::WA_NoBackground);
}

void MarqueeLabel::setText(const QString & text){
    QLabel::setText(text);
    helper->reset();
}

void MarqueeLabel::setFont(const QFont &font){
    QLabel::setFont(font);
    helper->reset();
}


void MarqueeLabel::updateMarquee(){
    helper->updateMarquee();
}

void MarqueeLabel::paintEvent(QPaintEvent *evt)
{
    QPainter p(this);

    //qt code to allow stylesheet
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    //if(!helper->isAnimating()){
        QLabel::paintEvent(evt);
//    }
//    else{
//        //animate the text
//        p.setFont(font());
//        p.setBrush(palette().text());//use the color defined in css file
//        p.drawText(helper->getTextX(), helper->getTextY(), text());
//    }
}
