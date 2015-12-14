#ifndef NINJAMTRACKVIEW_H
#define NINJAMTRACKVIEW_H

#include "TrackGroupView.h"
#include "BaseTrackView.h"

#include "persistence/UsersDataCache.h"

namespace Controller{
    class MainController;
}

class QLabel;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class NinjamTrackView : public BaseTrackView{
    Q_OBJECT
public:
    NinjamTrackView(Controller::MainController* mainController, long trackID, QString channelName, Persistence::CacheEntry initialValues);
    void setChannelName(QString name);
private:
    QLabel* channelNameLabel;
    Persistence::CacheEntry cacheEntry;//used to remember the track controls values
protected slots:
    //overriding the base class slots
    void onMuteClicked();
    void onFaderMoved(int value);
    void onPanSliderMoved(int value);
    void onBoostButtonClicked();
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class NinjamTrackGroupView : public TrackGroupView
{
public:
    NinjamTrackGroupView(QWidget *parent, Controller::MainController *mainController, long trackID, QString channelName, Persistence::CacheEntry initialValues);
    ~NinjamTrackGroupView();
    void setNarrowStatus(bool narrow);
     void updateGeoLocation();

private:
    Controller::MainController* mainController;
    QLabel* countryLabel;
    QString userIP;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#endif // NINJAMTRACKVIEW_H
