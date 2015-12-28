#include "StandaloneLocalTrackGroupView.h"
#include "StandaloneLocalTrackView.h"
#include "MainWindowStandalone.h"

StandaloneLocalTrackGroupView::StandaloneLocalTrackGroupView(int index,
                                                             MainWindowStandalone *mainWindow) :
    LocalTrackGroupView(index, mainWindow)
{
}

void StandaloneLocalTrackGroupView::populateMenu(QMenu &menu)
{
    LocalTrackGroupView::populateMenu(menu);
    createSubChannelActions(menu);
}

void StandaloneLocalTrackGroupView::createSubChannelActions(QMenu &menu)
{
    QAction *addSubchannelAction = menu.addAction(QIcon(":/images/more.png"), "Add subchannel");
    QObject::connect(addSubchannelAction, SIGNAL(triggered()), this, SLOT(addSubChannel()));
    addSubchannelAction->setEnabled(trackViews.size() < MAX_SUB_CHANNELS);
    if (trackViews.size() > 1) {
        for (int i = 2; i <= trackViews.size(); ++i) {
            QIcon icon(":/images/less.png");
            QString text = "Remove subchannel " + QString::number(i);
            QAction *action = menu.addAction(icon, text);
            action->setData(i-1); // using track view index as user data
            QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(removeSubchannel()));
            QObject::connect(action, SIGNAL(hovered()), this, SLOT(highlightHoveredSubchannel()));
        }
    }
}

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
        if (trackView->getInputIndex() == inputTrackIndex)
            dynamic_cast<StandaloneLocalTrackView *>(trackView)->refreshInputSelectionName();
    }
}

void StandaloneLocalTrackGroupView::refreshInputSelectionNames()
{
    QList<LocalTrackView *> tracks = getTracks();
    foreach (LocalTrackView *trackView, tracks)
        dynamic_cast<StandaloneLocalTrackView *>(trackView)->refreshInputSelectionName();
}
