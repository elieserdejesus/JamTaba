#include "IntervalProgressDisplay.h"
#include <QPen>
#include <QPainter>

QRectF IntervalProgressDisplay::EllipticalPaintStrategy::createContextRect(const PaintContext &context) const
{
    qreal margin = CIRCLE_MAX_SIZE + 4;
    qreal left = margin;
    qreal top = margin;
    qreal w = qMin(context.height * 3, context.width) - margin * 2;
    qreal h = context.height - margin * 2;
    return QRectF(left, top, w, h);
}

void IntervalProgressDisplay::EllipticalPaintStrategy::paint(QPainter &p, const PaintContext &context, const PaintColors &colors)
{
    QRectF rect = createContextRect(context);

    if (context.beatsPerInterval <= 32) {
        drawCircles(p, rect, context, colors, context.beatsPerInterval, 0, true);// draw a ellipse
    } else {
        int elementsInOuterEllipse = context.beatsPerInterval / 2;
        int elementsInNestedEllipse = context.beatsPerInterval - elementsInOuterEllipse;

        //draw the internal circles first, so external circles will appear in top
        int margin = CIRCLE_MAX_SIZE * 2;
        QMargins margins(margin, margin, margin, margin);
        bool drawPath = context.currentBeat >= elementsInOuterEllipse;
        drawCircles(p, rect.marginsRemoved(margins), context, colors, elementsInNestedEllipse, elementsInOuterEllipse, drawPath);

        //draw external circles
        if (context.currentBeat < elementsInOuterEllipse){ //only draw the external circles when playing the first part of interval
            drawCircles(p, rect, context, colors, elementsInOuterEllipse, 0, true);
        }
    }

    // draw current beat text in center
    p.setPen(colors.textColor);
    QString numberString(QString::number(context.currentBeat + 1) + " / " + QString::number(context.beatsPerInterval));

    int strWidth =  p.fontMetrics().width(numberString);
    p.drawText(rect.center().x() - strWidth / 2, rect.center().y() + p.fontMetrics().height()/2, numberString);
}

void IntervalProgressDisplay::EllipticalPaintStrategy::paintEllipticalPath(QPainter &p, const QRectF &rect, const PaintColors &colors, int currentBeat, int beatsToDraw)
{
    QPen pen(Qt::SolidLine);
    pen.setColor(colors.secondaryBeat);
    p.setPen(pen);
    p.setBrush(Qt::BrushStyle::NoBrush);
    //p.drawEllipse(rect);

    double degreesPerSlice = 360 / beatsToDraw;
    int beatIndex = currentBeat < beatsToDraw ? currentBeat : (currentBeat - (beatsToDraw - 1));
    int startAngle = -(degreesPerSlice * beatIndex - 90) * 16;
    int spanAngle =  -(degreesPerSlice * (beatsToDraw - beatIndex)) * 16;
    p.drawArc(rect, startAngle, spanAngle);
}

QBrush IntervalProgressDisplay::EllipticalPaintStrategy::getBrush(int beat, int beatOffset, const PaintContext &context, const PaintColors &colors) const
{
    bool isIntervalFirstBeat = beat + beatOffset == 0;
    bool isMeasureFirstBeat = (beat + beatOffset) % context.beatsPerAccent == 0;
    bool isCurrentBeat = beat + beatOffset == context.currentBeat;

    if (isCurrentBeat)
    {
        if (isIntervalFirstBeat || (context.showingAccents && isMeasureFirstBeat))
            return QBrush(colors.currentAccentBeat);//green
        else
            return QBrush(colors.currentBeat);//white
    }
    else{
        if(beatOffset > 0 && context.currentBeat < beatOffset){//drawing internal circles when playing external circle beats?
            return QBrush(colors.disabledBeats);//transparent gray
        }

        if (context.showingAccents && isMeasureFirstBeat) {
            return QBrush(colors.accentBeat);
        }
    }
    //not the current beat
    return QBrush(colors.secondaryBeat);
}

qreal IntervalProgressDisplay::EllipticalPaintStrategy::getCircleSize(int beat, int beatOffset, const PaintContext &context) const
{
    bool isCurrentBeat = beat + beatOffset == context.currentBeat;
    if (isCurrentBeat)
    {
        return CIRCLE_MAX_SIZE;
    }
    else{
        bool isMeasureFirstBeat = (beat + beatOffset) % context.beatsPerAccent == 0;
        if (context.showingAccents && isMeasureFirstBeat){
            return CIRCLE_MAX_SIZE - 1;
        }
    }
    return CIRCLE_MAX_SIZE - 2;
}

QPen IntervalProgressDisplay::EllipticalPaintStrategy::getPen(int beat, int beatOffset, const PaintContext &context) const
{
    bool isIntervalFirstBeat = beat + beatOffset == 0;
    bool isMeasureFirstBeat = (beat + beatOffset) % context.beatsPerAccent == 0;
    bool isCurrentBeat = beat + beatOffset == context.currentBeat;

    if (isCurrentBeat) {
        if (isIntervalFirstBeat || (context.showingAccents && isMeasureFirstBeat))
            return QPen(Qt::darkGreen, Qt::SolidLine); //accent and first beat
        else
            return QPen(Qt::darkGray, Qt::SolidLine);//highlight the current, but non accented beat
    }
    else{//not current beat
        if (beatOffset > 0 && context.currentBeat < beatOffset){//drawing internal circles when playing external circle beats?
            return QPen(Qt::NoPen);
        }

        if (context.showingAccents && isMeasureFirstBeat) { //mark the accents
            return QPen(Qt::darkGreen, Qt::SolidLine);
        }
    }
    return QPen(Qt::darkGray, Qt::SolidLine);
}

void IntervalProgressDisplay::EllipticalPaintStrategy::drawCircles(QPainter &p, const QRectF &rect, const PaintContext &context, const PaintColors &colors, int beatsToDraw, int beatsToDrawOffset, bool drawPath)
{
    if (drawPath) {
        paintEllipticalPath(p, rect, colors, context.currentBeat, beatsToDraw);
    }

    //p.drawRect(rect);

    double angle = -PI / 2.0;
    qreal hRadius = rect.width()/2;
    qreal vRadius = rect.height()/2;
    qreal centerX = rect.center().x();
    qreal centerY = rect.center().y();

    //p.drawLine(centerX, 0, centerX, height());
    //p.drawLine(0, centerY, width(), centerY);

    for (int beat = beatsToDraw; beat >= 0; --beat) {
        qreal x = centerX + ((hRadius) * std::cos(angle));
        qreal y = centerY + ((vRadius) * std::sin(angle));

        QPen pen = getPen(beat, beatsToDrawOffset, context);
        QBrush brush = getBrush(beat, beatsToDrawOffset, context, colors);

        // draw the current beat
        if (beat + beatsToDrawOffset >= context.currentBeat) {
            p.setBrush(brush);
            p.setPen(pen);
            qreal size = getCircleSize(beat, beatsToDrawOffset, context);
            p.drawEllipse(QPointF(x, y), size, size);
        }

        angle += 2 * -PI / beatsToDraw;
    }
}
