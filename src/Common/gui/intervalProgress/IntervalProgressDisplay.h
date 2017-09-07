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

    // custom properties defined in CSS files
    Q_PROPERTY(QColor currentBeatColor MEMBER currentBeatColor)
    Q_PROPERTY(QColor accentsColor MEMBER accentsColor)
    Q_PROPERTY(QColor currentAccentColor MEMBER currentAccentColor)
    Q_PROPERTY(QColor secondaryBeatsColor MEMBER secondaryBeatsColor)
    Q_PROPERTY(QColor disabledBeatsColor MEMBER disabledBeatsColor)
    Q_PROPERTY(QColor linesColor MEMBER linesColor)

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
    void setAccentBeats(QList<int> accents);
    QList<int> getAccentBeats() const;

    inline int getBeatsPerInterval() const
    {
        return beatsPerInterval;
    }
    void setBeatsPerInterval(int beatsPerInterval);

    void setPaintMode(PaintShape mode);
    inline PaintShape getPaintMode() const{ return paintMode; }

    QSize minimumSizeHint() const override;

    QSize sizeHint() const override;

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
        QList<int> accentBeats;
        qreal elementsSize;//size of circle, pies, etc
        qreal fontSize;

        PaintContext(int width, int height, int beatsPerInterval, int currentBeat, bool showingAccents, QList<int> accentBeats, qreal elementsSize, qreal fontSize);
    };

    struct PaintColors
    {
        QColor currentBeat;         // the most highlighted beat
        QColor secondaryBeat;       // non current beats
        QColor accentBeat;          // used in first interval beat and accents when these beats are non current beat
        QColor currentAccentBeat;   // used when drawing the current beat and this beat is an accent
        QColor disabledBeats;
        QBrush textColor;
        QColor linesColor;

        PaintColors(const QColor &currentBeat, const QColor &secondaryBeat, const QColor &accentBeat, const QColor &currentAccentBeat, const QColor &disabledBeats, const QBrush &textColor, QColor linesColor);
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
        void drawPoint(qreal x, qreal y, qreal size, QPainter &painter, int value, const QBrush &bgPaint, bool small);
    };

    class EllipticalPaintStrategy : public PaintStrategy
    {
    public:
        void paint(QPainter &p, const PaintContext &context, const PaintColors &colors) override;

    protected:
        virtual QRectF createContextRect(const PaintContext& context, qreal margin) const;
        QBrush getBrush(int beat, int beatOffset, const PaintContext &context, const PaintColors &colors) const;
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
    QList<int> accentBeats;
    bool showAccents;
    bool usingLowContrastColors; // used to paint with low contrast when using chord progressions

    static const double PI;

    QColor currentBeatColor;
    QColor accentsColor;
    QColor currentAccentColor;
    QColor secondaryBeatsColor;
    QColor disabledBeatsColor;
    QColor linesColor;

    static const QColor DEFAULT_CURRENT_BEAT_COLOR;
    static const QColor DEFAULT_CURRENT_ACCENT_COLOR;
    static const QColor DEFAULT_ACCENTS_COLOR;
    static const QColor DEFAULT_SECONDARY_BEATS_COLOR;
    static const QColor DEFAULT_DISABLED_BEATS_COLOR;
    static const QColor DEFAULT_LINES_COLOR;
};

#endif
