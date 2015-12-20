#include "IntervalChunksDisplay.h"

#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QColor>
#include <QStyleOption>

const QColor IntervalChunksDisplay::PROGRESS_COLOR = QColor(255, 255, 255, 100);

IntervalChunksDisplay::IntervalChunksDisplay(QWidget* parent)
    :QWidget(parent), downloadedChunks(0), totalChunks(0){

    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    setToolTip("Downloaded intervals");
    setAccessibleDescription(toolTip());
}

void IntervalChunksDisplay::incrementDownloadedChunks(){
    downloadedChunks++;
    update();
}

void IntervalChunksDisplay::startNewInterval(){
    totalChunks = downloadedChunks;
    downloadedChunks = 0;
    update();
}


void IntervalChunksDisplay::reset(){
    totalChunks = downloadedChunks = 0;
    update();
}

void IntervalChunksDisplay::paintEvent(QPaintEvent * ev){
    Q_UNUSED(ev)

    //to allow style sheet
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    //draw the current interval chunks

    if(downloadedChunks > 0){
        int chunkWidth = totalChunks > 0 ? ((width()-2)/totalChunks) : 2;//assume 2 pixels if the totalChunks value is unknown (the first interval is not fully downloaded)
        int rectWidth = downloadedChunks * chunkWidth;
        painter.fillRect(1, 1, rectWidth, height() -2, PROGRESS_COLOR);
    }

}

