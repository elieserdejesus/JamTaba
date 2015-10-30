#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QWidget>
#include <QVariant>
#include <QStyle>
#include "../audio/core/AudioPeak.h"

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

    enum BoostValue{
        ZERO, MINUS, PLUS
    };

    static BoostValue intToBoostValue(int intValue){
        if(intValue == 0){
            return BoostValue::ZERO;
        }
        if(intValue > 0){
            return BoostValue::PLUS;
        }
        return BoostValue::MINUS;
    }

    inline long getTrackID() const{return trackID;}
    static BaseTrackView* getTrackViewByID(long trackID);

    virtual void setToNarrow();
    virtual void setToWide();

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void updateGuiElements();

    inline Controller::MainController* getMainController() const{return mainController;}

    virtual void setUnlightStatus(bool unlighted);

    static const int NARROW_WIDTH = 80;
    static const int WIDE_WIDTH = 120;
protected:

    Ui::BaseTrackView *ui;
    Controller::MainController* mainController;
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *source, QEvent *ev);
    long trackID;
    bool activated;

    bool narrowed;

    bool drawDbValue;

    void setPeaks(float left, float right);
private:
    static QMap<long, BaseTrackView*> trackViews;
    Audio::AudioPeak maxPeak;

    void drawFaderDbValue(QPainter& p);
    //void drawFaderDbMarks(QPainter& p);


private slots:
    void onMuteClicked();
    void onSoloClicked();
    void onFaderMoved(int value);
    void onPanSliderMoved(int value);

    void onBoostButtonClicked();

};



#endif // TRACKVIEW_H
