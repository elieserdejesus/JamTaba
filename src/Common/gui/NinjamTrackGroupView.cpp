#include "NinjamTrackGroupView.h"
#include "geo/IpToLocationResolver.h"
#include "MainController.h"
#include "NinjamController.h"
#include <QMenu>
#include <QDateTime>

using namespace Controller;
using namespace Persistence;

NinjamTrackGroupView::NinjamTrackGroupView(MainController *mainController, long trackID,
                                           const QString &channelName, const QColor &userColor,
                                           const CacheEntry &initialValues) :
    TrackGroupView(nullptr),
    mainController(mainController),
    userIP(initialValues.getUserIP()),
    orientation(Qt::Vertical)
{

    // change the top panel layout to vertical (original is horizontal)
    topPanelLayout->setDirection(QHBoxLayout::TopToBottom);
    topPanelLayout->setContentsMargins(1, 1, 1, 1);
    topPanelLayout->setSpacing(3);

    // replace the original QLineEdit with a MarqueeLabel
    topPanelLayout->removeWidget(groupNameField);
    delete groupNameField;
    groupNameLabel = new MarqueeLabel();
    groupNameLabel->setObjectName("groupNameField");
    groupNameLabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum));

    QHBoxLayout *groupNameLayout = new QHBoxLayout();
    groupNameLayout->addWidget(groupNameLabel, 1);
    chatBlockIconLabel = new QLabel(this);
    chatBlockIconLabel->setPixmap(QPixmap(":/images/chat_blocked.png"));
    chatBlockIconLabel->setVisible(false);

    groupNameLayout->addWidget(chatBlockIconLabel);
    topPanelLayout->addLayout(groupNameLayout);
    topPanelLayout->setAlignment(groupNameLayout, Qt::AlignBottom);

    setGroupName(initialValues.getUserName());

    // country flag label
    countryLabel = new QLabel();
    countryLabel->setObjectName("countryLabel");
    countryLabel->setTextFormat(Qt::RichText);
    countryLabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum));
    updateGeoLocation();
    topPanelLayout->addWidget(countryLabel);
    topPanelLayout->setAlignment(countryLabel, Qt::AlignTop);

    // create the first subchannel by default
    NinjamTrackView *newTrackView = addTrackView(trackID);
    newTrackView->setChannelName(channelName);
    newTrackView->setInitialValues(initialValues);

    QString styleSheet = "background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, ";
    styleSheet += "stop: 0 rgba(0, 0, 0, 0), ";
    styleSheet += "stop: 0.35 " + userColor.name() + ", ";
    styleSheet += "stop: 0.65" + userColor.name() + ", ";
    styleSheet += "stop: 1 rgba(0, 0, 0, 0));";
    groupNameLabel->setStyleSheet(styleSheet);

    videoWidget = new VideoWidget(this);
    videoWidget->setVisible(false); // video preview will be visible when the first received frame is decoded
    videoWidgetLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    videoWidgetLayout->addWidget(videoWidget, 1);
    videoWidgetLayout->setContentsMargins(3, 3, 3, 3);

    connect(mainController, SIGNAL(ipResolved(QString)), this, SLOT(updateGeoLocation(QString)));

    // reacting to chat block/unblock events
    Controller::NinjamController *ninjamController = mainController->getNinjamController();
    connect(ninjamController, SIGNAL(userBlockedInChat(QString)), this, SLOT(showChatBlockIcon(QString)));
    connect(ninjamController, SIGNAL(userUnblockedInChat(QString)), this, SLOT(hideChatBlockIcon(QString)));
    connect(ninjamController, SIGNAL(startingNewInterval()), this, SLOT(startVideoIntervalDecoding()));

    setupVerticalLayout();
}

void NinjamTrackGroupView::addVideoInterval(const QByteArray &encodedVideoData)
{
    videoIntervals << encodedVideoData;
}

void NinjamTrackGroupView::startVideoIntervalDecoding()
{
    demuxer.close(); // close previous video interval decoder

    if (!videoIntervals.isEmpty()) {
        const QByteArray &videoData = videoIntervals.takeLast();

        videoIntervals.clear(); // always take the last video interval and discard others (if downloaded but not played yet)

        if (!demuxer.open(videoData)) {
            qCritical() << "Demuxer can't open video interval data!";
            demuxer.close();
        }
    }
    else {
        videoWidget->setVisible(false); // hide the video widget when transmition is stopped
        mainLayout->removeItem(videoWidgetLayout);
        updateGeometry();
    }
}

void NinjamTrackGroupView::updateVideoFrame(const QImage &frame)
{
    videoWidget->setCurrentFrame(frame);

    if (!videoWidgetLayout->parent())
        mainLayout->addLayout(videoWidgetLayout);

    videoWidget->setVisible(true);
}

void NinjamTrackGroupView::hideChatBlockIcon(const QString &unblockedUserName)
{
    if (unblockedUserName == getGroupName())
        chatBlockIconLabel->hide();
}

void NinjamTrackGroupView::showChatBlockIcon(const QString &blockedUserName)
{
    if (blockedUserName == getGroupName())
        chatBlockIconLabel->show();
}

void NinjamTrackGroupView::populateContextMenu(QMenu &contextMenu)
{
    QString userName = getGroupName();
    bool userIsBlockedInChat = mainController->getNinjamController()->userIsBlockedInChat(userName);
    QAction *blockAction = contextMenu.addAction(tr("Block %1 in chat").arg(userName), this, SLOT(blockChatMessages()));
    QAction *unblockAction = contextMenu.addAction(tr("Unblock %1 in chat").arg(userName), this, SLOT(unblockChatMessages()));
    blockAction->setEnabled(!userIsBlockedInChat);
    unblockAction->setEnabled(userIsBlockedInChat);

    TrackGroupView::populateContextMenu(contextMenu);
}

void NinjamTrackGroupView::blockChatMessages()
{
    QString userNameToBlock = getGroupName();
    mainController->blockUserInChat(userNameToBlock);
}

void NinjamTrackGroupView::unblockChatMessages()
{
    QString userNameToUnblock = getGroupName();
    mainController->unblockUserInChat(userNameToUnblock);
}

void NinjamTrackGroupView::updateGeoLocation(const QString &ip)
{
    if (ip != this->userIP)
        return;

    Geo::Location location = mainController->getGeoLocation(ip);
    QString countryCode = location.getCountryCode().toLower();
    QString flagImageHTML = "<img src=:/flags/flags/" + countryCode +".png>";
    countryLabel->setText(flagImageHTML + "<br>" + location.getCountryName());
}

void NinjamTrackGroupView::updateGeoLocation()
{
    updateGeoLocation(this->userIP);
}

void NinjamTrackGroupView::setEstimatedChunksPerInterval(int estimatedChunks)
{
    for (NinjamTrackView * track : getTracks<NinjamTrackView *>()) {
        track->setEstimatedChunksPerInterval(estimatedChunks);
    }
}

QString NinjamTrackGroupView::getRgbaColorString(const QColor &color, int alpha)
{
    int red = color.red();
    int green = color.green();
    int blue = color.blue();
    return "rgba(" + QString::number(red) + "," + QString::number(green) + "," + QString::number(blue) + "," + QString::number(alpha) + ")";
}

void NinjamTrackGroupView::setOrientation(Qt::Orientation newOrientation)
{
    if (newOrientation == orientation)
        return;

    orientation = newOrientation;
    auto tracks = getTracks<NinjamTrackView *>();
    for (NinjamTrackView *track : tracks) {
        track->setOrientation(newOrientation);
    }

    if(newOrientation == Qt::Horizontal){
        setupHorizontalLayout();
        topPanel->setFixedWidth(100); // using fixed width in horizontal layout
    }
    else{
        setupVerticalLayout();
        topPanel->setMaximumWidth(QWIDGETSIZE_MAX);
        topPanel->setMinimumWidth(1);
    }

    setProperty("horizontal", newOrientation == Qt::Horizontal ? true : false);
    refreshStyleSheet();

    updateGeometry();
}

void NinjamTrackGroupView::setupHorizontalLayout()
{
    tracksLayout->setDirection(QBoxLayout::TopToBottom);
    mainLayout->setDirection(QBoxLayout::LeftToRight);

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));
}

void NinjamTrackGroupView::setupVerticalLayout()
{
    tracksLayout->setDirection(QBoxLayout::LeftToRight);
    mainLayout->setDirection(QBoxLayout::TopToBottom);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
}

NinjamTrackView *NinjamTrackGroupView::createTrackView(long trackID)
{
    return new NinjamTrackView(mainController, trackID);
}

void NinjamTrackGroupView::setGroupName(const QString &groupName)
{
    groupNameLabel->setText(groupName);
}

QString NinjamTrackGroupView::getGroupName() const
{
    return groupNameLabel->text();
}

QSize NinjamTrackGroupView::sizeHint() const
{
    if(orientation == Qt::Vertical )
        return TrackGroupView::sizeHint();

    // using horizontal layout
    int height = 0;
    foreach (BaseTrackView *trackView, trackViews)
        height += trackView->minimumSizeHint().height();

    return QSize(1, qMax(height, 54));
}

NinjamTrackView *NinjamTrackGroupView::addTrackView(long trackID)
{
    NinjamTrackView *newTrackView = dynamic_cast<NinjamTrackView *>(TrackGroupView::addTrackView(trackID));
    newTrackView->setOrientation(this->orientation);
    return newTrackView;
}

void NinjamTrackGroupView::setNarrowStatus(bool narrow)
{
    for (BaseTrackView *trackView : trackViews) {
        bool setToWide = !narrow && trackViews.size() <= 1;
        if (setToWide)
            trackView->setToWide();
        else
            trackView->setToNarrow();
    }

    updateGeometry();
}

void NinjamTrackGroupView::updateGuiElements()
{
    TrackGroupView::updateGuiElements();
    groupNameLabel->updateMarquee();

    // video
    if (demuxer.isOpened()) {
        quint64 now = QDateTime::currentMSecsSinceEpoch();

        quint64 timePerFrame = 1000 / demuxer.getFrameRate();
        if (now - lastVideoRender >= timePerFrame) { // time to show a new video frame?
            lastVideoRender = now;
            updateVideoFrame(demuxer.decodeNextFrame());
        }
    }
}

NinjamTrackGroupView::~NinjamTrackGroupView()
{
    //
}
