#include "IntervalProgressDisplay.h"

#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QDebug>
#include <QStyleOption>
#include <cmath>

QColor IntervalProgressDisplay::PLAYED_BEATS_FIRST_COLOR = QColor(0, 0, 0, 0);// played beats are transparent
QColor IntervalProgressDisplay::PLAYED_BEATS_SECOND_COLOR = QColor(0, 0, 0, 0);
QColor IntervalProgressDisplay::PATH_COLOR = QColor(0, 0, 0, 20);

const int IntervalProgressDisplay::MARGIN = 2;
const int IntervalProgressDisplay::PREFERRED_OVAL_SIZE = 6;

const double IntervalProgressDisplay::PI = 3.141592653589793238462643383279502884;

// LINEAR PAINTING CONSTANTS
const QColor IntervalProgressDisplay::LINEAR_SLICE_NUMBER_COLOR = QColor(120, 120, 120);
const QColor IntervalProgressDisplay::LINEAR_BG_COLOR = QColor(230, 230, 230);
const QColor IntervalProgressDisplay::LINEAR_BG_SECOND_COLOR = QColor(160, 160, 160);
const QColor IntervalProgressDisplay::LINEAR_BORDER_COLOR = QColor(200, 200, 200);
const QColor IntervalProgressDisplay::LINEAR_TEXT_COLOR = QColor(180, 180, 180);// Color.LIGHT_GRAY;
const QColor IntervalProgressDisplay::LINEAR_ACCENT_BORDER_COLOR = QColor(120, 120, 120);
const int IntervalProgressDisplay::LINEAR_PAINT_MODE_OVAL_SIZE = 26;

// ++++++++++++++++++++++++++++++++++++++++++++++++++
IntervalProgressDisplay::IntervalProgressDisplay(QWidget *parent) :
    QWidget(parent),
    SMALL_FONT("Verdana, 7"),
    BIG_FONT("Verdana, 10"),
    ovalSize(PREFERRED_OVAL_SIZE),
    paintMode(PaintMode::LINEAR),
    showAccents(false),
    currentBeat(0),
    sliceNumberColor(Qt::gray),
    usingLowContrastColors(false),
    highlightColor(Qt::white)
{
    setAttribute(Qt::WA_NoBackground);

    setBeatsPerInterval(32);

    setShowAccents(false);

    initialize();
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

void IntervalProgressDisplay::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    initialize();
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
        initialize();
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
void IntervalProgressDisplay::paintCircular(QPainter &p, const QColor &textColor)
{
    int radius = std::min(horizontalRadius, verticalRadius);
    paintElliptical(p, textColor, radius, radius);
}

void IntervalProgressDisplay::paintEllipticalPath(QPainter &p, int hRadius, int vRadius)
{
    QPen pen(Qt::SolidLine);
    pen.setColor(PATH_COLOR);
    p.setPen(pen);
    p.setBrush(Qt::BrushStyle::NoBrush);
    p.drawEllipse(QPoint(width()/2, height()/2 + 2), hRadius, vRadius);
}

void IntervalProgressDisplay::paintElliptical(QPainter &p, const QColor &textColor, int hRadius,
                                              int vRadius)
{
    if (beats <= 32) {
        drawBeatCircles(p, hRadius, vRadius, beats, 0);// draw a ellipse
    } else {
        int elementsInOuterEllipse = beats / 2;
        int elementsInNestedEllipse = beats - elementsInOuterEllipse;
        drawBeatCircles(p, hRadius, vRadius, elementsInOuterEllipse, 0);
        drawBeatCircles(p, (int)(hRadius * 0.75), (int)(vRadius * 0.75), elementsInNestedEllipse,
                        elementsInOuterEllipse);
    }

    // draw current beat text in center
    p.setPen(textColor);// use text color form pallete, allowing style sheet
    QString numberString(QString::number(currentBeat + 1) + " / " + QString::number(beats));

    int strWidth = fontMetrics().width(numberString);
    p.drawText(centerX - strWidth / 2, centerY + fontMetrics().height()/2, numberString);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void IntervalProgressDisplay::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QColor textColor(Qt::black);

    QStyleOption opt;
    opt.init(this); // allow style sheet
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    switch (paintMode) {
    case ELLIPTICAL:
        paintElliptical(p, textColor, std::min(verticalRadius * 3, horizontalRadius),
                        verticalRadius);
        break;
    case CIRCULAR:
        paintCircular(p, textColor);
        break;
    case LINEAR:
        drawHorizontalPoints(&p, height() / 2, 0, this->beats);
        break;
    default:
        paintElliptical(p, textColor, horizontalRadius, verticalRadius);
        break;
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void IntervalProgressDisplay::setSliceNumberColor(const QColor &sliceNumberColor)
{
    this->sliceNumberColor = sliceNumberColor;
    update();
}

void IntervalProgressDisplay::drawBeatCircles(QPainter &p, int hRadius, int vRadius,
                                              int beatCircles, int offset)
{
    paintEllipticalPath(p, hRadius, vRadius);

    double angle = -PI / 2.0;

    const float FIRST_COLOR_POSITION = 0.5f;
    const float SECOND_COLOR_POSITION = 0.7f;

    int currentBeatX, currentBeatY;
    QBrush currentBeatBrush;

    for (int i = 0; i < beatCircles; i++) {
        int x = centerX + (hRadius  * std::cos(angle));// - ovalSize /2;
        int y = (centerY + (vRadius * std::sin(angle))) + ovalSize/2;

        QRadialGradient brush(x - ovalSize/3, y - ovalSize/3, ovalSize*2);
        QPen pen(Qt::NoPen);

        bool isIntervalFirstBeat = i + offset == 0;
        bool isMeasureFirstBeat = isShowingAccents() && (i + offset) % beatsPerAccent == 0;
        bool isCurrentBeat = i + offset == currentBeat;

        if (isCurrentBeat && (isIntervalFirstBeat || isMeasureFirstBeat)) { // first beats
            if (isIntervalFirstBeat || isShowingAccents()) {
                brush.setColorAt(FIRST_COLOR_POSITION, Qt::green);// accent beat colors
                brush.setColorAt(SECOND_COLOR_POSITION, Qt::darkGreen);
            }
            pen.setColor(Qt::darkGray);
            pen.setStyle(Qt::SolidLine);

            currentBeatX = x;
            currentBeatY = y;
            currentBeatBrush = brush;
        } else {
            if ((i + offset) % beatsPerAccent == 0 && (i+offset) > currentBeat) {
                if (isShowingAccents()) {
                    brush.setColorAt(FIRST_COLOR_POSITION, Qt::lightGray); // accent marks
                    brush.setColorAt(SECOND_COLOR_POSITION, Qt::gray);

                    pen.setColor(Qt::gray);
                    pen.setStyle(Qt::SolidLine);
                }
            } else {
                if ((i + offset) <= currentBeat) {
                    if (i + offset < currentBeat) {// played beats
                        brush.setColorAt(FIRST_COLOR_POSITION, PLAYED_BEATS_FIRST_COLOR); // played beats
                        brush.setColorAt(SECOND_COLOR_POSITION, PLAYED_BEATS_SECOND_COLOR);
                    } else {// the current beat is highlighted
                        brush.setColorAt(0.1f, highlightColor);  // playing beat highlight colors
                        brush.setColorAt(0.8f, Qt::darkGray);

                        pen.setColor(Qt::darkGray);
                        pen.setStyle(Qt::SolidLine);

                        currentBeatX = x;
                        currentBeatY = y;
                        currentBeatBrush = brush;
                    }
                } else {
                    // non played yet
                    brush.setColorAt(0.0f, Qt::darkGray);
                    brush.setColorAt(0.8f, Qt::darkGray);
                }
            }
        }

        if (!isCurrentBeat) { // the current beat is drawed
            if (!(offset > 0 && currentBeat < offset)) {// is drawing the external circles?
                p.setBrush(brush);
                p.setPen(pen);
                p.drawEllipse(QPoint(x, y), ovalSize, ovalSize);
            } else {// drawing internal circles when bpi is > 32
                p.setPen(pen);
                p.setBrush(QColor(160, 160, 160)); // the internal circles are darker
                p.drawEllipse(QPoint(x, y), ovalSize, ovalSize);
            }
        }

        angle -= 2 * -PI / beatCircles;
    }

    // draw the current beat in top of other circles
    p.setBrush(currentBeatBrush);
    QPen currentBeatPen(QColor(85, 85, 85), 1);
    p.setPen(currentBeatPen);
    p.drawEllipse(QPoint(currentBeatX, currentBeatY), ovalSize+1, ovalSize+1);
}

// +++++++++++++++++++++++++ LINEAR PAINTING +++++++++++
void IntervalProgressDisplay::drawPoint(int x, int y, int size, QPainter *painter, int value,
                                        const QBrush &bgPaint, const QColor &border, bool small, bool drawText)
{
    painter->setBrush(bgPaint);
    painter->drawEllipse(QPoint(x, y), size/2, size/2);

    painter->setPen(QPen(border));
    painter->drawEllipse(QPoint(x, y), size/2, size/2);

    if (drawText) {
        QString valueString = QString::number(value);
        painter->setFont(small ? SMALL_FONT : BIG_FONT);
        int valueStringWidth = painter->fontMetrics().width(valueString);
        int textX = (int)(x - valueStringWidth / 2.0) -1;
        int textY = y + painter->fontMetrics().descent() + 2;
        if (!small)
            painter->setPen(Qt::black);
        painter->drawText(textX, textY, valueString);
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

float IntervalProgressDisplay::getHorizontalSpace(int totalPoinstToDraw, int initialXPos) const
{
    return (float)(width() - initialXPos - LINEAR_PAINT_MODE_OVAL_SIZE/2) / (totalPoinstToDraw - 1);
}

void IntervalProgressDisplay::drawHorizontalPoints(QPainter *painter, int yPos, int startPoint,
                                         int totalPoinstToDraw)
{
    int initialXPos = 0 + LINEAR_PAINT_MODE_OVAL_SIZE / 2 + 1;
    float xSpace = getHorizontalSpace(totalPoinstToDraw, initialXPos+1);

    // draw the background line
    painter->setPen(QPen(LINEAR_BORDER_COLOR, 0.9f));
    painter->drawLine(initialXPos + 1, yPos, (int)(initialXPos + (totalPoinstToDraw - 1) * xSpace),
                      yPos);

    float xPos = initialXPos;
    // draw all backgrounds first
    int size = (int)(LINEAR_PAINT_MODE_OVAL_SIZE * 0.5f);
    for (int i = startPoint; i < totalPoinstToDraw; i++) {
        bool canDraw = i >= currentBeat
                       && (xSpace >= (LINEAR_PAINT_MODE_OVAL_SIZE * 0.5) || (i % 2 == 0));
        if (canDraw) {
            QColor border = LINEAR_BORDER_COLOR;
            QColor bg = (i < currentBeat) ? LINEAR_BG_COLOR : LINEAR_BG_SECOND_COLOR;
            drawPoint((int)xPos, yPos, size, painter, (i + 1), bg, border, true);
        }
        xPos += xSpace;
    }

    // draw accents
    if (isShowingAccents()) {
        xPos = initialXPos;
        int size = (int)(LINEAR_PAINT_MODE_OVAL_SIZE * 0.6f);
        for (int i = 0; i < totalPoinstToDraw; i += beatsPerAccent) {
            if (i > currentBeat) {
                QColor bg = LINEAR_BG_SECOND_COLOR;
                drawPoint((int)xPos, yPos, size, painter, (i + 1), bg, LINEAR_ACCENT_BORDER_COLOR,
                          true);
            }
            xPos += xSpace * beatsPerAccent;
        }
    }
    // draw current beat
    xPos = initialXPos + (currentBeat * xSpace);
    QRadialGradient bgRadial(xPos-5, yPos-5, LINEAR_PAINT_MODE_OVAL_SIZE*2);
    bgRadial.setColorAt(0.1f, highlightColor);
    bgRadial.setColorAt(0.8f, Qt::black);
    drawPoint((int)xPos, yPos, LINEAR_PAINT_MODE_OVAL_SIZE, painter, (currentBeat + 1), bgRadial,
              Qt::darkGray, false, true);
}

// +++++++++++++++++++++++++++++++++
void IntervalProgressDisplay::initialize()
{
    horizontalRadius = width()/2 - ovalSize*2;
    verticalRadius = height()/2 - ovalSize*2;
    centerX = width() / 2;
    centerY = height() / 2;
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
