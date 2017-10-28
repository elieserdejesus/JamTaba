#ifndef NINJAMTRACKGROUPVIEW_H
#define NINJAMTRACKGROUPVIEW_H

#include "TrackGroupView.h"
#include <QLabel>
#include <QBoxLayout>
#include "MarqueeLabel.h"
#include "NinjamTrackView.h"
#include "video/VideoWidget.h"

namespace Controller {
class MainController;
}

namespace Persistence {
class CacheEntry;
}

enum class TracksLayout {
    VerticalLayout,
    HorizontalLayout,
    GridLayout
};

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

    void setTracksLayout(TracksLayout newLayout);

    QSize sizeHint() const override;

    void addVideoInterval(const QByteArray &encodedVideoData);

    QColor getTintColor() const;

    static const uint MAX_WIDTH_IN_GRID_LAYOUT;
    static const uint MAX_HEIGHT_IN_GRID_LAYOUT;

signals:
    void createPrivateChat(const QString &userName, const QString &userIP);

public slots:
    void updateVideoFrame(const QImage &frame);

protected:
    NinjamTrackView *createTrackView(long trackID) override;

    void populateContextMenu(QMenu &contextMenu) override;

private:
    Controller::MainController *mainController;
    QLabel *countryLabel;
    QLabel *countryFlag;
    MarqueeLabel *groupNameLabel;
    QLabel *chatBlockIconLabel;
    QString userIP;
    TracksLayout tracksLayoutEnum;

    VideoWidget *videoWidget;
    QByteArray encodedVideoData;
    quint64 lastVideoRender;
    QList<QList<QImage>> decodedImages;
    uint videoFrameRate;

    void setupHorizontalLayout();
    void setupVerticalLayout();
    void setupGridLayout();

    QString getRgbaColorString(const QColor &color, int alpha);

    Qt::Orientation getTracksOrientation() const;

    void resetMainLayoutStretch();

private slots:
    void updateGeoLocation(const QString &resolvedIp);
    void blockChatMessages();
    void unblockChatMessages();
    void hideChatBlockIcon(const QString &unblockedUserName);
    void showChatBlockIcon(const QString &blockedUserName);

    void startVideoStream();
};

#endif // NINJAMTRACKGROUPVIEW_H
