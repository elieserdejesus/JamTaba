#pragma once

#include <QWidget>
#include <QColor>
#include <QObject>
#include <vector>
#include <QResizeEvent>
#include <QPainter>

/**
 * @author zeh
 */
class WavePeakPanel : public QWidget{

protected:
    void resizeEvent( QResizeEvent * /*event*/ );
    void paintEvent(QPaintEvent */*event*/);

private:
    static const int peaksRectWidth;
    static const int peaksPad;
    static const int roundBorderSize;

    std::vector<float> peaksArray;
    int startIndex = 0;
    int totalPeaks = 0;
    int addIndex = 0;

    QColor borderColor;

    QColor peaksColor;
    bool drawBorder;

    void recreatePeaksArray() ;
    void drawPeak(QPainter* g, int x, float peak) ;

public:
    WavePeakPanel(QWidget *parent=0);

    void addPeak(float peak) ;

    void setPeaksColor(QColor peaksColor) ;

    inline QColor getPeaksColor() const{
        return peaksColor;
    }

    //void setRoundBorderSize(int roundBorderSize) ;

    inline int getRoundBorderSize() const{
        return roundBorderSize;
    }

    void setBorderColor(QColor borderColor) ;

    inline QColor getBorderColor() const{
        return borderColor;
    }

    void setDrawBorder(bool drawBorder) ;

    inline bool isDrawBorder() const{
        return drawBorder;
    }

};
