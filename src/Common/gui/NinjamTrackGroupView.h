#ifndef NINJAMTRACKGROUPVIEW_H
#define NINJAMTRACKGROUPVIEW_H

#include "TrackGroupView.h"
#include "ui_TrackGroupView.h"
#include <QLabel>
#include "MarqueeLabel.h"
#include "NinjamTrackView.h"

namespace Controller {
class MainController;
}

namespace Persistence {
class CacheEntry;
}

class NinjamTrackGroupView : public TrackGroupView
{
public:
    NinjamTrackGroupView(QWidget *parent, Controller::MainController *mainController, long trackID,
                         QString channelName, Persistence::CacheEntry initialValues);
    ~NinjamTrackGroupView();
    void setNarrowStatus(bool narrow);
    void updateGeoLocation();
    void setGroupName(const QString &groupName);
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

#endif // NINJAMTRACKGROUPVIEW_H
