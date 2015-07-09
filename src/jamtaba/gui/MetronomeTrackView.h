#ifndef METRONOMETRACKVIEW_H
#define METRONOMETRACKVIEW_H

#include "BaseTrackView.h"

class MetronomeTrackView : public BaseTrackView
{
public:
    MetronomeTrackView(QWidget *parent, Controller::MainController* mainController, long metronomeID);
    ~MetronomeTrackView();
};

#endif // METRONOMETRACKVIEW_H
