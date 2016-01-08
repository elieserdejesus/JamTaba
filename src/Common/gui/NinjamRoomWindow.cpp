#include "NinjamRoomWindow.h"
#include "ui_NinjamRoomWindow.h"
#include "NinjamTrackView.h"
#include "NinjamTrackGroupView.h"

#include "ninjam/Server.h"
#include "ninjam/User.h"
#include "ninjam/UserChannel.h"
#include "audio/MetronomeTrackNode.h"
#include "audio/core/AudioDriver.h"
#include "audio/NinjamTrackNode.h"
#include "NinjamController.h"
#include "MainController.h"
#include "ninjam/Service.h"
#include "chords/ChordsPanel.h"
#include "chords/ChordProgression.h"
#include "chords/ChatChordsProgressionParser.h"
#include "NinjamPanel.h"
#include "ChatPanel.h"
#include "MainWindow.h"
#include "log/Logging.h"
#include "persistence/UsersDataCache.h"

#include <QMessageBox>
#include <cassert>
#include <QComboBox>
#include <QDebug>
#include <QDateTime>

using namespace Persistence;

// +++++++++++++++++++++++++
NinjamRoomWindow::NinjamRoomWindow(MainWindow *parent, const Login::RoomInfo &roomInfo,
                                   Controller::MainController *mainController) :
    QWidget(parent),
    ui(new Ui::NinjamRoomWindow),
    mainController(mainController),
    chatPanel(new ChatPanel(this, mainController->getBotNames())),
    fullViewMode(true),
    ninjamPanel(nullptr)
{
    qCDebug(jtNinjamGUI) << "NinjamRoomWindow construtor..";
    ui->setupUi(this);

    ui->licenceButton->setIcon(QIcon(QPixmap(":/images/licence.png")));

    QString labelText = roomInfo.getName() + " (" + QString::number(roomInfo.getPort()) + ")";
    ui->labelRoomName->setText(labelText);

    ui->tracksPanel->layout()->setAlignment(Qt::AlignLeft);// tracks are left aligned

    this->ninjamPanel = createNinjamPanel();

    QString serverLicence = mainController->getNinjamService()->getCurrentServerLicence();
    ui->licenceButton->setVisible(!serverLicence.isEmpty());

    chatPanel->setPreferredTranslationLanguage(mainController->getSettings().getTranslation());

    setupSignals(mainController->getNinjamController());
}

NinjamPanel *NinjamRoomWindow::createNinjamPanel()
{
    NinjamPanel *panel = new NinjamPanel();
    float initialMetronomeGain = mainController->getSettings().getMetronomeGain();
    float initialMetronomePan = mainController->getSettings().getMetronomePan();
    bool initialMetronomeMuteStatus = mainController->getSettings().getMetronomeMuteStatus();

    panel->setGainSliderValue(100 * initialMetronomeGain);
    panel->setPanSliderValue(4 * initialMetronomePan);
    panel->setMuteButtonStatus(initialMetronomeMuteStatus);
    panel->setIntervalShape(mainController->getSettings().getIntervalProgressShape());

    QObject::connect(panel, SIGNAL(bpiComboActivated(QString)), this,
                     SLOT(setNewBpi(QString)));
    QObject::connect(panel, SIGNAL(bpmComboActivated(QString)), this,
                     SLOT(setNewBpm(QString)));
    QObject::connect(panel, SIGNAL(accentsComboChanged(int)), this,
                     SLOT(setNewBeatsPerAccent(int)));

    QObject::connect(panel, SIGNAL(gainSliderChanged(int)), this,
                     SLOT(setMetronomeFaderPosition(int)));
    QObject::connect(panel, SIGNAL(panSliderChanged(int)), this,
                     SLOT(setMetronomePanSliderPosition(int)));
    QObject::connect(panel, SIGNAL(muteButtonClicked()), this, SLOT(toggleMetronomeMuteStatus()));
    QObject::connect(panel, SIGNAL(soloButtonClicked()), this, SLOT(toggleMetronomeSoloStatus()));

    return panel;
}

// +++++++++=
void NinjamRoomWindow::setFullViewStatus(bool fullView)
{
    if (fullView == this->fullViewMode)
        return;
    this->fullViewMode = fullView;
    int contentMargin = fullView ? 9 : 3;
    ui->contentLayout->setContentsMargins(contentMargin, 6, contentMargin, 0);
    ui->contentLayout->setSpacing(fullView ? 24 : 6);

    // main layout
    int topMargim = fullView ? 9 : 0;
    int bottomMargim = fullView ? 9 : 3;
    layout()->setContentsMargins(0, topMargim, 0, bottomMargim);

    ui->tracksPanel->layout()->setSpacing(fullView ? 6 : 3);

    foreach (NinjamTrackGroupView *trackGroup, trackGroups.values())
        trackGroup->setNarrowStatus(!fullView);

    update();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::updateGeoLocations()
{
    foreach (NinjamTrackGroupView *trackGroup, trackGroups)
        trackGroup->updateGeoLocation();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NinjamRoomWindow::setMetronomePanSliderPosition(int value)
{
    float sliderValue = value/(float)ninjamPanel->getPanSliderMaximumValue();
    mainController->setTrackPan(Controller::NinjamController::METRONOME_TRACK_ID, sliderValue);
}

void NinjamRoomWindow::setMetronomeFaderPosition(int value)
{
    mainController->setTrackGain(Controller::NinjamController::METRONOME_TRACK_ID, value/100.0);
}

void NinjamRoomWindow::toggleMetronomeMuteStatus()
{
    mainController->setTrackMute(Controller::NinjamController::METRONOME_TRACK_ID, !mainController->trackIsMuted(
                                     Controller::NinjamController::METRONOME_TRACK_ID));
}

void NinjamRoomWindow::toggleMetronomeSoloStatus()
{
    mainController->setTrackSolo(Controller::NinjamController::METRONOME_TRACK_ID, !mainController->trackIsSoloed(
                                     Controller::NinjamController::METRONOME_TRACK_ID));
}

void NinjamRoomWindow::sendNewChatMessage(const QString &msg)
{
    mainController->getNinjamController()->sendChatMessage(msg);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::handleUserLeaving(const QString &userName)
{
    if (chatPanel)
        chatPanel->addMessage("Jamtaba", userName + " leave the room.");
}

void NinjamRoomWindow::handleUserEntering(const QString &userName)
{
    if (chatPanel)
        chatPanel->addMessage("Jamtaba", userName + " enter in room.");
}

void NinjamRoomWindow::addChatMessage(const Ninjam::User &user, const QString &message)
{
    bool isVoteMessage = !message.isNull() && message.toLower().startsWith(
        "[voting system] leading candidate:");
    bool isChordProgressionMessage = false;
    try{
        ChatChordsProgressionParser chordsParser;
        isChordProgressionMessage = chordsParser.containsProgression(message);
    }
    catch (...) {
        isChordProgressionMessage = false;// just in case
    }

    bool showTranslationButton = !isChordProgressionMessage;
    chatPanel->addMessage(user.getName(), message, showTranslationButton);

    if (isVoteMessage)
        handleVoteMessage(user, message);
    else if (isChordProgressionMessage)
        handleChordProgressionMessage(user, message);
}

void NinjamRoomWindow::handleChordProgressionMessage(const Ninjam::User &user, const QString &message)
{
    Q_UNUSED(user)
    ChatChordsProgressionParser parser;
    try{
        ChordProgression chordProgression = parser.parse(message);
        chatPanel->addChordProgressionConfirmationMessage(chordProgression);
    }
    catch (const std::runtime_error &e) {
        qCCritical(jtNinjamGUI) << e.what();
    }
}

void NinjamRoomWindow::handleVoteMessage(const Ninjam::User &user, const QString &message)
{
    // local user is voting?
    static quint64 lastVoteCommand = 0;
    QString localUserFullName = mainController->getNinjamService()->getConnectedUserName();
    if (user.getFullName() == localUserFullName && message.toLower().contains("!vote"))
        lastVoteCommand = QDateTime::currentMSecsSinceEpoch();
    quint64 timeSinceLastVote = QDateTime::currentMSecsSinceEpoch() - lastVoteCommand;
    if (timeSinceLastVote >= 1000) {
        QString commandType = (message.toLower().contains("bpm")) ? "BPM" : "BPI";

        // [voting system] leading candidate: 1/2 votes for 12 BPI [each vote expires in 60s]
        int forIndex = message.indexOf("for");
        assert(forIndex >= 0);
        int spaceAfterValueIndex = message.indexOf(" ", forIndex + 4);
        QString voteValueString = message.mid(forIndex + 4, spaceAfterValueIndex - (forIndex + 4));
        int voteValue = voteValueString.toInt();

        if (commandType == "BPI")
            chatPanel->addBpiVoteConfirmationMessage(voteValue);
        else if (commandType == "BPM")// just in case
            chatPanel->addBpmVoteConfirmationMessage(voteValue);
    }
}

void NinjamRoomWindow::voteToChangeBpi(int newBpi)
{
    if (mainController->isPlayingInNinjamRoom()) {
        Controller::NinjamController *controller = mainController->getNinjamController();
        if (controller)
            controller->voteBpi(newBpi);
    }
}

void NinjamRoomWindow::voteToChangeBpm(int newBpm)
{
    if (mainController->isPlayingInNinjamRoom()) {
        Controller::NinjamController *controller = mainController->getNinjamController();
        if (controller)
            controller->voteBpm(newBpm);
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::updatePeaks()
{
    foreach (NinjamTrackGroupView *view, trackGroups) {
        if (view)
            view->updateGuiElements();
    }
    Audio::AudioPeak metronomePeak = mainController->getTrackPeak(
        Controller::NinjamController::METRONOME_TRACK_ID);
    ninjamPanel->setMetronomePeaks(metronomePeak.getLeft(), metronomePeak.getRight());
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NinjamRoomWindow::hideIntervalDownloadingProgressBar(long trackID)
{
    NinjamTrackView *trackView = getTrackViewByID(trackID);
    if (trackView)
        trackView->finishCurrentDownload();
}

void NinjamRoomWindow::updateIntervalDownloadingProgressBar(long trackID)
{
    NinjamTrackNode *node = dynamic_cast<NinjamTrackNode *>(mainController->getTrackNode(trackID));
    NinjamTrackView *trackView = getTrackViewByID(trackID);
    if (node && trackView) {
        bool waitingToPlayTheFirstInterval = !node->isPlaying();
        if (waitingToPlayTheFirstInterval)
            trackView->incrementDownloadedChunks();
        trackView->setDownloadedChunksDisplayVisibility(waitingToPlayTheFirstInterval);
    }
}

void NinjamRoomWindow::setChannelXmitStatus(long channelID, bool transmiting)
{
    qCDebug(jtNinjamGUI) << "channel xmit changed:" << channelID << " state:" << transmiting;
    NinjamTrackView *trackView = getTrackViewByID(channelID);
    if (trackView)
        trackView->setUnlightStatus(!transmiting);
}

void NinjamRoomWindow::removeChannel(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID)
{
    qCDebug(jtNinjamGUI) << "channel removed:" << channel.getName();
    Q_UNUSED(channel);

    NinjamTrackGroupView *group = trackGroups[user.getFullName()];
    if (group) {
        if (group->getTracksCount() == 1) {// removing the last track, the group is removed too
            trackGroups.remove(user.getFullName());
            ui->tracksPanel->layout()->removeWidget(group);
            group->deleteLater();
        } else {// remove one subchannel
            BaseTrackView *trackView = BaseTrackView::getTrackViewByID(channelID);
            if (trackView)
                group->removeTrackView(trackView);
        }
        adjustTracksWidth();
    }
}

NinjamTrackView *NinjamRoomWindow::getTrackViewByID(long trackID)
{
    return dynamic_cast<NinjamTrackView *>(NinjamTrackView::getTrackViewByID(trackID));
}

void NinjamRoomWindow::changeChannelName(const Ninjam::User &, const Ninjam::UserChannel &channel, long channelID)
{
    qCDebug(jtNinjamGUI) << "channel name changed:" << channel.getName();
    NinjamTrackView *trackView = getTrackViewByID(channelID);
    if (trackView)
        trackView->setChannelName(channel.getName());
}

void NinjamRoomWindow::addChannel(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID)
{
    qCDebug(jtNinjamGUI) << "channel added - creating channel view:" << user.getFullName() << " "
                         << channel.getName();

    // remembering user track controls (gain, mute, pan, boost)
    UsersDataCache *cache = mainController->getUsersDataCache();

    // a empty/default cacheEntry is returned if the user is not cached
    CacheEntry cacheEntry
        = cache->getUserCacheEntry(user.getIp(), user.getName(),
                                   static_cast<quint8>(channel.getIndex()));

    if (!trackGroups.contains(user.getFullName())) {// first channel from this user?
        QString channelName = channel.getName();
        NinjamTrackGroupView *trackView = new NinjamTrackGroupView(ui->tracksPanel,
                                                                   this->mainController, channelID,
                                                                   channelName, cacheEntry);
        trackView->setNarrowStatus(!fullViewMode);
        ui->tracksPanel->layout()->addWidget(trackView);
        trackGroups.insert(user.getFullName(), trackView);
        adjustTracksWidth();
    } else {// the second, or third channel from same user, group with other channels
        NinjamTrackGroupView *trackGroup = trackGroups[user.getFullName()];
        if (trackGroup) {
            NinjamTrackView *ninjamTrackView
                = dynamic_cast<NinjamTrackView *>(trackGroup->addTrackView(channelID));
            ninjamTrackView->setChannelName(channel.getName());
            ninjamTrackView->setInitialValues(cacheEntry);
            ninjamTrackView->setUnlightStatus(true);/** disabled/grayed until receive the first bytes. When the first bytes
            are downloaded the 'on_channelXmitChanged' slot is executed and this track is enabled.*/
        }
    }
    qCDebug(jtNinjamGUI) << "channel view created";
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::adjustTracksWidth()
{
    if (!fullViewMode)// in mini view mode tracks are always narrowed
        return;

    int availableWidth = ui->scrollArea->width();

    // can use wide to all tracks?
    int wideWidth = 0;
    foreach (NinjamTrackGroupView *trackGroup, trackGroups) {
        int subTracks = trackGroup->getTracksCount();
        wideWidth
            += (subTracks
                > 1) ? BaseTrackView::WIDE_WIDTH : BaseTrackView::NARROW_WIDTH * subTracks;
    }
    bool canUseWideTracks = wideWidth <= availableWidth;
    foreach (NinjamTrackGroupView *trackGroup, trackGroups)
        trackGroup->setNarrowStatus(!canUseWideTracks);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::disconnectFromNinjamControllerSignals(Controller::NinjamController* ninjamController){
    if(!ninjamController){//just in case
        return;
    }
    disconnect(ninjamController, SIGNAL(channelAdded(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(addChannel(Ninjam::User, Ninjam::UserChannel, long)));

    disconnect(ninjamController, SIGNAL(channelRemoved(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(removeChannel(Ninjam::User, Ninjam::UserChannel, long)));

    disconnect(ninjamController, SIGNAL(channelNameChanged(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(changeChannelName(Ninjam::User, Ninjam::UserChannel, long)));

    disconnect(ninjamController, SIGNAL(channelAudioChunkDownloaded(long)), this, SLOT(updateIntervalDownloadingProgressBar(long)));

    disconnect(ninjamController, SIGNAL(channelAudioFullyDownloaded(long)), this, SLOT(hideIntervalDownloadingProgressBar(long)));

    disconnect(ninjamController, SIGNAL(chatMsgReceived(Ninjam::User, QString)), this, SLOT(addChatMessage(Ninjam::User, QString)));

    disconnect(ninjamController, SIGNAL(channelXmitChanged(long, bool)), this, SLOT(setChannelXmitStatus(long, bool)));
}

NinjamRoomWindow::~NinjamRoomWindow()
{
    qCDebug(jtNinjamGUI) << "NinjamRoomWindow destructor";
    Controller::NinjamController *ninjamController = mainController->getNinjamController();
    if (ninjamController) {
        disconnectFromNinjamControllerSignals(ninjamController);
    }

    mainController->storeIntervalProgressShape(ninjamPanel->getIntervalShape());

    if (ninjamPanel) {
        ninjamPanel->setParent(0);
        ninjamPanel->deleteLater();
        ninjamPanel = nullptr;
    }

    delete ui;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NinjamRoomWindow::showServerLicence()
{
    QString licence = mainController->getNinjamService()->getCurrentServerLicence();
    QMessageBox *msgBox = new QMessageBox(parentWidget());
    msgBox->setText(licence);
    msgBox->setWindowTitle(ui->labelRoomName->text());
    msgBox->setAttribute(Qt::WA_DeleteOnClose);

    // hack to set minimum width in QMessageBox
    QSpacerItem *horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum,
                                                    QSizePolicy::Expanding);
    QGridLayout *layout = (QGridLayout *)msgBox->layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    msgBox->show();

    QPoint basePosition = mapToGlobal(parentWidget()->pos());    // parent widget is QTabWidget;
    int x = basePosition.x() + parentWidget()->width()/2 - msgBox->width()/2;
    int y = basePosition.y() + parentWidget()->height()/2 - msgBox->height()/2;
    msgBox->move(x, y);
}

// ----------
void NinjamRoomWindow::setNewBeatsPerAccent(int index)
{
    Q_UNUSED(index)
    int beatsPerAccent = ninjamPanel->getCurrentBeatsPerAccent();
    mainController->getNinjamController()->setMetronomeBeatsPerAccent(beatsPerAccent);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::setNewBpi(const QString &newText)
{
    int currentBpi = mainController->getNinjamController()->getCurrentBpi();
    int newBpi = newText.toInt();
    if (newBpi == currentBpi)
        return;

    mainController->getNinjamController()->voteBpi(newBpi);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::setNewBpm(const QString &newText)
{
    int currentBpm = mainController->getNinjamController()->getCurrentBpm();
    int newBpm = newText.toInt();
    if (newBpm == currentBpm)
        return;

    mainController->getNinjamController()->voteBpm(newBpm);
}

void NinjamRoomWindow::setupSignals(Controller::NinjamController* ninjamController)
{
    Q_ASSERT(ninjamController);
    qCDebug(jtNinjamGUI) << "connecting signals in ninjamController...";
    connect(ninjamController, SIGNAL(channelAdded(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(addChannel(Ninjam::User, Ninjam::UserChannel, long)));

    connect(ninjamController, SIGNAL(channelRemoved(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(removeChannel(Ninjam::User, Ninjam::UserChannel, long)));

    connect(ninjamController, SIGNAL(channelNameChanged(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(changeChannelName(Ninjam::User, Ninjam::UserChannel, long)));

    connect(ninjamController, SIGNAL(channelAudioChunkDownloaded(long)), this, SLOT(updateIntervalDownloadingProgressBar(long)));

    connect(ninjamController, SIGNAL(channelAudioFullyDownloaded(long)), this, SLOT(hideIntervalDownloadingProgressBar(long)));

    connect(ninjamController, SIGNAL(chatMsgReceived(Ninjam::User, QString)), this, SLOT(addChatMessage(Ninjam::User, QString)));

    connect(ninjamController, SIGNAL(channelXmitChanged(long, bool)), this, SLOT(setChannelXmitStatus(long, bool)));

    connect(ninjamController, SIGNAL(userLeave(QString)), this, SLOT(handleUserLeaving(QString)));

    connect(ninjamController, SIGNAL(userEnter(QString)), this, SLOT(handleUserEntering(QString)));

    connect(chatPanel, SIGNAL(userSendingNewMessage(QString)), this, SLOT(sendNewChatMessage(QString)));

    connect(chatPanel, SIGNAL(userConfirmingVoteToBpiChange(int)), this, SLOT(voteToChangeBpi(int)));

    connect(chatPanel, SIGNAL(userConfirmingVoteToBpmChange(int)), this, SLOT(voteToChangeBpm(int)));

    connect(ui->licenceButton, SIGNAL(clicked(bool)), this, SLOT(showServerLicence()));
}
