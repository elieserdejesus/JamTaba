#include "LocalTrackGroupViewStandalone.h"
#include "LocalTrackViewStandalone.h"
#include "MainWindowStandalone.h"
#include "audio/core/LocalInputNode.h"

LocalTrackGroupViewStandalone::LocalTrackGroupViewStandalone(int index, MainWindowStandalone *mainWindow) :
    LocalTrackGroupView(index, mainWindow)
{
    //
}

void LocalTrackGroupViewStandalone::populateMenu(QMenu &menu)
{
    LocalTrackGroupView::populateMenu(menu);
    createSubChannelActions(menu);
}

void LocalTrackGroupViewStandalone::removeSubchannel()
{
    if (trackViews.size() > 1) { // only the second subchannel can be removed
         auto secondTrack = getTracks<LocalTrackViewStandalone *>().at(1);
         if (secondTrack) {
             auto inputNode = secondTrack->getInputNode();
             if (inputNode->isRoutingMidiInput()) {
                 secondTrack->setMidiRouting(false); // deactivate midi routing before delete the subchannel
             }
         }
    }

    LocalTrackGroupView::removeSubchannel();
}

void LocalTrackGroupViewStandalone::createSubChannelActions(QMenu &menu)
{
    QAction *addSubchannelAction = menu.addAction(QIcon(":/images/more.png"), tr("Add subchannel"));
    QObject::connect(addSubchannelAction, SIGNAL(triggered()), this, SLOT(addSubChannel()));
    addSubchannelAction->setEnabled(trackViews.size() < MAX_SUB_CHANNELS);
    if (trackViews.size() > 1) {
        for (int i = 2; i <= trackViews.size(); ++i) {
            QIcon icon(":/images/less.png");
            QString text = tr("Remove subchannel %1").arg(QString::number(i));
            QAction *action = menu.addAction(icon, text);
            action->setData(i-1); // using track view index as user data
            QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(removeSubchannel()));
            QObject::connect(action, SIGNAL(hovered()), this, SLOT(highlightHoveredSubchannel()));
        }
    }
}

//overrided factory method
LocalTrackViewStandalone* LocalTrackGroupViewStandalone::createTrackView(long trackID)
{
    auto controller = dynamic_cast<MainWindowStandalone *>(mainFrame)->getMainController();

    auto trackView = new LocalTrackViewStandalone(controller, trackID );

    connect(trackView, &LocalTrackViewStandalone::trackInputChanged, this, &LocalTrackGroupViewStandalone::repaintLocalTracks);

    return trackView;
}

void LocalTrackGroupViewStandalone::repaintLocalTracks()
{
    for (auto trackView : this->trackViews) {
        trackView->update();
    }
}

LocalTrackViewStandalone *LocalTrackGroupViewStandalone::addTrackView(long trackID)
{
    auto newTrackView = dynamic_cast<LocalTrackViewStandalone *>(LocalTrackGroupView::addTrackView(trackID));
    if (newTrackView) {
        if (getTracksCount() > 1)
            newTrackView->setToNoInput();
        else
            newTrackView->refreshInputSelectionName();
    }
    return newTrackView;
}

void LocalTrackGroupViewStandalone::refreshInputSelectionName(int inputTrackIndex)
{
    for (auto trackView : getTracks<LocalTrackViewStandalone *>()) {
        if (trackView->getInputIndex() == inputTrackIndex)
            trackView->refreshInputSelectionName();
    }
}

void LocalTrackGroupViewStandalone::refreshInputSelectionNames()
{
    for (auto trackView : getTracks<LocalTrackViewStandalone *>())
        trackView->refreshInputSelectionName();
}
