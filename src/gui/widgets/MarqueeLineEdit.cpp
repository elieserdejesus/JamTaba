#include "MarqueeLineEdit.h"
#include "MarqueeHelper.h"
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QApplication>

class MarqueeLineEdit::MarqueeLineEditHelper : public MarqueeHelper{
public:
    MarqueeLineEditHelper(QLineEdit* widget)
        :MarqueeHelper(widget){

    }
protected:
    QString getTextFromWidget() const{
        return dynamic_cast<QLineEdit*>(widget)->text();
    }

    void setWidgetTextAlignment(Qt::Alignment alignment){
        dynamic_cast<QLineEdit*>(widget)->setAlignment(alignment);
    }

    bool needAnimation() const{
        //don't animate if the QLineEdit is editable (local input intracks names)
        return MarqueeHelper::needAnimation();// && !widget->isEnabled();
    }
};

//+++++++++++++++++++++++++
MarqueeLineEdit::MarqueeLineEdit(QWidget *parent)
    :QLineEdit(parent), helper(new MarqueeLineEditHelper(this)){

    setAlignment(Qt::AlignLeft);
}

void MarqueeLineEdit::setText(const QString &text){
    QLineEdit::setText(text);
    helper->reset();
}

void MarqueeLineEdit::setFont(const QFont &font){
    QLineEdit::setFont(font);
    helper->reset();
}

void MarqueeLineEdit::updateMarquee(){
    helper->updateMarquee();
}

void MarqueeLineEdit::focusInEvent(QFocusEvent *){
    helper->reset();
    update();
}

void MarqueeLineEdit::paintEvent(QPaintEvent *evt)
{
    //qt code to allow stylesheet
    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    if(!helper->isAnimating()){
        QLineEdit::paintEvent(evt);
    }
    else{


        //animate the text
        p.setFont(font());
        p.setBrush(palette().text());//use the color defined in css file
        p.drawText(helper->getTextX(), helper->getTextY(), text());
    }
}

