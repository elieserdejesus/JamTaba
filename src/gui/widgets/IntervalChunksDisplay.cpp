#include "IntervalChunksDisplay.h"

#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QColor>

const QColor IntervalChunksDisplay::DARK_COLOR = QColor(0, 0, 0, 75);
const QColor IntervalChunksDisplay::LIGHT_COLOR = QColor(0, 0, 0, 100);

IntervalChunksDisplay::IntervalChunksDisplay(QWidget* parent)
    :QWidget(parent), downloadedChunks(0), totalChunks(0), stackedIntervals(0){

    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    setToolTip("Downloaded intervals");
    setAccessibleDescription(toolTip());
}

QSize IntervalChunksDisplay::minimumSizeHint() const{
    QSize minSize = QWidget::minimumSizeHint();
    minSize.setHeight(stackedIntervals > 0 ? stackedIntervals * 2 : 2);
    if(minSize.width() <= 0){
        minSize.setWidth(width());
    }
    return minSize;
}

QSize IntervalChunksDisplay::sizeHint() const{
    return minimumSizeHint();
}

void IntervalChunksDisplay::incrementDownloadedChunks(){
    downloadedChunks++;
    if(stackedIntervals <= 0){
        stackedIntervals = 1;
    }
    update();
}

void IntervalChunksDisplay::pushNewDownloadInStack(){
    stackedIntervals++;
    totalChunks = downloadedChunks;
    downloadedChunks = 0;
    if(stackedIntervals > 2){//limit the displayed intervals
        stackedIntervals = 2;
    }
    updateGeometry();
    update();
}

void IntervalChunksDisplay::popDownloadFromStack(){
    stackedIntervals--;
    if(stackedIntervals < 0){
        stackedIntervals = 0;
    }
    update();
}

void IntervalChunksDisplay::reset(){
    stackedIntervals = totalChunks = downloadedChunks = 0;
    update();
}

void IntervalChunksDisplay::paintEvent(QPaintEvent * ev){
    Q_UNUSED(ev)
    QPainter painter(this);

    //draw the fully downloaded chunks
    if(stackedIntervals > 1){
        painter.setPen(DARK_COLOR);
        for (int c = 0; c < stackedIntervals-1; ++c) {
            int y = c * 2;
            painter.drawLine(0, y, width()-1, y);
        }
    }

    //draw the current interval chunks
    if(downloadedChunks > 0){
        //drawing unfinished interval chunks?
        if( totalChunks <= 0 || downloadedChunks < totalChunks - 1){
            const int CHUNKS_MARGIN = 2;
            int chunkWidth = totalChunks > 0 ? (width()/totalChunks) : 2;//assume 2 pixels if the totalChunks value is unknown (the first interval is not fully downloaded)
            int chunkX = 0;
            int chunkY = height() - 2;
            painter.setPen(LIGHT_COLOR);
            for (int c = 0; c < downloadedChunks; ++c) {
                painter.drawLine(chunkX, chunkY, chunkX + chunkWidth, chunkY);
                chunkX += chunkWidth + CHUNKS_MARGIN;
            }
        }
        else{//drawing finished interval chunk
            painter.setPen(DARK_COLOR);
            int y = height()-2;
            painter.drawLine(0, y, width()-1, y);
        }
    }

}

