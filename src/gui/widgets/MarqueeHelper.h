#ifndef MARQUEEHELPER_H
#define MARQUEEHELPER_H

#include <QWidget>

class MarqueeHelper : public QObject
{
    Q_OBJECT
public:
    MarqueeHelper(QWidget* widget);

    int getTextX() const{ return (int)textX; }
    int getTextY() const{ return (int)textY; }
    bool isAnimating() const { return animating; }
    void updateMarquee();
    void reset();

protected:
    QWidget* widget;

    bool eventFilter(QObject *, QEvent *);

    virtual QString getTextFromWidget() const = 0;
    virtual void setWidgetTextAlignment(Qt::Alignment) = 0;

    virtual bool needAnimation() const;

private:
    float textX;
    float textY;
    int textLength;
    bool animating;

    static const int MARQUEE_SPEED;
    static const int TIME_BETWEEN_ANIMATIONS;

    quint64 lastUpdate;
    quint64 timeSinceLastAnimation;

    void updateTextY();
    void updateTextLenght();
    void updateWidgetTextAlignment();

};

#endif // MARQUEEHELPER_H
