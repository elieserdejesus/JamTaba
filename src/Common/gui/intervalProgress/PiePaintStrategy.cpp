#include "IntervalProgressDisplay.h"
#include <QPainterPath>
#include <QPainter>

void IntervalProgressDisplay::PiePaintStrategy::paint(QPainter &p, const PaintContext &context, const PaintColors &colors)
{
    qreal margin = 4;
    QRectF rect = createContextRect(context, margin);

    //draw current beat text
    drawCurrentBeatValue(p, rect, context, colors);

    if (context.beatsPerInterval <= 32) {
        qreal piesHeight = context.elementsSize;
        p.setClipPath(getClipPath(rect, piesHeight)); //define the clip region to get a hole in the center
        drawPies(p, rect, context, colors, context.beatsPerInterval, 0, piesHeight );
    }
    else {
        int outerPies = context.beatsPerInterval / 2;
        int innerPies = context.beatsPerInterval - outerPies;

        //draw the internal pies first, so external pies will appear in top
        qreal piesHeight = context.elementsSize;
        qreal margin = piesHeight + 2;//2 pixels as margin between inner and outer pies
        QMarginsF margins(margin, margin, margin, margin);
        QRectF innerRect = rect.marginsRemoved(margins);
        p.setClipPath(getClipPath(innerRect, piesHeight));
        drawPies(p, innerRect, context, colors, innerPies, outerPies, piesHeight );

        //draw external/outer pies
        if (context.currentBeat < outerPies){ //only draw the external pies when playing the first part of interval
            p.setClipPath(getClipPath(rect, piesHeight));
            drawPies(p, rect, context, colors, outerPies, 0, piesHeight);
        }
    }
}

QPainterPath IntervalProgressDisplay::PiePaintStrategy::getClipPath(const QRectF &rect, qreal piesHeight) const
{
    //create a rect with full size
    QPainterPath rectPath;
    rectPath.addRect(rect);

    //create a circle (ellipse with same width and height) and subtract from the above rect to create a hole in the rect when painting
    QPainterPath erasePath;
    qreal newHeight = piesHeight + 0.5;
    QMarginsF margins(newHeight, newHeight, newHeight, newHeight);
    erasePath.addEllipse(rect.marginsRemoved(margins));
    return rectPath.subtracted(erasePath);
}

void IntervalProgressDisplay::PiePaintStrategy::drawPies(QPainter &p, const QRectF &rect, const PaintContext &context, const PaintColors &colors, int beatsToDraw, int beatsToDrawOffset, qreal piesHeight)
{
    qreal angleIncrement = (2 * -PI) / beatsToDraw;
    double angle = -PI/2 + angleIncrement;
    QRectF smallRect = rect.marginsRemoved(QMarginsF(piesHeight, piesHeight, piesHeight, piesHeight));

    for (int beat = beatsToDraw-1; beat >= 0; --beat) {

        // drawing the current beat
        if (beat + beatsToDrawOffset >= context.currentBeat) {
            //QPen pen = getPen(beat, beatsToDrawOffset, context);
            QBrush brush = getBrush(beat, beatsToDrawOffset, context, colors);
            p.setBrush(brush);
            p.setPen(Qt::NoPen);

            double degreesPerSlice = 360.0 / beatsToDraw;
            qreal startAngle = -(degreesPerSlice * beat - 90) * 16;
            qreal spanAngle = -((degreesPerSlice-3) * 16);//using a gap with 3 degress between pies
            p.drawPie(rect, startAngle, spanAngle);
            p.drawArc(smallRect, startAngle, spanAngle);
        }
        angle += angleIncrement;
    }
}
