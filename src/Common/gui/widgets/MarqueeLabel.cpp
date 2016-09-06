#include "MarqueeLabel.h"

#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QDateTime>
#include <QtMath>

const int MarqueeLabel::MARQUEE_SPEED = 20;// pixels per second
int MarqueeLabel::TIME_BETWEEN_ANIMATIONS = 10000;

// +++++++++++++++++++++++++++++++++++++++++++++

MarqueeLabel::MarqueeLabel(QWidget *parent) :
    QLabel(parent),
    textX(0),
    textY(0),
    lastUpdate(0),
    animating(false),
    timeSinceLastAnimation(0),
    speedDecay(1)
{
    //setAttribute(Qt::WA_NoBackground);
    lastUpdate = QDateTime::currentMSecsSinceEpoch();// avoid start a animation when the widget is created
}

void MarqueeLabel::setTimeBetweenAnimations(quint32 newTime)
{
    MarqueeLabel::TIME_BETWEEN_ANIMATIONS = newTime;
}

void MarqueeLabel::resetAnimationProperties()
{
    animating = false;
    timeSinceLastAnimation = 0;
    lastUpdate = QDateTime::currentMSecsSinceEpoch();
    speedDecay = 1;
    textX = needAnimation() ? 0 : width()/2 -  fontMetrics().width(text())/2; // draw text in center if the text is small
}

void MarqueeLabel::reset()
{
    resetAnimationProperties();
    textY = height() -  (height() - fontInfo().pointSize())/2;
}

void MarqueeLabel::setFont(const QFont &font)
{
    QLabel::setFont(font);
    reset();
}

bool MarqueeLabel::needAnimation() const
{
    int textLength = getTextLenght();
    return textLength > width() && !hasSelectedText();
}

int MarqueeLabel::getTextLenght() const
{
    return fontMetrics().width(text() + " ");
}

void MarqueeLabel::updateMarquee()
{
    if (!needAnimation())
        return;

    quint64 timeEllapsed = QDateTime::currentMSecsSinceEpoch() - lastUpdate;
    if (!animating) {
        timeSinceLastAnimation += timeEllapsed;
        if (timeSinceLastAnimation >= TIME_BETWEEN_ANIMATIONS) {// time to start a new marquee animation?
            reset();
            animating = true;
        }
    } else {// animating
        int oldTextX = qRound(textX);
        textX -= (timeEllapsed/1000.0f * speedDecay) * MARQUEE_SPEED;

        int totalShiftAmount = getTextLenght() - width();// how many pixels text is shifted?
        if (totalShiftAmount > 0) {// avoid zero as divider
            float currentShift = qAbs(textX)/totalShiftAmount;
            speedDecay = 1.0f - qPow(currentShift, 3);// exponentially slowing down the animation in the end
            if (speedDecay <= 0.005f)// avoid endless animation when the speedDecay value is too small
                textX -= 1.0f;// forcing the animation end
        } else {
            speedDecay = 1.0f;
        }

        if (qRound(textX) != oldTextX) {// avoid repaint the widget if the text position don't change
            if (textX + totalShiftAmount <= 0)// end of animation?
                resetAnimationProperties();
            update();
        }
    }

    lastUpdate = QDateTime::currentMSecsSinceEpoch();
}

void MarqueeLabel::resizeEvent(QResizeEvent *ev)
{
    Q_UNUSED(ev);
    reset();
}

QRectF MarqueeLabel::getSelectedTextRect() const
{
    if (!hasSelectedText())
        return QRectF();

    QFontMetrics metrics = fontMetrics();
    int textWidth = metrics.width(selectedText());
    QString firstLetters = text().left(selectionStart());
    int startPosition = metrics.width(firstLetters);

    return QRectF(startPosition, textY - metrics.ascent(), textWidth, metrics.height());
}

void MarqueeLabel::mousePressEvent(QMouseEvent *evt)
{
    QLabel::mousePressEvent(evt);
    reset();
}

void MarqueeLabel::paintEvent(QPaintEvent *evt)
{
    QString elidedText = fontMetrics().elidedText(text(), Qt::ElideRight, width());
    bool needEliding = elidedText.size() != text().size();
    if (!animating && !needEliding) {
        QLabel::paintEvent(evt);
    }
    else {
        QPainter p(this);
        p.setFont(font());

        qreal x = textX;
        qreal y = textY;
        QRectF selectedRect = getSelectedTextRect();
        if (!animating) {//not animating and need eliding
            x = 0; // when eliding text is always left aligned

            if (hasSelectedText()) {
                p.fillRect(selectedRect, palette().highlight()); //draw the
            }
        }

        p.setPen(QPen(palette().text(), 1.0)); // using the color defined in css file
        p.drawText(QPointF(x, y), !animating ? elidedText : text()); //draw the entire text

        if (hasSelectedText()) { //draw the selected text with highlight color
            p.setPen(QPen(palette().highlightedText(), 1.0));
            p.drawText(QPointF(selectedRect.left(), textY), selectedText());
        }
    }
}

