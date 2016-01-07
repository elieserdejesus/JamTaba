#ifndef NINJAMTRACKVIEW_H
#define NINJAMTRACKVIEW_H

#include "TrackGroupView.h"
#include "BaseTrackView.h"
#include "IntervalChunksDisplay.h"

#include "persistence/UsersDataCache.h"
#include "MarqueeLabel.h"

namespace Controller {
class MainController;
}

class QLabel;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class NinjamTrackView : public BaseTrackView
{
    Q_OBJECT
public:
    NinjamTrackView(Controller::MainController *mainController, long trackID);
    void setChannelName(QString name);
    void setInitialValues(Persistence::CacheEntry initialValues);

    // interval chunks visual feedback
    void incrementDownloadedChunks();// called when a interval part (a chunk) is received
    void finishCurrentDownload(); // called when the interval is fully downloaded
    void setDownloadedChunksDisplayVisibility(bool visible);

    void setUnlightStatus(bool status);

    void updateGuiElements();

private:
    MarqueeLabel *channelNameLabel;
    Persistence::CacheEntry cacheEntry;// used to remember the track controls values
    IntervalChunksDisplay *chunksDisplay;// display downloaded interval chunks
protected slots:
    // overriding the base class slots
    void toggleMuteStatus();
    void setGain(int value);
    void setPan(int value);
    void updateBoostValue();
};
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class NinjamTrackGroupView : public TrackGroupView
{
public:
    NinjamTrackGroupView(QWidget *parent, Controller::MainController *mainController, long trackID,
                         QString channelName, Persistence::CacheEntry initialValues);
    ~NinjamTrackGroupView();
    void setNarrowStatus(bool narrow);
    void updateGeoLocation();
    void setGroupName(QString groupName);
    void updateGuiElements();

    inline NinjamTrackView *addTrackView(long trackID) override
    {
        return dynamic_cast<NinjamTrackView *>(TrackGroupView::addTrackView(trackID));
    }

protected:
    NinjamTrackView *createTrackView(long trackID) override;

private:
    Controller::MainController *mainController;
    QLabel *countryLabel;
    MarqueeLabel *groupNameLabel;
    QString userIP;
};
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#endif // NINJAMTRACKVIEW_H
