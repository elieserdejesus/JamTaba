#ifndef NINJAMTRACKVIEW_H
#define NINJAMTRACKVIEW_H

#include "TrackGroupView.h"
#include "BaseTrackView.h"

namespace Controller{
    class MainController;
}

class QLabel;
//class QLineEdit;

class NinjamTrackView : public BaseTrackView{
    Q_OBJECT
public:
    NinjamTrackView(Controller::MainController* mainController, long trackID, QString channelName);
    void setChannelName(QString name);
private:
    QLabel* channelNameLabel;
};

class NinjamTrackGroupView : public TrackGroupView
{
public:
    NinjamTrackGroupView(QWidget *parent, Controller::MainController *mainController, long trackID, QString userName, QString channelName, QString countryName, QString countyCode);
    ~NinjamTrackGroupView();
//    void setChannelName(QString newChannelName);

private:
    Controller::MainController* mainController;
    QLabel* countryLabel;
};

#endif // NINJAMTRACKVIEW_H
