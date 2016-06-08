#ifndef CIRCULAR_PROGRESS_DISPLAY
#define CIRCULAR_PROGRESS_DISPLAY

#include <QFrame>
#include <QScopedPointer>

class QResizeEvent;
class QPaintEvent;
class QPainter;
class QComboBox;

class IntervalProgressDisplay : public QFrame
{
    Q_OBJECT
public:
    enum PaintShape {
        CIRCULAR, ELLIPTICAL, LINEAR, PIE
    };

    explicit IntervalProgressDisplay(QWidget *parent);
    ~IntervalProgressDisplay();
    inline bool isShowingAccents() const
    {
        return showAccents;
    }

    void setShowAccents(bool showAccents);
    void setCurrentBeat(int interval);
    void setBeatsPerAccent(int beatsPerInterval);
    inline int getBeatsPerInterval() const
    {
        return beatsPerInterval;
    }

    inline int getBeatsPerAccent() const
    {
        return beatsPerAccent;
    }

    void setBeatsPerInterval(int beatsPerInterval);

    void setPaintMode(PaintShape mode);
    inline PaintShape getPaintMode() const{ return paintMode; }

    QSize minimumSizeHint() const;

    void setPaintUsingLowContrastColors(bool state);

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *) override;
private:

    struct PaintContext
    {
        int height;
        int width;
        int beatsPerInterval;
        int currentBeat;
        bool showingAccents;
        int beatsPerAccent;
        qreal elementsSize;//size of circle, pies, etc
        qreal fontSize;

        PaintContext(int width, int height, int beatsPerInterval, int currentBeat, bool showingAccents, int beatsPerAccent, qreal elementsSize, qreal fontSize);
    };

    struct PaintColors
    {
        QColor currentBeat; //white, the most highlighted beat
        QColor secondaryBeat; //non current beats
        QColor accentBeat;//used in first interval beat and accents when these beats are non current beat
        QColor currentAccentBeat;//used when drawing the current beat and this beat is an accent
        QColor disabledBeats;
        QBrush textColor;


        PaintColors(const QColor &currentBeat, const QColor &secondaryBeat, const QColor &accentBeat, const QColor &currentAccentBeat, const QColor &disabledBeats, const QBrush &textColor);
    };

    class PaintStrategy
    {
    public:
        PaintStrategy();
        virtual ~PaintStrategy();
        virtual void paint(QPainter &p, const PaintContext &context, const PaintColors &colors) = 0;

    protected:
        QFont font;
    };

    QScopedPointer<PaintStrategy> paintStrategy;

    class LinearPaintStrategy : public PaintStrategy
    {
    public:
        LinearPaintStrategy();
        void paint(QPainter &p, const PaintContext &context, const PaintColors &colors) override;
    private:
        qreal getHorizontalSpace(int width, qreal elementsSize, int totalPoinstToDraw, int initialXPos) const;
        void drawPoint(qreal x, qreal y, qreal size, QPainter &painter, int value, const QBrush &bgPaint, const QColor &border, bool small);
        static const QColor CIRCLES_BORDER_COLOR;
    };

    class EllipticalPaintStrategy : public PaintStrategy
    {
    public:
        void paint(QPainter &p, const PaintContext &context, const PaintColors &colors) override;

    protected:
        virtual QRectF createContextRect(const PaintContext& context, qreal margin) const;
        QBrush getBrush(int beat, int beatOffset, const PaintContext &context, const PaintColors &colors) const;
        QPen getPen(int beat, int beatOffset, const PaintContext &context) const;
        qreal getCircleSize(int beat, int beatOffset, const PaintContext &context) const;
        void drawCurrentBeatValue(QPainter &p, const QRectF &rect, const PaintContext &context, const PaintColors &colors);
    private:
        void paintEllipticalPath(QPainter &p, const QRectF &rect, const PaintColors &colors, int currentBeat, int beatsPerInterval);
        void drawCircles(QPainter &p, const QRectF &rect, const PaintContext &context, const PaintColors &colors, int beatsToDraw, int beatsToDrawOffset, bool drawPath);
        bool isMeasureFirstBeat(int beat, int beatOffset, const PaintContext &context) const;
    };

    class CircularPaintStrategy : public EllipticalPaintStrategy
    {
    protected:
        virtual QRectF createContextRect(const PaintContext& context, qreal margin) const override;
    };

    class PiePaintStrategy : public CircularPaintStrategy
    {
    public:
        void paint(QPainter &p, const PaintContext &context, const PaintColors &colors) override;

    private:
        void drawPies(QPainter &p, const QRectF &rect, const PaintContext &context, const PaintColors &colors, int beatsToDraw, int beatsToDrawOffset, qreal piesHeight);
        QPainterPath getClipPath(const QRectF &rect, qreal piesHeight) const;
    };

    PaintStrategy *createPaintStrategy(PaintShape paintMode) const;
    qreal getElementsSize(PaintShape paintMode) const;
    qreal getFontSize(PaintShape paintMode) const;

    PaintShape paintMode;

    QSize baseSize;

    int currentBeat;
    int beatsPerInterval;
    int beatsPerAccent;
    bool showAccents;
    bool usingLowContrastColors;//used to paint with low contrast when using chord progressions

    static const double PI;

    static const QColor CURRENT_ACCENT_COLOR;
    static const QColor ACCENT_COLOR;
    static const QColor SECONDARY_BEATS_COLOR;
    static const QColor DISABLED_BEATS_COLOR;
};

#endif
