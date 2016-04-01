#include "IntervalProgressDisplay.h"

#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

const double IntervalProgressDisplay::PI = 3.141592653589793238462643383279502884;

const QColor IntervalProgressDisplay::CURRENT_ACCENT_COLOR = Qt::green;
const QColor IntervalProgressDisplay::ACCENT_COLOR = QColor(240, 240, 240);
const QColor IntervalProgressDisplay::SECONDARY_BEATS_COLOR = Qt::gray;
const QColor IntervalProgressDisplay::DISABLED_BEATS_COLOR = QColor(0, 0, 0, 15);
const QColor IntervalProgressDisplay::TEXT_COLOR = Qt::black;

IntervalProgressDisplay::PaintStrategy::PaintStrategy()
    :font("Verdana")
{

}

IntervalProgressDisplay::PaintStrategy::~PaintStrategy()
{

}
// ++++++++++++++++++++++++++++++++++++++++++++++++++
IntervalProgressDisplay::IntervalProgressDisplay(QWidget *parent) :
    QFrame(parent),
    paintMode(PaintMode::LINEAR),
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

void IntervalProgressDisplay::setPaintMode(PaintMode mode)
{
    if (mode != this->paintMode) {
        this->paintMode = mode;
        paintStrategy.reset(createPaintStrategy(mode));
        updateGeometry();
        repaint();
    }
}

IntervalProgressDisplay::PaintStrategy *IntervalProgressDisplay::createPaintStrategy(PaintMode paintMode) const
{
    switch (paintMode) {
    case PaintMode::LINEAR:     return new LinearPaintStrategy();
    case PaintMode::ELLIPTICAL: return new EllipticalPaintStrategy();
    case PaintMode::CIRCULAR:   return new CircularPaintStrategy();
    case PaintMode::PIE:        return new PiePaintStrategy();
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
        PaintColors paintColors(currentBeatColor, SECONDARY_BEATS_COLOR, ACCENT_COLOR, CURRENT_ACCENT_COLOR, DISABLED_BEATS_COLOR, TEXT_COLOR);
        paintStrategy->paint(p, paintContext, paintColors);
    }
}

qreal IntervalProgressDisplay::getFontSize(PaintMode paintMode) const
{
    qreal baseFontSize = 8.0;
    int size;
    switch (paintMode) {
        case PaintMode::LINEAR:     return qMax(baseFontSize, width() * 0.015);
        case PaintMode::CIRCULAR:
        case PaintMode::ELLIPTICAL:
        case PaintMode::PIE:        size = qMin(width(), height()); break;
    }
    return qMax(baseFontSize, size * 0.03);
}


qreal IntervalProgressDisplay::getElementsSize(PaintMode paintMode) const
{
    switch (paintMode) {
    case PaintMode::LINEAR: return qMax(width() * 0.035, 22.5);
    case PaintMode::CIRCULAR:
    case PaintMode::ELLIPTICAL:
    {
        int minSize = qMin(width(), height());
        return qMax(minSize * 0.035, 7.0);
    }
    case PaintMode::PIE:
    {
        int minSize = qMin(width(), height());
        return qMax(minSize * 0.1, 8.0);
    }
    }
    return 0;
}

QSize IntervalProgressDisplay::minimumSizeHint() const
{
    QSize size = QWidget::minimumSizeHint();
    switch (paintMode) {
    case CIRCULAR:
    case PIE:
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
                                                              const QColor &textColor)

    : currentBeat(currentBeat),
      secondaryBeat(secondaryBeat),
      accentBeat(accentBeat),
      currentAccentBeat(currentAccentBeat),
      disabledBeats(disabledBeats),
      textColor(textColor)
{

}

