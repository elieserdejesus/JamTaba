#ifndef CIRCULAR_PROGRESS_DISPLAY
#define CIRCULAR_PROGRESS_DISPLAY

#include <QWidget>

class QResizeEvent;
class QPaintEvent;
class QPainter;
class QComboBox;

class IntervalProgressDisplay : public QWidget{
    Q_OBJECT
public:
    enum PaintMode {CIRCULAR, ELLIPTICAL, LINEAR};
    IntervalProgressDisplay(QWidget* parent) ;
    ~IntervalProgressDisplay();
    inline bool isShowingAccents() const{ return showAccents;}
    void setShowAccents(bool showAccents) ;
    void setCurrentBeat(int interval) ;
    void setBeatsPerAccent(int beats) ;
    inline int getBeatsPerInterval() const{return beats; }
    void setBeatsPerInterval(int beats) ;
    void setSliceNumberColor(QColor sliceNumberColor) ;
    inline QColor getSliceNumberColor() const{ return sliceNumberColor; }
    void setPaintMode(PaintMode mode);
protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void resizeEvent(QResizeEvent * event);

private:

    static QColor PLAYED_BEATS_FIRST_COLOR;// = QColor(0, 0, 0, 20);
    static QColor PLAYED_BEATS_SECOND_COLOR;// = QColor(0, 0, 0, 35);
    static QColor PATH_COLOR;

    //linear painting
    static const int LINEAR_PAINT_MODE_OVAL_SIZE = 26;//25;
    static const QColor LINEAR_SLICE_NUMBER_COLOR;// = QColor(120, 120, 120);
    static const QColor LINEAR_BG_COLOR;// = QColor(230, 230, 230);
    static const QColor LINEAR_BG_SECOND_COLOR;// = Color(215, 215, 215);
    static const QColor LINEAR_BORDER_COLOR;// = QColor(200, 200, 200);
    static const QColor LINEAR_TEXT_COLOR;// = QColor(180, 180, 180);// Color.LIGHT_GRAY;
    static const QColor LINEAR_ACCENT_BORDER_COLOR;// = QColor(160, 160, 160)


    PaintMode paintMode;// = PaintMode::ELLIPTICAL;

    //ellipse and circle painting
    void paintElliptical(QPainter& p, QColor textColor, int hRadius, int vRadius);
    void paintEllipticalPath(QPainter &p, int hRadius, int vRadius);
    void paintCircular(QPainter& p, QColor textColor);
    void drawBeatCircles(QPainter &p, int hRadius, int vRadius, int beatCircles, int startIterval) ;

    //linear painting
    void drawPoint(int x, int y, int size, QPainter* g, int value, QBrush bgPaint, QColor border, bool small) ;
    void drawPoints(QPainter* g, int yPos, int startPoint, int totalPoinstToDraw) ;
    float getHorizontalSpace(int totalPoinstToDraw, int initialXPos) const;

    const QFont SMALL_FONT;
    const QFont BIG_FONT;

    void initialize();

    static const int MARGIN = 2;
    static const int PREFERRED_OVAL_SIZE = 6;
    int ovalSize = PREFERRED_OVAL_SIZE ;

    int currentBeat = 0;
    int beats = 16;

    int horizontalRadius;
    int verticalRadius;
    int centerX;// = getWidth() / 2;
    int centerY;// = getHeight() / 2;

    QColor sliceNumberColor = Qt::gray;

    int beatsPerAccent;

    bool showAccents = true;

    static const double PI;

};

#endif
