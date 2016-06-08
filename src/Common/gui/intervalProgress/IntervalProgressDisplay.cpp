#include "IntervalProgressDisplay.h"

#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

const double IntervalProgressDisplay::PI = 3.141592653589793238462643383279502884;

const QColor IntervalProgressDisplay::CURRENT_ACCENT_COLOR = Qt::green;
const QColor IntervalProgressDisplay::ACCENT_COLOR = QColor(225, 225, 225);
const QColor IntervalProgressDisplay::SECONDARY_BEATS_COLOR = Qt::gray;
const QColor IntervalProgressDisplay::DISABLED_BEATS_COLOR = QColor(0, 0, 0, 15);

IntervalProgressDisplay::PaintStrategy::PaintStrategy()
    :font("Verdana")
{
    font.setStretch(QFont::SemiCondensed);
}

IntervalProgressDisplay::PaintStrategy::~PaintStrategy()
{

}
// ++++++++++++++++++++++++++++++++++++++++++++++++++
IntervalProgressDisplay::IntervalProgressDisplay(QWidget *parent) :
    QFrame(parent),
    paintMode(PaintShape::LINEAR),
    showAccents(false),
    currentBeat(0),
    beatsPerAccent(0),
    usingLowContrastColors(false)
{
    //setAttribute(Qt::WA_NoBackground);

    setBeatsPerInterval(32);

    setShowAccents(false);

    paintStrategy.reset(createPaintStrategy(paintMode));
}

IntervalProgressDisplay::~IntervalProgressDisplay()
{

}

void IntervalProgressDisplay::setPaintUsingLowContrastColors(bool useLowContrastColors)
{
    usingLowContrastColors = useLowContrastColors;
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
        currentBeat = beat % beatsPerInterval;
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
        this->beatsPerInterval = beats;
        if (beatsPerAccent <= 0)
            beatsPerAccent = beats / 2;
    }
}

void IntervalProgressDisplay::setPaintMode(PaintShape mode)
{
    if (mode != this->paintMode) {
        this->paintMode = mode;
        paintStrategy.reset(createPaintStrategy(mode));
        updateGeometry();
        repaint();
    }
}

IntervalProgressDisplay::PaintStrategy *IntervalProgressDisplay::createPaintStrategy(PaintShape paintMode) const
{
    switch (paintMode) {
    case PaintShape::LINEAR:     return new LinearPaintStrategy();
    case PaintShape::ELLIPTICAL: return new EllipticalPaintStrategy();
    case PaintShape::CIRCULAR:   return new CircularPaintStrategy();
    case PaintShape::PIE:        return new PiePaintStrategy();
    }
    qCritical() << "Can't create a paint strategy to " << paintMode;
    return nullptr;
}

void IntervalProgressDisplay::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    if (paintStrategy) {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        qreal elementsSize = getElementsSize(paintMode);
        qreal fontSize = getFontSize(paintMode);
        PaintContext paintContext(width(), height(), beatsPerInterval, currentBeat, isShowingAccents(), beatsPerAccent, elementsSize, fontSize);
        QColor currentBeatColor = usingLowContrastColors ? Qt::lightGray : Qt::white;
        QBrush textBrush = palette().text(); //using the color define in loaded stylesheet theme
        PaintColors paintColors(currentBeatColor, SECONDARY_BEATS_COLOR, ACCENT_COLOR, CURRENT_ACCENT_COLOR, DISABLED_BEATS_COLOR, textBrush);
        paintStrategy->paint(p, paintContext, paintColors);
    }
}

qreal IntervalProgressDisplay::getFontSize(PaintShape paintMode) const
{
    qreal baseFontSize = 8.0;
    int size;
    switch (paintMode) {
        case PaintShape::LINEAR:     return qMax(baseFontSize, width() * 0.015);
        case PaintShape::CIRCULAR:
        case PaintShape::ELLIPTICAL:
        case PaintShape::PIE:        size = qMin(width(), height()); break;
    }
    return qMax(baseFontSize, size * 0.05);
}


qreal IntervalProgressDisplay::getElementsSize(PaintShape paintMode) const
{
    switch (paintMode) {
    case PaintShape::LINEAR: return qMax(width() * 0.04, 22.5);
    case PaintShape::CIRCULAR:
    case PaintShape::ELLIPTICAL:
    {
        int minSize = qMin(width(), height());
        return qMax(minSize * 0.035, 7.0);
    }
    case PaintShape::PIE:
    {
        int minSize = qMin(width(), height());
        return qMax(minSize * 0.1, 8.0);
    }
    }
    return 0;
}

void IntervalProgressDisplay::resizeEvent(QResizeEvent *e)
{
    if (baseSize.isEmpty()){
        baseSize = e->size();
    }
}

QSize IntervalProgressDisplay::minimumSizeHint() const
{
    QSize size = QWidget::minimumSizeHint();
    int h = baseSize.height() > 0 ? baseSize.height() : height();
    switch (paintMode) {
    case CIRCULAR:
    case PIE:
        size.setWidth(h);
        break;
    case ELLIPTICAL:
        size.setWidth(h * 2);
        break;
    case LINEAR:
        size.setWidth(h * 5);
        break;
    }
    return size;
}

IntervalProgressDisplay::PaintContext::PaintContext(int width, int height, int beatsPerInterval, int currentBeat,
                                                    bool drawAccents, int beatsPerAccent, qreal elementsSize, qreal fontSize)
    : width(width),
      height(height),
      beatsPerInterval(beatsPerInterval),
      currentBeat(currentBeat),
      showingAccents(drawAccents),
      beatsPerAccent(beatsPerAccent),
      elementsSize(elementsSize),
      fontSize(fontSize)
{

}

IntervalProgressDisplay::PaintColors::PaintColors(const QColor &currentBeat,
            const QColor &secondaryBeat, const QColor &accentBeat, const QColor &currentAccentBeat, const QColor &disabledBeats,
                                                              const QBrush &textColor)

    : currentBeat(currentBeat),
      secondaryBeat(secondaryBeat),
      accentBeat(accentBeat),
      currentAccentBeat(currentAccentBeat),
      disabledBeats(disabledBeats),
      textColor(textColor)
{

}

