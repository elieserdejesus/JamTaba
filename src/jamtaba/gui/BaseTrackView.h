#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QWidget>
#include <QVariant>
#include <QStyle>

namespace Ui {
    class BaseTrackView;
}

namespace Controller {
    class MainController;
}

class BaseTrackView : public QWidget
{
    Q_OBJECT

public:
    BaseTrackView(Controller::MainController* mainController, long trackID);
    virtual ~BaseTrackView();

    inline long getTrackID() const{return trackID;}
    static BaseTrackView* getTrackViewByID(long trackID);


    virtual void setToNarrow();
    virtual void setToWide();

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void updatePeaks();

    inline Controller::MainController* getMainController() const{return mainController;}

    static const int NARROW_WIDTH = 80;

    void setUnlightStatus(bool unlighted);
protected:
    Ui::BaseTrackView *ui;
    Controller::MainController* mainController;
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *source, QEvent *ev);
    long trackID;
    bool activated;

    bool narrowed;

    void setPeaks(float left, float right);
private:
    static QMap<long, BaseTrackView*> trackViews;



private slots:
    void onMuteClicked();
    void onSoloClicked();
    void onFaderMoved(int value);
    void onPanSliderMoved(int value);


};



#endif // TRACKVIEW_H
