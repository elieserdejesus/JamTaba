#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QWidget>

namespace Ui {
    class TrackView;
}

namespace Gui{

class TrackView : public QWidget
{
    Q_OBJECT

public:
    explicit TrackView(QWidget *parent = 0);
    ~TrackView();

private:
    Ui::TrackView *ui;

protected:
    void paintEvent(QPaintEvent *);
};

}

#endif // TRACKVIEW_H
