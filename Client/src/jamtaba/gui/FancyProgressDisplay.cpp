#include "FancyProgressDisplay.h"
#include <QPainter>
#include <QRadialGradient>
#include <QDebug>

const QColor FancyProgressDisplay::sliceNumberColor = QColor(120, 120, 120);
const QColor FancyProgressDisplay::bgPaintColor = QColor(230, 230, 230);
const QColor FancyProgressDisplay::bgPaintSecondColor = QColor(215, 215, 215);
const QColor FancyProgressDisplay::borderPaint = QColor(200, 200, 200);
const QColor FancyProgressDisplay::textPaint = QColor(180, 180, 180);// Color.LIGHT_GRAY;
const QColor FancyProgressDisplay::accentBorderColor = QColor(160, 160, 160);

QColor DARK_GRAY(70, 70, 70);
QColor BLACK(0, 0, 0);
QColor WHITE(255, 255, 255);

FancyProgressDisplay::FancyProgressDisplay(QWidget* parent)
    :QWidget(parent), fontMetrics(QFontMetrics(font()))
{
    setBeatsPerInterval(16);
    this->showAccents = false;
    this->currentBeat = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FancyProgressDisplay::setShowAccents(bool showAccents) {
    this->showAccents = showAccents;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FancyProgressDisplay::setCurrentBeat(int beat) {
    if (beat != currentBeat) {
        currentBeat = beat % beatsPerInterval;
        update();
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FancyProgressDisplay::setBeatsPerAccent(int beats) {
    beatsPerAccent = beats;
    update();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FancyProgressDisplay::setBeatsPerInterval(int beats) {
    if (beats > 0 && beats <= 64 && beats != beatsPerInterval) {
        this->beatsPerInterval = beats;
        if (beatsPerAccent <= 0) {
            beatsPerAccent = beats / 2;
        }
        update();
        //qWarning() << "Setando beatsPerInterval: " << beatsPerInterval << endl;
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FancyProgressDisplay::paintEvent(QPaintEvent * /*e*/){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //painter.setBrush(QBrush(Qt::black));
    //painter.drawRect(QRect(0, 0, width(), height() ));
    drawPoints(&painter, height() / 2, 0, this->beatsPerInterval);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FancyProgressDisplay::drawPoint(int x, int y, int size, QPainter* painter, int value,QBrush bgPaint, QColor border) {
    painter->setBrush(bgPaint);
    painter->drawEllipse( QPoint(x, y), size/2, size/2);

    painter->setPen(QPen(border));
    painter->drawEllipse(QPoint(x, y), size/2, size/2);

    QString valueString = QString::number(value);
    int valueStringWidth = fontMetrics.width(valueString);
    //painter->setPen(QPen(textPaint));
    int textX = (int)(x - valueStringWidth / 2.0) -1;
    int textY = y + fontMetrics.descent() + 2;
    painter->drawText(textX, textY, valueString );
    //painter->drawText(QRect(textX, textY, size, size), Qt::AlignCenter, valueString );
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


float FancyProgressDisplay::getHorizontalSpace(int totalPoinstToDraw, int initialXPos) const {
    return (float) (width() - initialXPos - OVAL_SIZE/2) / (totalPoinstToDraw - 1);
}


void FancyProgressDisplay::drawPoints(QPainter* g, int yPos, int startPoint, int totalPoinstToDraw) {

    int initialXPos = 0 + OVAL_SIZE / 2 + 1;
    float xSpace = getHorizontalSpace(totalPoinstToDraw, initialXPos+1);

    //draw the background line
    g->setPen(QPen(borderPaint));
    g->drawLine(initialXPos + 1, yPos, (int) (initialXPos + (totalPoinstToDraw - 1) * xSpace), yPos);

    //draw the current line
    //g.setPaint(Color.GRAY.brighter());
    //g.drawLine(initialXPos + 1, yPos+1, (int)(initialXPos + currentInterval * xSpace), yPos+1);
    float xPos = initialXPos;
    //draw all backgrounds first
    for (int i = startPoint; i < totalPoinstToDraw; i++) {
        //boolean isMeasureFirstBeat = ((i + startPoint) % beatsPerAccent) == 0;
        //bool isLastPoint = i == totalPoinstToDraw - 1;
        bool canDraw = xSpace >= OVAL_SIZE || (i % 2 == 0);
        if (canDraw) {
            QColor border = borderPaint;// (isShowingAccents() && isMeasureFirstBeat) ? accentBorderColor : borderPaint;
            int size = (int) (OVAL_SIZE * 0.7f);// (i < currentInterval) ? ((int) (OVAL_SIZE * 0.7)) : ((int) (OVAL_SIZE * 0.85));
            //if (isMeasureFirstBeat) {
            //    size += OVAL_SIZE * 0.15;
            //}
            QColor bg = (i < currentBeat) ? bgPaintColor : bgPaintSecondColor;
            drawPoint((int) xPos, yPos, size, g, (i + 1), bg, border);
        }
        xPos += xSpace;
    }

    //draw accents
    if (isShowingAccents()) {
        xPos = initialXPos;
        int size = (int)(OVAL_SIZE * 0.9f);
        for (int i = 0; i < totalPoinstToDraw; i += beatsPerAccent) {
            //bool isLastPoint = i == totalPoinstToDraw - 1;
            QColor bg = (i < currentBeat) ? bgPaintColor : bgPaintSecondColor;
            drawPoint((int) xPos, yPos, size, g, (i + 1), bg, accentBorderColor);
            xPos += xSpace * beatsPerAccent;
        }
    }
    //draw current beat
    xPos = initialXPos + (currentBeat * xSpace);
    //bool isLast = currentInterval == totalPoinstToDraw - 1;
    //(float cx, float cy, float radius, float[] fractions, Color[] colors)
    QRadialGradient bgRadial(xPos-5, yPos-5, OVAL_SIZE*2);//, new float[]{0.1f, 0.8f},
                        //new Color[]{Color.WHITE, Color.BLACK});
    bgRadial.setColorAt(0.1f, WHITE);
    bgRadial.setColorAt(0.8f, BLACK);
    drawPoint((int) xPos, yPos, OVAL_SIZE, g, (currentBeat + 1), bgRadial, DARK_GRAY);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
