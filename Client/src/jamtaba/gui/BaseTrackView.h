#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QWidget>

namespace Ui {
    class TrackView;
}

namespace Controller {
    class MainController;
}

class BaseTrackView : public QWidget
{
    Q_OBJECT

public:
    BaseTrackView(QWidget *parent, Controller::MainController* mainController, long trackID);
    virtual ~BaseTrackView();
    void setPeaks(float left, float right);
    inline long getTrackID() const{return trackID;}
    static BaseTrackView* getTrackViewByID(long trackID);
    virtual void setEnabled(bool);

    virtual void setToNarrow();
    virtual void setToWide();

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    static const int NARROW_WIDTH = 80;
protected:
    Ui::TrackView *ui;
    Controller::MainController* mainController;
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *source, QEvent *ev);
    long trackID;
    bool activated;

    bool narrowed;
private:
    static QMap<long, BaseTrackView*> trackViews;

private slots:
    void onMuteClicked();
    void onSoloClicked();
    void onFaderMoved(int value);
    void onPanSliderMoved(int value);


};



#endif // TRACKVIEW_H
