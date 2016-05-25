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
    NinjamTrackGroupView(Controller::MainController *mainController, long trackID,
                         const QString &channelName, const QColor &userColor, const Persistence::CacheEntry &initialValues);
    ~NinjamTrackGroupView();
    void setNarrowStatus(bool narrow);
    void updateGeoLocation();
    void setGroupName(const QString &groupName);
    QString getGroupName() const override;
    void updateGuiElements();
    void setEstimatedChunksPerInterval(int estimatedChunks);

    NinjamTrackView *addTrackView(long trackID) override;

    void setOrientation(Qt::Orientation orientation);

    QSize sizeHint() const override;

protected:
    NinjamTrackView *createTrackView(long trackID) override;

private:
    Controller::MainController *mainController;
    QLabel *countryLabel;
    MarqueeLabel *groupNameLabel;
    QLabel *chatBlockIconLabel;
    QString userIP;
    Qt::Orientation orientation;

    void setupHorizontalLayout();
    void setupVerticalLayout();

    QString getRgbaColorString(const QColor &color, int alpha);

private slots:
    void updateGeoLocation(const QString &resolvedIp);
    void showContextMenu(const QPoint &pos);
    void blockChatMessages();
    void unblockChatMessages();
    void hideChatBlockIcon(const QString &unblockedUserName);
    void showChatBlockIcon(const QString &blockedUserName);
};

#endif // NINJAMTRACKGROUPVIEW_H
