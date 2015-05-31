#include "NinjamTrackView.h"
#include "ui_BaseTrackView.h"

NinjamTrackView::NinjamTrackView(QWidget *parent, Controller::MainController *mainController, int trackID)
    :BaseTrackView(parent, mainController, trackID)
{
    this->setMaximumWidth(100);
}

NinjamTrackView::~NinjamTrackView()
{

}

