#include "IntervalProgressDisplay.h"
#include <QPen>
#include <QPainter>
#include <QFont>
#include <QDebug>

const QColor IntervalProgressDisplay::LinearPaintStrategy::CIRCLES_BORDER_COLOR = QColor(0, 0, 0, 30);

IntervalProgressDisplay::LinearPaintStrategy::LinearPaintStrategy()
{
}

void IntervalProgressDisplay::LinearPaintStrategy::paint(QPainter &p, const PaintContext &context, const PaintColors &colors)
{

    font.setPointSizeF(context.fontSize);

    qreal initialXPos = context.elementsSize / 2.0 + 1;
    qreal xSpace = getHorizontalSpace(context.width, context.elementsSize, context.beatsPerInterval, initialXPos+1);
    qreal yPos = context.height/2.0;

    // draw the background line
    p.setPen(QPen(CIRCLES_BORDER_COLOR, 0.9f));
    p.drawLine(initialXPos + (xSpace * context.currentBeat), yPos, initialXPos + (context.beatsPerInterval - 1) * xSpace, yPos);

    qreal xPos = initialXPos;
    // draw all backgrounds first
    int size = (int)(context.elementsSize * 0.5f);
    for (int i = 0; i < context.beatsPerInterval; i++) {
        bool canDraw = i >= context.currentBeat && (xSpace >= (context.elementsSize * 0.5) || (i % 2 == 0));
        if (canDraw) {
            drawPoint(xPos, yPos, size, p, (i + 1), colors.secondaryBeat, CIRCLES_BORDER_COLOR, true);
        }
        xPos += xSpace;
    }

    // draw accents
    if (context.showingAccents) {
        xPos = initialXPos;
        qreal size = context.elementsSize * 0.6f;
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
    drawPoint(xPos, yPos, context.elementsSize, p, (context.currentBeat + 1), bgPaint, Qt::darkGray, false);
}

qreal IntervalProgressDisplay::LinearPaintStrategy::getHorizontalSpace(int width, qreal elementsSize, int totalPoinstToDraw, int initialXPos) const
{
    return (qreal)(width - initialXPos - elementsSize/2) / (totalPoinstToDraw - 1);
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
        painter.setFont(font);
        QFontMetrics fontMetrics = painter.fontMetrics();
        QRect boundingRect = fontMetrics.tightBoundingRect(valueString);
        qreal textX = x - fontMetrics.width(valueString)/2.0;
        qreal textY = y + boundingRect.height()/2.0;
        painter.setPen(Qt::black);
        painter.drawText(QPointF(textX, textY), valueString);
    }
}
