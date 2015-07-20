#ifndef CIRCULAR_PROGRESS_DISPLAY
#define CIRCULAR_PROGRESS_DISPLAY

#include <QWidget>


class QResizeEvent;
class QPaintEvent;
class QPainter;

class CircularIntervalProgressDisplay : public QWidget{

public:
    CircularIntervalProgressDisplay(QWidget* parent) ;
    inline bool isShowingAccents() const{ return showAccents;}
    void setShowAccents(bool showAccents) ;
    void setCurrentBeat(int interval) ;
    void setBeatsPerAccent(int beats) ;
    inline int getBeatsPerInterval() const{return beats; }
    void setBeatsPerInterval(int beats) ;
    virtual void paintEvent(QPaintEvent* e);
    virtual void resizeEvent(QResizeEvent * event);
    void setSliceNumberColor(QColor sliceNumberColor) ;
    inline QColor getSliceNumberColor() const{ return sliceNumberColor; }

private:

    void drawCircles(QPainter* p, int theRadius, int circles, int startIterval) ;
    void initialize();

    static const int MARGIN = 2;
    static const int OVAL_SIZE = 6;

    int currentBeat = 0;
    int beats = 16;

    int radius;// = size / 2;
    int centerX;// = getWidth() / 2;
    int centerY;// = getHeight() / 2;

    QColor sliceNumberColor = Qt::gray;

    int beatsPerAccent;

    bool showAccents = true;

};

#endif
