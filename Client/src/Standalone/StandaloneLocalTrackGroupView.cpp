#include "StandaloneLocalTrackGroupView.h"
#include "StandaloneLocalTrackView.h"

BaseTrackView *StandaloneLocalTrackGroupView::addTrackView(long trackID)
{
    StandaloneLocalTrackView *newTrackView
        = dynamic_cast<StandaloneLocalTrackView *>(LocalTrackGroupView::addTrackView(trackID));
    if (newTrackView) {
        if (getTracksCount() > 1)
            newTrackView->setToNoInput();
        else
            newTrackView->refreshInputSelectionName();
    }
    return newTrackView;
}

void StandaloneLocalTrackGroupView::refreshInputSelectionName(int inputTrackIndex)
{
    QList<LocalTrackView *> tracks = getTracks();
    foreach (LocalTrackView *trackView, tracks) {
        if (trackView->getInputIndex() == inputTrackIndex){
            dynamic_cast<StandaloneLocalTrackView*>(trackView)->refreshInputSelectionName();
        }
    }
}

void StandaloneLocalTrackGroupView::refreshInputSelectionNames()
{
    QList<LocalTrackView *> tracks = getTracks();
    foreach (LocalTrackView *trackView, tracks){
        dynamic_cast<StandaloneLocalTrackView*>(trackView)->refreshInputSelectionName();
    }
}
