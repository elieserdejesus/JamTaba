#ifndef STANDALONE_LOCAL_TRACKGROUP_VIEW_H
#define STANDALONE_LOCAL_TRACKGROUP_VIEW_H

#include "LocalTrackGroupView.h"

class StandaloneLocalTrackGroupView : public LocalTrackGroupView
{

public:
    BaseTrackView* addTrackView(long trackID) override;
    void refreshInputSelectionName(int inputTrackIndex);
    void refreshInputSelectionNames();

};

#endif
