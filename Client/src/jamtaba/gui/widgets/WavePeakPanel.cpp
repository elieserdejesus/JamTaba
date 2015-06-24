#include "WavePeakPanel.h"

#include <QPainter>

const int WavePeakPanel::peaksRectWidth = 3;
const int WavePeakPanel::peaksPad = 2;
const int WavePeakPanel::roundBorderSize = 10;

WavePeakPanel::WavePeakPanel(QWidget* parent)
    :   QWidget(parent),
      startIndex(0),
      totalPeaks(0),
      addIndex(0),
      borderColor(QColor(128, 128, 128)),
      peaksColor(QColor(255, 0, 0)),
      drawBorder(false)
{
    setAutoFillBackground(false);
    recreatePeaksArray();
}


void WavePeakPanel::clearPeaks(){
    peaksArray.clear();
    startIndex = totalPeaks = addIndex = 0;
    update();
}

void WavePeakPanel::resizeEvent( QResizeEvent * /*event*/ ){
    recreatePeaksArray();
}

void WavePeakPanel::recreatePeaksArray() {
    int newSize = width() / (peaksRectWidth + peaksPad);
    peaksArray.resize(newSize, 0);
    startIndex = addIndex = 0;
    for (int i = 0; i < newSize; ++i) {
        peaksArray[i] = 0;
    }
}

void WavePeakPanel::addPeak(float peak) {
    if (peaksArray.size() <= 0) {
        recreatePeaksArray();
    }
    if (peaksArray.size() > 0) {
        peaksArray[addIndex] = peak;
        addIndex = (addIndex + 1) % peaksArray.size();
        if (totalPeaks < (int)peaksArray.size()) {
            totalPeaks++;
            startIndex = 0;
        } else {
            startIndex = (startIndex + 1) % peaksArray.size();
        }
        update();//repaint
    }
}


void WavePeakPanel::paintEvent(QPaintEvent */*event*/){

    if (isVisible()) {
        QPainter painter(this);
        int size = totalPeaks;
        for (int i = 0; i < size; i++) {
            int xPos = i * (peaksRectWidth + peaksPad);// (i + startIndex) % size;
            drawPeak(&painter, xPos, peaksArray[(i + startIndex) % peaksArray.size()]);
        }
        if (drawBorder) {
            //painter.set setColor(borderColor);
            painter.setPen(borderColor);
            painter.drawRoundedRect(0, 0, width() - 1, height() - 1, roundBorderSize, roundBorderSize);
        }
    }
}


void WavePeakPanel::drawPeak(QPainter* g, int x, float peak) {
    int peakHeight = (int) (height() * peak);
    if (peakHeight == 0) {
        peakHeight = 2;
    }
    int y = (height() - peakHeight) / 2;

    g->fillRect(x, y, peaksRectWidth, peakHeight, peaksColor);
}


void WavePeakPanel::setPeaksColor(QColor peaksColor) {
    this->peaksColor = peaksColor;
    update();
}



//void WavePeakPanel::setRoundBorderSize(int roundBorderSize) {
//    this->roundBorderSize = roundBorderSize;
//    update();
//}

void WavePeakPanel::setBorderColor(QColor borderColor) {
    this->borderColor = borderColor;
    update();
}

void WavePeakPanel::setDrawBorder(bool drawBorder) {
    this->drawBorder = drawBorder;
    update();
}


