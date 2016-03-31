#include "IntervalProgressDisplay.h"
#include <QPen>
#include <QPainter>
#include <QFont>
#include <QDebug>

const qreal IntervalProgressDisplay::LinearPaintStrategy::CIRCLES_SIZE = 25;
const QColor IntervalProgressDisplay::LinearPaintStrategy::CIRCLES_BORDER_COLOR = QColor(200, 200, 200);

IntervalProgressDisplay::LinearPaintStrategy::LinearPaintStrategy()
    :FONT("Verdana, 10")
{
}

void IntervalProgressDisplay::LinearPaintStrategy::paint(QPainter &p, const PaintContext &context, const PaintColors &colors)
{
    qreal initialXPos = 0 + CIRCLES_SIZE / 2 + 1;
    qreal xSpace = getHorizontalSpace(context.width, context.beatsPerInterval, initialXPos+1);
    qreal yPos = context.height/2;

    // draw the background line
    p.setPen(QPen(CIRCLES_BORDER_COLOR, 0.9f));
    p.drawLine(initialXPos + (xSpace * context.currentBeat), yPos, initialXPos + (context.beatsPerInterval - 1) * xSpace, yPos);

    qreal xPos = initialXPos;
    // draw all backgrounds first
    int size = (int)(CIRCLES_SIZE * 0.5f);
    for (int i = 0; i < context.beatsPerInterval; i++) {
        bool canDraw = i >= context.currentBeat
                       && (xSpace >= (CIRCLES_SIZE * 0.5) || (i % 2 == 0));
        if (canDraw) {

            drawPoint(xPos, yPos, size, p, (i + 1), colors.secondaryBeat, CIRCLES_BORDER_COLOR, true);
        }
        xPos += xSpace;
    }

    // draw accents
    if (context.showingAccents) {
        xPos = initialXPos;
        qreal size = CIRCLES_SIZE * 0.6f;
        for (int i = 0; i < context.beatsPerInterval; i += context.beatsPerAccent) {
            if (i > context.currentBeat) {
                drawPoint(xPos, yPos, size, p, (i + 1), colors.accentBeat, CIRCLES_BORDER_COLOR, true);
            }
            xPos += xSpace * context.beatsPerAccent;
        }
    }
    // draw current beat
    xPos = initialXPos + (context.currentBeat * xSpace);
    QBrush bgPaint(colors.currentBeat);
    bool isMeasureFirstBeat = context.currentBeat % context.beatsPerAccent == 0;
    bool isIntervalFirstBeat = context.currentBeat == 0;
    if (isIntervalFirstBeat || (context.showingAccents && isMeasureFirstBeat )) {
        bgPaint = QColor(Qt::green);
    }
    drawPoint(xPos, yPos, CIRCLES_SIZE, p, (context.currentBeat + 1), bgPaint, Qt::darkGray, false);
}

qreal IntervalProgressDisplay::LinearPaintStrategy::getHorizontalSpace(int width, int totalPoinstToDraw, int initialXPos) const
{
    return (float)(width - initialXPos - CIRCLES_SIZE/2) / (totalPoinstToDraw - 1);
}

void IntervalProgressDisplay::LinearPaintStrategy::drawPoint(qreal x, qreal y, qreal size, QPainter &painter, int value,
                                        const QBrush &bgPaint, const QColor &border, bool small)
{
    painter.setBrush(bgPaint);
    painter.drawEllipse(QPointF(x, y), size/2, size/2);

    painter.setPen(QPen(border));
    painter.drawEllipse(QPointF(x, y), size/2, size/2);

    bool drawText = !small;
    if (drawText) {
        QString valueString = QString::number(value);
        painter.setFont(FONT);
        qreal valueStringWidth = painter.fontMetrics().width(valueString);
        qreal textX = (x - valueStringWidth / 2.0);
        qreal textY = y + painter.fontMetrics().descent() + 2;
        if (!small)
            painter.setPen(Qt::black);
        painter.drawText(QPointF(textX, textY), valueString);
    }
}
