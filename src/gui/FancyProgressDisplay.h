#ifndef FANCY_PROGRESS_DISPLAY_H
#define FANCY_PROGRESS_DISPLAY_H

#include <QWidget>


class FancyProgressDisplay : public QWidget{
private:
    static const int OVAL_SIZE = 26;//25;

    static const QColor sliceNumberColor;// = QColor(120, 120, 120);
    static const QColor bgPaintColor;// = QColor(230, 230, 230);
    static const QColor bgPaintSecondColor;// = Color(215, 215, 215);
    static const QColor borderPaint;// = QColor(200, 200, 200);
    static const QColor textPaint;// = QColor(180, 180, 180);// Color.LIGHT_GRAY;
    static const QColor accentBorderColor;// = QColor(160, 160, 160);

    int currentBeat;
    int beatsPerInterval;
    int beatsPerAccent;
    bool showAccents;

    QFontMetrics fontMetrics;

public:
    explicit FancyProgressDisplay(QWidget* parent);
    inline bool isShowingAccents() const{ return this->showAccents && beatsPerAccent > 0; }
    void setShowAccents(bool showAccents) ;
    void setCurrentBeat(int beat) ;
    void setBeatsPerAccent(int beats) ;
    void setBeatsPerInterval(int beatsPerInterval) ;
protected:
    void paintEvent(QPaintEvent *e);
private:
    void drawPoint(int x, int y, int size, QPainter* g, int value, QBrush bgPaint, QColor border) ;
    void drawPoints(QPainter* g, int yPos, int startPoint, int totalPoinstToDraw) ;
    float getHorizontalSpace(int totalPoinstToDraw, int initialXPos) const;

};
#endif// FANCY_PROGRESS_DISPLAY_H
