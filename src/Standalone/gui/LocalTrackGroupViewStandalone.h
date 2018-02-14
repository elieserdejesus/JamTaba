#ifndef STANDALONE_LOCAL_TRACKGROUP_VIEW_H
#define STANDALONE_LOCAL_TRACKGROUP_VIEW_H

#include "gui/LocalTrackGroupView.h"
#include "LocalTrackViewStandalone.h"

class MainWindowStandalone;

class LocalTrackGroupViewStandalone : public LocalTrackGroupView
{
    Q_OBJECT

public:
    LocalTrackGroupViewStandalone(int index, MainWindowStandalone *mainWindow);

    LocalTrackViewStandalone* addTrackView(long trackID) override;

    void refreshInputSelectionName(int inputTrackIndex);

    void refreshInputSelectionNames();

protected:
    void populateMenu(QMenu &menu) override;
    LocalTrackViewStandalone *createTrackView(long trackID) override;

private:
    void createSubChannelActions(QMenu &menu);

private slots:
     void repaintLocalTracks();

protected slots:
     void removeSubchannel() override;

};

#endif
