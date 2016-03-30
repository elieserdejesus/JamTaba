#ifndef CIRCULAR_PROGRESS_DISPLAY
#define CIRCULAR_PROGRESS_DISPLAY

#include <QFrame>

class QResizeEvent;
class QPaintEvent;
class QPainter;
class QComboBox;

class IntervalProgressDisplay : public QFrame
{
    Q_OBJECT
public:
    enum PaintMode {
        CIRCULAR, ELLIPTICAL, LINEAR
    };
    explicit IntervalProgressDisplay(QWidget *parent);
    ~IntervalProgressDisplay();
    inline bool isShowingAccents() const
    {
        return showAccents;
    }

    void setShowAccents(bool showAccents);
    void setCurrentBeat(int interval);
    void setBeatsPerAccent(int beats);
    inline int getBeatsPerInterval() const
    {
        return beats;
    }

    inline int getBeatsPerAccent() const
    {
        return beatsPerAccent;
    }

    void setBeatsPerInterval(int beats);

    void setPaintMode(PaintMode mode);

    QSize minimumSizeHint() const;

    void setPaintUsingLowContrastColors(bool state);

protected:
    void paintEvent(QPaintEvent *e) override;

private:

    static QColor PLAYED_BEATS_FIRST_COLOR;
    static QColor PLAYED_BEATS_SECOND_COLOR;
    static QColor PATH_COLOR;

    // linear painting
    static const int LINEAR_PAINT_MODE_OVAL_SIZE;
    static const QColor LINEAR_SLICE_NUMBER_COLOR;
    static const QColor LINEAR_BG_COLOR;
    static const QColor LINEAR_BG_SECOND_COLOR;
    static const QColor LINEAR_BORDER_COLOR;
    static const QColor LINEAR_TEXT_COLOR;
    static const QColor LINEAR_ACCENT_BORDER_COLOR;

    PaintMode paintMode;

    QColor highlightColor;

    // ellipse and circle painting
    void paintElliptical(QPainter &p, const QColor &textColor, const QRectF &rect);
    void paintEllipticalPath(QPainter &p, const QRectF &rect, int beats);
    void drawBeatCircles(QPainter &p, const QRectF &rect, int beatCircles, int offset, bool drawPath);

    QBrush getBrush(int beat, int beatOffset);
    QPen getPen(int beat, int beatOffset);
    qreal getOvalSize(int beat, int beatOffset);

    // linear painting
    void drawPoint(qreal x, qreal y, qreal size, QPainter &g, int value, const QBrush &bgPaint, const QColor &border,
                   bool small, bool drawText = false);
    void drawHorizontalPoints(QPainter &painter, qreal yPos, int startPoint, int totalPoinstToDraw);
    qreal getHorizontalSpace(int totalPoinstToDraw, int initialXPos) const;

    const QFont SMALL_FONT;
    const QFont BIG_FONT;

    static const qreal MARGIN;
    static const qreal PREFERRED_OVAL_SIZE;
    qreal ovalSize;

    int currentBeat;
    int beats;

    static const QColor BEAT_NUMBER_COLOR;
    static const QColor TRANSPARENT_BRUSH_COLOR;

    int beatsPerAccent;

    bool showAccents;

    bool usingLowContrastColors;//used to paint with low contrast when using chord progressions

    static const double PI;
};

#endif
