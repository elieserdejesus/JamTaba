#ifndef NINJAMTRACKVIEW_H
#define NINJAMTRACKVIEW_H

#include "BaseTrackView.h"
//#include "../loginserver/JamRoom.h"

class NinjamTrackView : public BaseTrackView
{
public:
    NinjamTrackView(QWidget *parent, Controller::MainController *mainController, long trackID, QString userName, QString channelName, QString countryName, QString countyCode);
    ~NinjamTrackView();
    void setUserName(QString newName);
    void setChannelName(QString newChannelName);

private:
    //Login::NinjamPeer* peer;
};

#endif // NINJAMTRACKVIEW_H
