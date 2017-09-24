#include "NinjamTrackGroupView.h"
#include "geo/IpToLocationResolver.h"
#include "MainController.h"
#include "NinjamController.h"
#include <QMenu>
#include <QDateTime>
#include <QLayout>
#include <QStackedLayout>

using namespace Controller;
using namespace Persistence;

const uint NinjamTrackGroupView::MAX_WIDTH_IN_GRID_LAYOUT = 350;
const uint NinjamTrackGroupView::MAX_HEIGHT_IN_GRID_LAYOUT = NinjamTrackGroupView::MAX_WIDTH_IN_GRID_LAYOUT * 0.64;

NinjamTrackGroupView::NinjamTrackGroupView(MainController *mainController, long trackID,
                                           const QString &channelName, const QColor &userColor,
                                           const CacheEntry &initialValues) :
    TrackGroupView(nullptr),
    mainController(mainController),
    userIP(initialValues.getUserIP()),
    tracksLayoutEnum(TracksLayout::VerticalLayout)
{

    // change the top panel layout to vertical (original is horizontal)
    topPanelLayout->setDirection(QHBoxLayout::TopToBottom);
    topPanelLayout->setContentsMargins(0, 0, 0, 0);
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

    topPanelLayout->setAlignment(countryLabel, Qt::AlignTop);

    setGroupName(initialValues.getUserName());

    // country flag and label
    countryLabel = new QLabel();
    countryLabel->setObjectName("countryLabel");

    countryFlag = new QLabel();

    updateGeoLocation();

    topPanelLayout->addSpacing(6);
    topPanelLayout->addWidget(countryFlag);
    topPanelLayout->addWidget(countryLabel);
    topPanelLayout->setAlignment(countryFlag, Qt::AlignCenter);

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
    connect(videoWidget, &VideoWidget::visibilityChanged, [=](bool visible) {

        if (tracksLayoutEnum == TracksLayout::GridLayout && visible) {
            setupGridLayout();
        }

    });

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
        mainLayout->removeWidget(videoWidget);
        updateGeometry();
    }
}

void NinjamTrackGroupView::updateVideoFrame(const QImage &frame)
{
    videoWidget->setCurrentFrame(frame);

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
    QString flagImage = ":/flags/flags/" + countryCode +".png";
    countryFlag->setPixmap(QPixmap(flagImage));
    countryLabel->setText(location.getCountryName());
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

void NinjamTrackGroupView::setTracksLayout(TracksLayout newLayout)
{
    if (newLayout == tracksLayoutEnum)
        return;

    tracksLayoutEnum = newLayout;
    auto tracks = getTracks<NinjamTrackView *>();
    Qt::Orientation orientation = getTracksOrientation();
    for (NinjamTrackView *track : tracks) {
        track->setOrientation(orientation);
    }

    if (newLayout == TracksLayout::HorizontalLayout) {
        setupHorizontalLayout();
        topPanel->setFixedWidth(100); // using fixed width in horizontal layout
    }
    else if (newLayout == TracksLayout::VerticalLayout) {
        setupVerticalLayout();
        topPanel->setMaximumWidth(QWIDGETSIZE_MAX);
        topPanel->setMinimumWidth(1);
    }
    else if(newLayout == TracksLayout::GridLayout) {
        setupGridLayout();
        topPanel->setMaximumWidth(QWIDGETSIZE_MAX);
        topPanel->setMinimumWidth(1);
    }
    else {
        qCritical() << "Invalid layout " << static_cast<quint8>(newLayout);
    }

    topPanel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum));

    setProperty("horizontal", orientation == Qt::Horizontal ? true : false);
    refreshStyleSheet();

    updateGeometry();
}

void NinjamTrackGroupView::setupGridLayout()
{
    mainLayout->removeWidget(topPanel);
    mainLayout->removeItem(tracksLayout);
    mainLayout->removeWidget(videoWidget);

    int topPanelRowSpan = videoWidget->isVisible() ? 1 : mainLayout->rowCount();
    mainLayout->addWidget(topPanel, 0, 0, topPanelRowSpan, 1);

    if (videoWidget->isVisible())
        mainLayout->addWidget(videoWidget, 1, 0, 1, 1);

    mainLayout->addLayout(tracksLayout, 0, 1, mainLayout->rowCount(), 1);

    resetMainLayoutStretch();
    mainLayout->setColumnStretch(0, 1); // video is streched
    mainLayout->setRowStretch(1, 1);

    mainLayout->setSpacing(3);

    topPanelLayout->setDirection(videoWidget->isVisible() ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom);

    tracksLayout->setDirection(QBoxLayout::LeftToRight);

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));
}

void NinjamTrackGroupView::setupHorizontalLayout()
{
    mainLayout->removeWidget(topPanel);
    mainLayout->removeItem(tracksLayout);
    mainLayout->removeWidget(videoWidget);

    mainLayout->addWidget(topPanel,    0, 0, 1, 1);
    mainLayout->addLayout(tracksLayout,      0, 1, 1, 1);
    mainLayout->addWidget(videoWidget, 0, 2, 1, 1);

    mainLayout->setSpacing(0);

    resetMainLayoutStretch();
    mainLayout->setColumnStretch(1, 1); // tracks are strechted

    tracksLayout->setDirection(QBoxLayout::TopToBottom);
    topPanelLayout->setDirection(QBoxLayout::TopToBottom);

    topPanel->setVisible(true);

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));
}

void NinjamTrackGroupView::setupVerticalLayout()
{
    mainLayout->removeWidget(topPanel);
    mainLayout->removeItem(tracksLayout);
    mainLayout->removeWidget(videoWidget);

    mainLayout->addWidget(topPanel, 0, 0, 1, 1);
    mainLayout->addLayout(tracksLayout, 1, 0, 1, 1);
    mainLayout->addWidget(videoWidget, 2, 0, 1, 1);

    mainLayout->setSpacing(0);

    resetMainLayoutStretch();
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setRowStretch(1, 1); // tracks are strechted

    topPanel->setVisible(true);

    tracksLayout->setDirection(QBoxLayout::LeftToRight);
    topPanelLayout->setDirection(QBoxLayout::TopToBottom);

    setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
}

void NinjamTrackGroupView::resetMainLayoutStretch()
{
    int rows = mainLayout->rowCount();
    for (int r = 0; r < rows; ++r) {
        mainLayout->setRowStretch(r, 0);
    }

    int collumns = mainLayout->columnCount();
    for (int c = 0; c < collumns; ++c) {
        mainLayout->setColumnStretch(c, 0);
    }
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
    if (tracksLayoutEnum == TracksLayout::VerticalLayout) {
        return TrackGroupView::sizeHint();
    }
    else if (tracksLayoutEnum == TracksLayout::HorizontalLayout) {
        int height = 0;
        for (auto trackView : trackViews) {
            height += trackView->minimumSizeHint().height();
        }

        return QSize(1, qMax(height, 54));
    }

    // grid layout
    return QSize(NinjamTrackGroupView::MAX_WIDTH_IN_GRID_LAYOUT, NinjamTrackGroupView::MAX_HEIGHT_IN_GRID_LAYOUT);
}

NinjamTrackView *NinjamTrackGroupView::addTrackView(long trackID)
{
    NinjamTrackView *newTrackView = dynamic_cast<NinjamTrackView *>(TrackGroupView::addTrackView(trackID));
    newTrackView->setOrientation(getTracksOrientation());
    return newTrackView;
}

Qt::Orientation NinjamTrackGroupView::getTracksOrientation() const
{
    if (tracksLayoutEnum == TracksLayout::VerticalLayout)
        return Qt::Vertical;

    if (tracksLayoutEnum == TracksLayout::HorizontalLayout)
        return Qt::Horizontal;

    return  Qt::Vertical;
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
