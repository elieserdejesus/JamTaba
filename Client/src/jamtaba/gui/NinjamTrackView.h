#ifndef NINJAMTRACKVIEW_H
#define NINJAMTRACKVIEW_H

#include "BaseTrackView.h"
#include "../loginserver/JamRoom.h"

class NinjamTrackView : public BaseTrackView
{
public:
    NinjamTrackView(QWidget *parent, Controller::MainController *mainController, int trackID, QString userName, QString channelName, QString countryName, QString countyCode);
    ~NinjamTrackView();

private:
    //Login::NinjamPeer* peer;
};

#endif // NINJAMTRACKVIEW_H
