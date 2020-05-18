#ifndef NINJAMTRACKGROUPVIEW_H
#define NINJAMTRACKGROUPVIEW_H

#include "TrackGroupView.h"
#include "NinjamTrackView.h"
#include "widgets/MarqueeLabel.h"
#include "video/VideoWidget.h"

#include <QLabel>
#include <QBoxLayout>

namespace controller {
class MainController;
}

namespace persistence {
class CacheEntry;
}

enum class TracksLayout
{
    VerticalLayout,
    HorizontalLayout,
    GridLayout
};

class NinjamTrackGroupView : public TrackGroupView
{
    Q_OBJECT

public:
    NinjamTrackGroupView(controller::MainController *mainController, long trackID,
                         const QString &channelName, NinjamTrackNode::ChannelMode channelMode, const QColor &userColor, const persistence::CacheEntry &initialValues);
    ~NinjamTrackGroupView();
    void setNarrowStatus(bool narrow);
    void updateGeoLocation();
    void setUserName(const QString &groupName);
    void updateGuiElements() override;
    void setEstimatedChunksPerInterval(int estimatedChunks);

    bool isShowingVideo() const;

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
    controller::MainController *mainController;
    QLabel *countryLabel;
    QLabel *countryFlag;
    MarqueeLabel *userNameLabel;
    QLabel *chatBlockIconLabel;
    QString userIP;
    TracksLayout tracksLayoutEnum;

    VideoWidget *videoWidget;
    QByteArray encodedVideoData;
    quint64 lastVideoRender;
    QList<QList<QImage>> decodedImages;
    uint videoFrameRate;
    uint intervalsWithoutReceiveVideo;

    void setupHorizontalLayout();
    void setupVerticalLayout();
    void setupGridLayout();

    QString getRgbaColorString(const QColor &color, int alpha);

    Qt::Orientation getTracksOrientation() const;

    void resetMainLayoutStretch();

    QString getUniqueName() const;

private slots:
    void updateGeoLocation(const QString &resolvedIp);
    void blockChatMessages();
    void unblockChatMessages();
    void hideChatBlockIcon(const QString &unblockedUserName);
    void showChatBlockIcon(const QString &blockedUserName);

    void startVideoStream();
};

#endif // NINJAMTRACKGROUPVIEW_H
