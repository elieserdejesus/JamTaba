#include "IntervalProgressDisplay.h"

QRectF IntervalProgressDisplay::CircularPaintStrategy::createContextRect(const PaintContext &context, qreal margin) const
{
    qreal size = qMin(context.width, context.height) - margin * 2;
    qreal left = margin;
    qreal top = margin;
    if (context.height < context.width){
        left = (context.width - size)/2; //draw in center horizontally
    }
    else{
        top = (context.height - size)/2; //draw in center vertically
    }
    return QRectF(left, top, size, size);
}
