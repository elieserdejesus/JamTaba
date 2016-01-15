#ifndef NINJAMTRACKGROUPVIEW_H
#define NINJAMTRACKGROUPVIEW_H

#include "TrackGroupView.h"
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
    Q_OBJECT
public:
    NinjamTrackGroupView(QWidget *parent, Controller::MainController *mainController, long trackID,
                         QString channelName, Persistence::CacheEntry initialValues);
    ~NinjamTrackGroupView();
    void setNarrowStatus(bool narrow);
    void updateGeoLocation();
    void setGroupName(const QString &groupName);
    void updateGuiElements();

    NinjamTrackView *addTrackView(long trackID) override;

    void setOrientation(Qt::Orientation orientation);

    QSize sizeHint() const override;

protected:
    NinjamTrackView *createTrackView(long trackID) override;

private:
    Controller::MainController *mainController;
    QLabel *countryLabel;
    MarqueeLabel *groupNameLabel;
    QString userIP;
    Qt::Orientation orientation;

    void setupHorizontalLayout();
    void setupVerticalLayout();
};

#endif // NINJAMTRACKGROUPVIEW_H
