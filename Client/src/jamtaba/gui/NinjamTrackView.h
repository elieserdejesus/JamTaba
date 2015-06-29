#ifndef NINJAMTRACKVIEW_H
#define NINJAMTRACKVIEW_H

#include "BaseTrackView.h"

class QLabel;
class QLineEdit;

class NinjamTrackView : public BaseTrackView
{
public:
    NinjamTrackView(QWidget *parent, Controller::MainController *mainController, long trackID, QString userName, QString channelName, QString countryName, QString countyCode);
    ~NinjamTrackView();
    void setUserName(QString newName);
    void setChannelName(QString newChannelName);


};

#endif // NINJAMTRACKVIEW_H
