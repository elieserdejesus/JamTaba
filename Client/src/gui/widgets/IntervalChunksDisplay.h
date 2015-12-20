#ifndef INTERVALCHUNKSDISPLAY_H
#define INTERVALCHUNKSDISPLAY_H

#include <QWidget>

class QPaintEvent;

class IntervalChunksDisplay : public QWidget
{
public:
    IntervalChunksDisplay(QWidget* parent);

    void incrementDownloadedChunks();
    void startNewInterval();
    void reset();
protected:
    void paintEvent(QPaintEvent *);

private:
    int downloadedChunks; //downloaded chunks in current interval
    int totalChunks; //total chunks in a interval

    static const QColor PROGRESS_COLOR;

};

#endif // INTERVALCHUNKSDISPLAY_H
