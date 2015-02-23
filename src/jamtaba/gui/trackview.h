#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QWidget>

namespace Ui {
    class TrackView;
}

class TrackView : public QWidget
{
    Q_OBJECT

public:
    explicit TrackView(QWidget *parent = 0);
    ~TrackView();

protected:
    Ui::TrackView *ui;

protected:
    void paintEvent(QPaintEvent *);
};



#endif // TRACKVIEW_H
