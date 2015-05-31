#ifndef NINJAMTRACKVIEW_H
#define NINJAMTRACKVIEW_H

#include "BaseTrackView.h"

class NinjamTrackView : public BaseTrackView
{
public:
    NinjamTrackView(QWidget *parent, Controller::MainController* mainController, int trackID);
    ~NinjamTrackView();

private:

};

#endif // NINJAMTRACKVIEW_H
