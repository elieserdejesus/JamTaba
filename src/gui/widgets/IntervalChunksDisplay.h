#ifndef INTERVALCHUNKSDISPLAY_H
#define INTERVALCHUNKSDISPLAY_H

#include <QWidget>

class QPaintEvent;

class IntervalChunksDisplay : public QWidget
{
public:
    IntervalChunksDisplay(QWidget* parent);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void incrementDownloadedChunks();
    void pushNewDownloadInStack();
    void popDownloadFromStack();
    void reset();
protected:
    void paintEvent(QPaintEvent *);

private:
    int downloadedChunks; //downloaded chunks in current interval
    int totalChunks; //total chunks in a interval
    int stackedIntervals;

    static const QColor DARK_COLOR;
    static const QColor LIGHT_COLOR;
};

#endif // INTERVALCHUNKSDISPLAY_H
