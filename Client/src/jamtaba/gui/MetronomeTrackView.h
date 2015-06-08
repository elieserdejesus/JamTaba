#ifndef METRONOMETRACKVIEW_H
#define METRONOMETRACKVIEW_H

#include "BaseTrackView.h"

class MetronomeTrackView : public BaseTrackView
{
public:
    MetronomeTrackView(QWidget *parent, Controller::MainController* mainController);
    ~MetronomeTrackView();
};

#endif // METRONOMETRACKVIEW_H
