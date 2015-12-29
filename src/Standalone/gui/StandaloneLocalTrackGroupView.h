#ifndef STANDALONE_LOCAL_TRACKGROUP_VIEW_H
#define STANDALONE_LOCAL_TRACKGROUP_VIEW_H

#include "LocalTrackGroupView.h"

class StandaloneMainWindow;

class StandaloneLocalTrackGroupView : public LocalTrackGroupView
{
    Q_OBJECT

public:
    StandaloneLocalTrackGroupView(int index, StandaloneMainWindow *mainWindow);

    BaseTrackView* addTrackView(long trackID) override;

    void refreshInputSelectionName(int inputTrackIndex);

    void refreshInputSelectionNames();

protected:
    void populateMenu(QMenu &menu) override;
    BaseTrackView* createTrackView(long trackID) override;

private:
    void createSubChannelActions(QMenu &menu);

};

#endif
