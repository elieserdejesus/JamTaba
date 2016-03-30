#include "IntervalProgressDisplay.h"

#include <QPaintEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QDebug>
#include <cmath>

QColor IntervalProgressDisplay::PLAYED_BEATS_FIRST_COLOR = QColor(0, 0, 0, 0);// played beats are transparent
QColor IntervalProgressDisplay::PLAYED_BEATS_SECOND_COLOR = QColor(0, 0, 0, 0);
QColor IntervalProgressDisplay::PATH_COLOR = QColor(0, 0, 0, 20);

const qreal IntervalProgressDisplay::MARGIN = 2;
const qreal IntervalProgressDisplay::PREFERRED_OVAL_SIZE = 7;

const double IntervalProgressDisplay::PI = 3.141592653589793238462643383279502884;

// LINEAR PAINTING CONSTANTS
const QColor IntervalProgressDisplay::LINEAR_SLICE_NUMBER_COLOR = QColor(120, 120, 120);
const QColor IntervalProgressDisplay::LINEAR_BG_COLOR = QColor(230, 230, 230);
const QColor IntervalProgressDisplay::LINEAR_BG_SECOND_COLOR = QColor(160, 160, 160);
const QColor IntervalProgressDisplay::LINEAR_BORDER_COLOR = QColor(200, 200, 200);
const QColor IntervalProgressDisplay::LINEAR_TEXT_COLOR = QColor(180, 180, 180);// Color.LIGHT_GRAY;
const QColor IntervalProgressDisplay::LINEAR_ACCENT_BORDER_COLOR = QColor(120, 120, 120);
const int IntervalProgressDisplay::LINEAR_PAINT_MODE_OVAL_SIZE = 25;

//Elliptical painting colors
const QColor IntervalProgressDisplay::BEAT_NUMBER_COLOR(Qt::gray);
const QColor IntervalProgressDisplay::TRANSPARENT_BRUSH_COLOR(0, 0, 0, 20);

// ++++++++++++++++++++++++++++++++++++++++++++++++++
IntervalProgressDisplay::IntervalProgressDisplay(QWidget *parent) :
    QFrame(parent),
    SMALL_FONT("Verdana, 7"),
    BIG_FONT("Verdana, 10"),
    ovalSize(PREFERRED_OVAL_SIZE),
    paintMode(PaintMode::LINEAR),
    showAccents(false),
    currentBeat(0),
    beatsPerAccent(0),
    usingLowContrastColors(false),
    highlightColor(Qt::white)
{
    //setAttribute(Qt::WA_NoBackground);

    setBeatsPerInterval(32);

    setShowAccents(false);
}

IntervalProgressDisplay::~IntervalProgressDisplay()
{
    // delete comboBoxPaintMode;
}

void IntervalProgressDisplay::setPaintUsingLowContrastColors(bool useLowContrastColors)
{
    usingLowContrastColors = useLowContrastColors;
    highlightColor = usingLowContrastColors ? Qt::lightGray : Qt::white;
    repaint();
}

void IntervalProgressDisplay::setShowAccents(bool showAccents)
{
    this->showAccents = showAccents;
    update();
}

void IntervalProgressDisplay::setCurrentBeat(int beat)
{
    if (beat != currentBeat) {
        currentBeat = beat % beats;
        update();
    }
}

void IntervalProgressDisplay::setBeatsPerAccent(int beats)
{
    beatsPerAccent = beats;
    if (!isShowingAccents() && beatsPerAccent > 1)
        this->showAccents = true;
    update();
}

void IntervalProgressDisplay::setBeatsPerInterval(int beats)
{
    if (beats > 0 && beats <= 64) {
        this->beats = beats;
        if (beatsPerAccent <= 0)
            beatsPerAccent = beats / 2;
        if (beats <= 16)
            ovalSize = PREFERRED_OVAL_SIZE;
        else
            ovalSize = PREFERRED_OVAL_SIZE - beats/64.0 * 2;
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void IntervalProgressDisplay::setPaintMode(PaintMode mode)
{
    if (mode != this->paintMode) {
        this->paintMode = mode;
        updateGeometry();
        repaint();
    }
}

// ++++++++++++++++++++++++++  ELLIPSE PAINTING ++++++++++++++++++++++++++++++++++++++
void IntervalProgressDisplay::paintEllipticalPath(QPainter &p, const QRectF &rect, int beatsToDraw)
{
    QPen pen(Qt::SolidLine);
    pen.setColor(PATH_COLOR);
    p.setPen(pen);
    p.setBrush(Qt::BrushStyle::NoBrush);
    //p.drawEllipse(rect);

    double degreesPerSlice = 360 / beatsToDraw;
    int beatIndex = currentBeat < beatsToDraw ? currentBeat : (currentBeat - (beatsToDraw - 1));
    int startAngle = -(degreesPerSlice * beatIndex - 90) * 16;
    int spanAngle =  -(degreesPerSlice * (beatsToDraw - beatIndex)) * 16;
    p.drawArc(rect, startAngle, spanAngle);
}

void IntervalProgressDisplay::paintElliptical(QPainter &p, const QColor &textColor, const QRectF &rect)
{
    if (beats <= 32) {
        drawBeatCircles(p, rect, beats, 0, true);// draw a ellipse
    } else {
        int elementsInOuterEllipse = beats / 2;
        int elementsInNestedEllipse = beats - elementsInOuterEllipse;
        if (currentBeat < elementsInOuterEllipse){ //only draw the external circles when playing the first part of interval
            drawBeatCircles(p, rect, elementsInOuterEllipse, 0, true);
        }

        int margim = rect.width() * 0.1;
        QMargins margins(margim, margim, margim, margim);
        bool drawPath = currentBeat >= elementsInOuterEllipse;
        drawBeatCircles(p, rect.marginsRemoved(margins), elementsInNestedEllipse, elementsInOuterEllipse, drawPath);
    }

    // draw current beat text in center
    p.setPen(textColor);
    QString numberString(QString::number(currentBeat + 1) + " / " + QString::number(beats));

    int strWidth = fontMetrics().width(numberString);
    p.drawText(rect.center().x() - strWidth / 2, rect.center().y() + fontMetrics().height()/2, numberString);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void IntervalProgressDisplay::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QColor textColor(Qt::black);

    switch (paintMode) {
    case ELLIPTICAL:
    {
        qreal margin = ovalSize + 4;
        qreal left = margin;
        qreal top = margin;
        qreal w = qMin(height() * 3, width()) - margin * 2;
        qreal h = height() - margin * 2;
        paintElliptical(p, textColor, QRectF(left, top, w, h));
    }
    break;
    case CIRCULAR:
    {
        qreal margin = ovalSize + 4;
        qreal size = qMin(width(), height()) - margin * 2;
        qreal left = margin;
        qreal top = 0;
        if (height() < width()){
            left += (width() - size)/2; //draw in center horizontally
        }
        else{
            top += (height() - size)/2; //draw in center vertically
        }
        paintElliptical(p, textColor, QRectF(left, top, size, size));
    }
    break;
    case LINEAR:
        drawHorizontalPoints(p, height() / 2, 0, this->beats);
        break;
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QBrush IntervalProgressDisplay::getBrush(int beat, int beatOffset)
{
    bool isIntervalFirstBeat = beat + beatOffset == 0;
    bool isMeasureFirstBeat = (beat + beatOffset) % beatsPerAccent == 0;
    bool isCurrentBeat = beat + beatOffset == currentBeat;

    if (isCurrentBeat)
    {
        if (isIntervalFirstBeat || (isShowingAccents() && isMeasureFirstBeat))
            return QBrush(Qt::green);
        else
            return QBrush(Qt::white);//highlight;
    }
    else{
        if(beatOffset > 0 && currentBeat < beatOffset){//drawing internal circles when playing external circle beats?
            return QBrush(TRANSPARENT_BRUSH_COLOR);
        }

        if (isShowingAccents() && isMeasureFirstBeat) {
            return QBrush(Qt::lightGray);
        }
    }
    //not the current beat
    return QBrush(Qt::gray);
}

qreal IntervalProgressDisplay::getOvalSize(int beat, int beatOffset)
{
    bool isCurrentBeat = beat + beatOffset == currentBeat;
    if (isCurrentBeat)
    {
        return ovalSize + 1;
    }
    else{
        bool isMeasureFirstBeat = (beat + beatOffset) % beatsPerAccent == 0;
        if (isShowingAccents() && isMeasureFirstBeat){
            return ovalSize;
        }
    }
    return ovalSize-1;
}

QPen IntervalProgressDisplay::getPen(int beat, int beatOffset)
{
    bool isIntervalFirstBeat = beat + beatOffset == 0;
    bool isMeasureFirstBeat = (beat + beatOffset) % beatsPerAccent == 0;
    bool isCurrentBeat = beat + beatOffset == currentBeat;

    if (isCurrentBeat) {
        if (isIntervalFirstBeat || (isShowingAccents() && isMeasureFirstBeat))
            return QPen(Qt::darkGreen, Qt::SolidLine); //accent and first beat
        else
            return QPen(Qt::darkGray, Qt::SolidLine);//highlight the current, but non accented beat
    }
    else{//not current beat
        if (beatOffset > 0 && currentBeat < beatOffset){//drawing internal circles when playing external circle beats?
            return QPen(Qt::NoPen);
        }

        if (isShowingAccents() && isMeasureFirstBeat) { //mark the accents
            return QPen(Qt::darkGreen, Qt::SolidLine);
        }
    }
    return QPen(Qt::darkGray, Qt::SolidLine);
}

void IntervalProgressDisplay::drawBeatCircles(QPainter &p, const QRectF &rect, int beatCircles, int offset, bool drawPath)
{

    if (drawPath) {
        paintEllipticalPath(p, rect, beatCircles);
    }

    //p.drawRect(rect);

    double angle = -PI / 2.0;
    qreal hRadius = rect.width()/2;
    qreal vRadius = rect.height()/2;
    qreal centerX = rect.center().x();
    qreal centerY = rect.center().y();

    //p.drawLine(centerX, 0, centerX, height());
    //p.drawLine(0, centerY, width(), centerY);

    for (int beat = beatCircles-1; beat >= 0; --beat) {
        qreal x = centerX + ((hRadius) * std::cos(angle));
        qreal y = centerY + ((vRadius) * std::sin(angle));

        QPen pen = getPen(beat, offset);
        QBrush brush = getBrush(beat, offset);

        // draw the current beat
        if (beat + offset >= currentBeat) {
            p.setBrush(brush);
            p.setPen(pen);
            qreal size = getOvalSize(beat, offset);
            p.drawEllipse(QPointF(x, y), size, size);
        }

        angle += 2 * -PI / beatCircles;
    }

}

// +++++++++++++++++++++++++ LINEAR PAINTING +++++++++++
void IntervalProgressDisplay::drawPoint(qreal x, qreal y, qreal size, QPainter &painter, int value,
                                        const QBrush &bgPaint, const QColor &border, bool small, bool drawText)
{
    painter.setBrush(bgPaint);
    painter.drawEllipse(QPointF(x, y), size/2, size/2);

    painter.setPen(QPen(border));
    painter.drawEllipse(QPointF(x, y), size/2, size/2);

    if (drawText) {
        QString valueString = QString::number(value);
        painter.setFont(small ? SMALL_FONT : BIG_FONT);
        qreal valueStringWidth = painter.fontMetrics().width(valueString);
        qreal textX = (x - valueStringWidth / 2.0);
        qreal textY = y + painter.fontMetrics().descent() + 2;
        if (!small)
            painter.setPen(Qt::black);
        painter.drawText(QPointF(textX, textY), valueString);
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

qreal IntervalProgressDisplay::getHorizontalSpace(int totalPoinstToDraw, int initialXPos) const
{
    return (float)(width() - initialXPos - LINEAR_PAINT_MODE_OVAL_SIZE/2) / (totalPoinstToDraw - 1);
}

void IntervalProgressDisplay::drawHorizontalPoints(QPainter &painter, qreal yPos, int startPoint,
                                         int totalPoinstToDraw)
{
    qreal initialXPos = 0 + LINEAR_PAINT_MODE_OVAL_SIZE / 2 + 1;
    qreal xSpace = getHorizontalSpace(totalPoinstToDraw, initialXPos+1);

    // draw the background line
    painter.setPen(QPen(LINEAR_BORDER_COLOR, 0.9f));
    painter.drawLine(initialXPos + (xSpace * currentBeat), yPos, initialXPos + (totalPoinstToDraw - 1) * xSpace,
                      yPos);

    qreal xPos = initialXPos;
    // draw all backgrounds first
    int size = (int)(LINEAR_PAINT_MODE_OVAL_SIZE * 0.5f);
    for (int i = startPoint; i < totalPoinstToDraw; i++) {
        bool canDraw = i >= currentBeat
                       && (xSpace >= (LINEAR_PAINT_MODE_OVAL_SIZE * 0.5) || (i % 2 == 0));
        if (canDraw) {
            QColor border = LINEAR_BORDER_COLOR;
            QColor bg = (i < currentBeat) ? LINEAR_BG_COLOR : LINEAR_BG_SECOND_COLOR;
            drawPoint(xPos, yPos, size, painter, (i + 1), bg, border, true);
        }
        xPos += xSpace;
    }

    // draw accents
    if (isShowingAccents()) {
        xPos = initialXPos;
        qreal size = (int)(LINEAR_PAINT_MODE_OVAL_SIZE * 0.6f);
        for (int i = 0; i < totalPoinstToDraw; i += beatsPerAccent) {
            if (i > currentBeat) {
                QColor bg = LINEAR_BG_SECOND_COLOR;
                drawPoint(xPos, yPos, size, painter, (i + 1), bg, LINEAR_ACCENT_BORDER_COLOR,
                          true);
            }
            xPos += xSpace * beatsPerAccent;
        }
    }
    // draw current beat
    xPos = initialXPos + (currentBeat * xSpace);
    QBrush bgPaint(highlightColor);
    bool isMeasureFirstBeat = currentBeat % beatsPerAccent == 0;
    bool isIntervalFirstBeat = currentBeat == 0;
    if (isIntervalFirstBeat || (isShowingAccents() && isMeasureFirstBeat )) {
        bgPaint = QColor(Qt::green);
    }
    drawPoint(xPos, yPos, LINEAR_PAINT_MODE_OVAL_SIZE, painter, (currentBeat + 1), bgPaint,
              Qt::darkGray, false, true);
}

// ++++++++++=
QSize IntervalProgressDisplay::minimumSizeHint() const
{
    QSize size = QWidget::minimumSizeHint();
    switch (paintMode) {
    case CIRCULAR:
        size.setWidth(height());
        break;
    case ELLIPTICAL:
        size.setWidth(height() * 2);
        break;
    case LINEAR:
        size.setWidth(height() * 5);
        break;
    }
    return size;
}
