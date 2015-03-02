#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QWidget>

namespace Ui {
    class TrackView;
}

namespace Controller {
    class MainController;
}

class TrackView : public QWidget
{
    Q_OBJECT

public:
    TrackView(QWidget *parent, Controller::MainController* mainController, int trackID);
    ~TrackView();
    void setPeaks(float left, float right);
protected:
    Ui::TrackView *ui;
    Controller::MainController* mainController;
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *source, QEvent *ev);
    int trackID;

private slots:
    void onMuteClicked();
    void onSoloClicked();
    void onFaderMoved(int value);
    void onPanSliderMoved(int value);
};



#endif // TRACKVIEW_H
