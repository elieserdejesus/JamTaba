#ifndef METRONOMETRACKVIEW_H
#define METRONOMETRACKVIEW_H

#include "BaseTrackView.h"

class MetronomeTrackView : public BaseTrackView
{
public:
    MetronomeTrackView(Controller::MainController* mainController, long metronomeID, float initialGain, float initialPan);
    ~MetronomeTrackView();
};

#endif // METRONOMETRACKVIEW_H
