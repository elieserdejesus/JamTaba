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
#include "chat/ChatPanel.h"
#include "chat/NinjamVotingMessageParser.h"
#include "MainWindow.h"
#include "log/Logging.h"
#include "persistence/UsersDataCache.h"
#include "MetronomeUtils.h"

#include <QMessageBox>
#include <QRegExp>
#include <QLocale>
#include <cassert>
#include <QComboBox>
#include <QDebug>
#include <QDateTime>
#include <QToolButton>
#include <QButtonGroup>
#include <QTimer>

using namespace Persistence;
using namespace Controller;

const QString NinjamRoomWindow::JAMTABA_CHAT_BOT_NAME("JamTaba");

NinjamRoomWindow::NinjamRoomWindow(MainWindow *mainWindow, const Login::RoomInfo &roomInfo,
                                                                MainController *mainController) :
    QWidget(mainWindow),
    ui(new Ui::NinjamRoomWindow),
    mainWindow(mainWindow),
    mainController(mainController),
    chatPanel(new ChatPanel(MainController::getBotNames(), &usersColorsPool,
                                    mainWindow ? mainWindow->createTextEditorModifier() : nullptr)),
    ninjamPanel(nullptr),
    tracksOrientation(Qt::Vertical),
    tracksSize(TracksSize::WIDE),
    roomInfo(roomInfo)
{
    qCDebug(jtNinjamGUI) << "NinjamRoomWindow::NinjamRoomWindow ctor";
    ui->setupUi(this);

    ui->licenceButton->setIcon(QIcon(QPixmap(":/images/licence.png")));

    ui->tracksPanel->layout()->setAlignment(Qt::AlignLeft); // tracks are left aligned

    this->ninjamPanel = createNinjamPanel();

    QString serverLicence = mainController->getNinjamService()->getCurrentServerLicence();
    ui->licenceButton->setVisible(!serverLicence.isEmpty());

    chatPanel->setPreferredTranslationLanguage(mainController->getSettings().getTranslation());

    Qt::Orientation lastTracksLayoutOrientation = mainController->getLastTracksLayoutOrientation();
    createLayoutDirectionButtons(lastTracksLayoutOrientation);

    TracksSize lastTracksSize = mainController->isUsingNarrowedTracks() ? TracksSize::NARROW : TracksSize::WIDE;
    createTracksSizeButtons(lastTracksSize);

    setupSignals(mainController->getNinjamController());

    // remember the last tracks layout orientation and size (narrow or wide)
    setTracksOrientation(lastTracksLayoutOrientation);
    setTracksSize(lastTracksSize);

    translate();

    initializeVotingExpirationTimers();

    updateBpmBpiLabel();

    connect(mainController, &Controller::MainController::themeChanged, this, &NinjamRoomWindow::updateStylesheet);
    qCDebug(jtNinjamGUI) << "NinjamRoomWindow::NinjamRoomWindow done";
}

void NinjamRoomWindow::updateStylesheet()
{
    if (ninjamPanel)
        ninjamPanel->updateStyleSheet();

    auto ninjamTracks = ui->tracksPanel->findChildren<NinjamTrackView *>();
    for (auto ninjamTrack : ninjamTracks) {
        ninjamTrack->updateStyleSheet();
    }
}

void NinjamRoomWindow::initializeVotingExpirationTimers()
{
    bpiVotingExpiratonTimer = new QTimer(this);
    bpmVotingExpirationTimer = new QTimer(this);
    bpiVotingExpiratonTimer->setSingleShot(true);
    bpmVotingExpirationTimer->setSingleShot(true);
    connect(bpiVotingExpiratonTimer, &QTimer::timeout, this, &NinjamRoomWindow::resetBpiComboBox);
    connect(bpmVotingExpirationTimer, &QTimer::timeout, this, &NinjamRoomWindow::resetBpmComboBox);
}

void NinjamRoomWindow::updateBpmBpiLabel()
{
    auto controller = mainController->getNinjamController();
    if (!controller)
        return;

    int bpi = controller->getCurrentBpi();
    int bpm = controller->getCurrentBpm();
    QString newText(QString::number(bpm) + " BPM   " + QString::number(bpi) + " BPI");
    ui->labelBpmBpi->setText(newText);
}

void NinjamRoomWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}

void NinjamRoomWindow::translate()
{
    ui->retranslateUi(this); // translate the fixed elements created in Qt ui designer

    // translate other elements
    horizontalLayoutButton->setToolTip(tr("Set tracks layout to horizontal"));
    verticalLayoutButton->setToolTip(tr("Set tracks layout to vertical"));
    wideButton->setToolTip(tr("Wide tracks"));
    narrowButton->setToolTip(tr("Narrow tracks"));

    updateUserNameLabel();
}

void NinjamRoomWindow::updateUserNameLabel()
{
    QString userName = mainController->getUserName();
    QString labelText = tr("Connected as %1").arg(userName);
    ui->labelUserName->setText(labelText);
}

void NinjamRoomWindow::createLayoutDirectionButtons(Qt::Orientation initialOrientation)
{
    horizontalLayoutButton = new QToolButton();
    horizontalLayoutButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    horizontalLayoutButton->setIcon(QIcon(":/images/horizontal_layout.png"));
    horizontalLayoutButton->setObjectName(QStringLiteral("buttonHorizontalLayout"));
    horizontalLayoutButton->setCheckable(true);

    verticalLayoutButton = new QToolButton();
    verticalLayoutButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    verticalLayoutButton->setIcon(QIcon(":/images/vertical_layout.png"));
    verticalLayoutButton->setObjectName(QStringLiteral("buttonVerticalLayout"));
    verticalLayoutButton->setCheckable(true);

    if(initialOrientation == Qt::Vertical)
        verticalLayoutButton->setChecked(true);
    else
        horizontalLayoutButton->setChecked(true);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(0);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->addWidget(verticalLayoutButton);
    buttonsLayout->addWidget(horizontalLayoutButton);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(verticalLayoutButton);
    buttonGroup->addButton(horizontalLayoutButton);

    int licenceButtonIndex = ui->topLayout->indexOf(ui->licenceButton);
    ui->topLayout->insertLayout(licenceButtonIndex, buttonsLayout);

    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toggleTracksLayoutOrientation(QAbstractButton*)));
}

void NinjamRoomWindow::createTracksSizeButtons(TracksSize initialTracksSize)
{
    wideButton = new QToolButton();
    wideButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    wideButton->setIcon(QIcon(":/images/wide.png"));
    wideButton->setObjectName(QStringLiteral("wide"));
    wideButton->setCheckable(true);

    narrowButton = new QToolButton();
    narrowButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    narrowButton->setIcon(QIcon(":/images/narrow.png"));
    narrowButton->setObjectName(QStringLiteral("narrow"));
    narrowButton->setCheckable(true);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(0);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->addWidget(wideButton);
    buttonsLayout->addWidget(narrowButton);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(wideButton);
    buttonGroup->addButton(narrowButton);

    if(initialTracksSize == TracksSize::NARROW)
        narrowButton->setChecked(true);
    else
        wideButton->setChecked(true);

    int licenceButtonIndex = ui->topLayout->indexOf(ui->licenceButton);
    ui->topLayout->insertLayout(licenceButtonIndex, buttonsLayout);

    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toggleTracksSize(QAbstractButton*)));
}

void NinjamRoomWindow::toggleTracksLayoutOrientation(QAbstractButton* buttonClicked)
{
    Qt::Orientation newOrientation = buttonClicked == this->verticalLayoutButton ? Qt::Vertical : Qt::Horizontal;
    setTracksOrientation(newOrientation);
}

void NinjamRoomWindow::toggleTracksSize(QAbstractButton *buttonClicked)
{
    TracksSize newTracksSize = buttonClicked == narrowButton ? TracksSize::NARROW : TracksSize::WIDE;
    setTracksSize(newTracksSize);
}

NinjamPanel *NinjamRoomWindow::createNinjamPanel()
{
    TextEditorModifier *bpiComboModifier = mainWindow ? mainWindow->createTextEditorModifier() : nullptr;
    TextEditorModifier *bpmComboModifier = mainWindow ? mainWindow->createTextEditorModifier() : nullptr;
    NinjamPanel *panel = new NinjamPanel(bpiComboModifier, bpmComboModifier);

    float initialMetronomeGain = mainController->getSettings().getMetronomeGain();
    float initialMetronomePan = mainController->getSettings().getMetronomePan();
    bool initialMetronomeMuteStatus = mainController->getSettings().getMetronomeMuteStatus();

    panel->setGainSliderValue(100 * initialMetronomeGain);
    panel->setPanSliderValue(4 * initialMetronomePan);
    panel->setMuteButtonStatus(initialMetronomeMuteStatus);
    panel->setIntervalShape(mainController->getSettings().getIntervalProgressShape());

    connect(panel, &NinjamPanel::bpiComboActivated, this, &NinjamRoomWindow::setNewBpi);
    connect(panel, &NinjamPanel::bpmComboActivated, this, &NinjamRoomWindow::setNewBpm);
    connect(panel, &NinjamPanel::accentsComboChanged, this, &NinjamRoomWindow::handleAccentBeatsComboChange);
    connect(panel, &NinjamPanel::accentsTextChanged, this, &NinjamRoomWindow::handleCustomAccentBeatsChange);

    connect(panel, &NinjamPanel::gainSliderChanged, this, &NinjamRoomWindow::setMetronomeFaderPosition);
    connect(panel, &NinjamPanel::panSliderChanged, this, &NinjamRoomWindow::setMetronomePanSliderPosition);
    connect(panel, &NinjamPanel::muteButtonClicked, this, &NinjamRoomWindow::toggleMetronomeMuteStatus);
    connect(panel, &NinjamPanel::soloButtonClicked, this, &NinjamRoomWindow::toggleMetronomeSoloStatus);
    connect(panel, &NinjamPanel::preferencesButtonClicked, this, &NinjamRoomWindow::showMetronomePreferences);

    return panel;
}

void NinjamRoomWindow::showMetronomePreferences()
{
    Q_ASSERT(mainWindow);

    mainWindow->showMetronomePreferencesDialog();
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
        chatPanel->addMessage(JAMTABA_CHAT_BOT_NAME, tr("%1 has left the room.").arg(userName));

    usersColorsPool.giveBack(userName); // reuse the color mapped to this 'leaving' user
}

void NinjamRoomWindow::handleUserEntering(const QString &userName)
{
    if (chatPanel)
        chatPanel->addMessage(JAMTABA_CHAT_BOT_NAME, tr("%1 has joined the room.").arg(userName));
}

void NinjamRoomWindow::addServerTopicMessage(const QString &topicMessage)
{
    addChatMessage(Ninjam::User(JAMTABA_CHAT_BOT_NAME), topicMessage);

    showLastChordsInChat();
}

void NinjamRoomWindow::showLastChordsInChat()
{
    Login::LoginService *loginService = mainController->getLoginService();
    QString lastChordProgression = loginService->getChordProgressionFor(roomInfo);
    ChatChordsProgressionParser parser;
    if (parser.containsProgression(lastChordProgression)) {
        ChordProgression progression = parser.parse(lastChordProgression);
        QString title = tr("Last chords used");
        chatPanel->addLastChordsMessage(title, progression.toString());
        chatPanel->addChordProgressionConfirmationMessage(parser.parse(lastChordProgression));

    }
}

void NinjamRoomWindow::addChatMessage(const Ninjam::User &user, const QString &message)
{
    QString userName = user.getName();

    bool isSystemVoteMessage = Gui::Chat::parseSystemVotingMessage(message).isValidVotingMessage();

    bool isChordProgressionMessage = false;
    if (!isSystemVoteMessage) {
        ChatChordsProgressionParser chordsParser;
        isChordProgressionMessage = chordsParser.containsProgression(message);
    }

    bool showBlockButton = canShowBlockButtonInChatMessage(userName);
    bool showTranslationButton = !isChordProgressionMessage;
    chatPanel->addMessage(userName, message, showTranslationButton, showBlockButton);

    static bool localUserWasVotingInLastMessage = false;

    if (isSystemVoteMessage) {
        Gui::Chat::SystemVotingMessage voteMessage = Gui::Chat::parseSystemVotingMessage(message);

        QTimer *expirationTimer = voteMessage.isBpiVotingMessage() ? bpiVotingExpiratonTimer : bpmVotingExpirationTimer;

        bool isFirstSystemVoteMessage = Gui::Chat::isFirstSystemVotingMessage(userName, message);
        if (isFirstSystemVoteMessage) { //starting a new votation round
            if (!localUserWasVotingInLastMessage) {  //don't create the vote button if local user is proposing BPI or BPM change
                createVoteButton(voteMessage);
            }
            else{ //if local user is proposing a bpi/bpm change the combos are disabled until the voting reach majority or expire
                if (voteMessage.isBpiVotingMessage())
                    ninjamPanel->setBpiComboPendingStatus(true);
                else
                    ninjamPanel->setBpmComboPendingStatus(true);
                if (QApplication::focusWidget()) //clear comboboxes focus when disabling
                    QApplication::focusWidget()->clearFocus();
            }
        }

        //timer is restarted in every vote
        expirationTimer->start(voteMessage.getExpirationTime() * 1000); //QTimer::start will cancel a previous voting expiration timer
    }
    else if (isChordProgressionMessage) {
        handleChordProgressionMessage(user, message);
    }

    localUserWasVotingInLastMessage = Gui::Chat::isLocalUserVotingMessage(message) && user.getName() == mainController->getUserName();
}

void NinjamRoomWindow::resetBpiComboBox()
{
    Controller::NinjamController *ninjamController = mainController->getNinjamController();
    ninjamPanel->setBpiComboPendingStatus(false);
    ninjamPanel->setBpiComboText(QString::number(ninjamController->getCurrentBpi()));
}

void NinjamRoomWindow::resetBpmComboBox()
{
    Controller::NinjamController *ninjamController = mainController->getNinjamController();
    ninjamPanel->setBpmComboPendingStatus(false);
    ninjamPanel->setBpmComboText(QString::number(ninjamController->getCurrentBpm()));
}

bool NinjamRoomWindow::canShowBlockButtonInChatMessage(const QString &userName) const
{
    /**
        Avoid the block button for bot and current user messages. Is not a good idea allow user
    to block yourself :).
        In vote messages (to change BPI or BPM) user name is empty. The last logic test is
    avoiding show block button in vote messages (fixing #389).

    **/

    bool userIsBot = mainController->getNinjamController()->userIsBot(userName) || userName == JAMTABA_CHAT_BOT_NAME;
    bool currentUserIsPostingTheChatMessage = userName == mainController->getUserName(); // chat message author and the current user name are the same?
    return !userIsBot && !currentUserIsPostingTheChatMessage && !userName.isEmpty();
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
        qCritical() << e.what();
    }
}

void NinjamRoomWindow::createVoteButton(const Gui::Chat::SystemVotingMessage &votingMessage)
{
    if (!votingMessage.isValidVotingMessage())
        return;

    quint32 voteValue = votingMessage.getVoteValue();
    quint32 expireTime = votingMessage.getExpirationTime();
    if (votingMessage.isBpiVotingMessage())
        chatPanel->addBpiVoteConfirmationMessage(voteValue, expireTime);
    else
        chatPanel->addBpmVoteConfirmationMessage(voteValue, expireTime);
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

    ninjamPanel->setMetronomePeaks(metronomePeak.getLeftPeak(),
                                   metronomePeak.getRightPeak(),
                                   metronomePeak.getLeftRMS(),
                                   metronomePeak.getRightRMS());
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
    }
}

void NinjamRoomWindow::setChannelXmitStatus(long channelID, bool transmiting)
{
    NinjamTrackView *trackView = getTrackViewByID(channelID);
    if (trackView)
        trackView->setActivatedStatus(!transmiting);
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
    }

    updateTracksSizeButtons();
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
        QColor userColor = usersColorsPool.get(user.getName());// the user channel and your chat messages are painted with same color
        NinjamTrackGroupView *trackGroupView = new NinjamTrackGroupView(mainController, channelID,
                                                                   channelName, userColor, cacheEntry);
        trackGroupView->setOrientation(tracksOrientation);
        trackGroupView->setNarrowStatus(tracksSize == TracksSize::NARROW);
        ui->tracksPanel->layout()->addWidget(trackGroupView);
        trackGroups.insert(user.getFullName(), trackGroupView);
        trackGroupView->setEstimatedChunksPerInterval(calculateEstimatedChunksPerInterval());
    } else {// the second, or third channel from same user, group with other channels
        NinjamTrackGroupView *trackGroup = trackGroups[user.getFullName()];
        if (trackGroup) {
            NinjamTrackView *ninjamTrackView = trackGroup->addTrackView(channelID);
            ninjamTrackView->setChannelName(channel.getName());
            ninjamTrackView->setInitialValues(cacheEntry);
            ninjamTrackView->setEstimatedChunksPerInterval(calculateEstimatedChunksPerInterval());
            ninjamTrackView->setActivatedStatus(true);/** disabled/grayed until receive the first bytes. When the first bytes
            are downloaded the 'on_channelXmitChanged' slot is executed and this track is enabled.*/
        }
    }

    // bind the new track view with ninjam channel data (user full name and channel index). These ninjam data is necessary to send receive on/off messages to server when user click in 'receive' button
    NinjamTrackGroupView *groupView = trackGroups[user.getFullName()];
    if (groupView != nullptr) {
        QList<NinjamTrackView *> trackViews = groupView->getTracks<NinjamTrackView*>();
        if (!trackViews.isEmpty())
            trackViews.last()->setNinjamChannelData(user.getFullName(), channel.getIndex());
    }

    qCDebug(jtNinjamGUI) << "channel view created";

    updateTracksSizeButtons();
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int NinjamRoomWindow::calculateEstimatedChunksPerInterval() const
{
    Controller::NinjamController *ninjamController = mainController->getNinjamController();
    if(ninjamController){
        int bpi = ninjamController->getCurrentBpi();
        int bpm = ninjamController->getCurrentBpm();
        if(bpm > 0){
            return (60000/bpm * bpi) / 320;
            /** 320 is just a 'almost magical empyrical (non exact)' value estimated observing how many chunks are downloaded from a remote stereo audio stream (48 KHz).
                To compute exactly how many chunks per interval we need some infos like: remote sample rate, remote is mono or stereo, bpm, bpi, and the
                quality used in the remove encoder (more quality need more bytes, and more chunks). Some of these infos are very trick to obtain, so
                I'm just using a 'good enough guess' approach. Feel free to improve!
            */
        }
    }
    return 0;
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
    Controller::NinjamController *ninjamController = mainController->getNinjamController();
    if (ninjamController) {
        disconnectFromNinjamControllerSignals(ninjamController);
    }

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
    static QRegExp ccLink("(http://creativecommons.org/licenses/\\S+/)");
    static QRegExp brPattern("(\\r\\n|\\r|\\n)");
    QString locale = QLocale::system().name();
    QString anchorTag = QStringLiteral("<a href=\"\\1deed.%1\">\\1</a>").arg(locale);

    QString licence = mainController->getNinjamService()->getCurrentServerLicence();
    QMessageBox *msgBox = new QMessageBox();

    msgBox->setWindowIcon(mainWindow->windowIcon());
    msgBox->setTextFormat(Qt::RichText);
    msgBox->setText(licence
              .replace("<", "&lt;")
              .replace(brPattern, "<br>")
              .replace(ccLink, anchorTag));
    msgBox->setWindowTitle(tr("Server Licence"));
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setWindowModality(Qt::WindowModal);

    // hack to set minimum width in QMessageBox
    QSpacerItem *horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum,
                                                    QSizePolicy::Expanding);
    QGridLayout *layout = (QGridLayout *)msgBox->layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox->raise();
    
    msgBox->show();
    msgBox->exec();
}

// ----------
void NinjamRoomWindow::handleAccentBeatsComboChange(int index)
{
    Q_UNUSED(index)
    int accentBeatsCb = ninjamPanel->getAccentBeatsComboValue();
    qCDebug(jtNinjamGUI) << "NinjamRoomWindow::handleAccentBeatsComboChange " << accentBeatsCb << index;

    if (accentBeatsCb == -1) {
        ninjamPanel->setAccentBeatsTextEnabled(true);
        // do nothing else until the Ninjam Panel UI works out the accent beats text
    } else {
        ninjamPanel->setAccentBeatsTextEnabled(false);
        int currentBpi = mainController->getNinjamController()->getCurrentBpi();
        mainController->getNinjamController()->setMetronomeBeatsPerAccent(accentBeatsCb, currentBpi);
    }
}

void NinjamRoomWindow::handleCustomAccentBeatsChange(const QString &value)
{
    Q_UNUSED(value)
    qCDebug(jtNinjamGUI) << "NinjamRoomWindow::handleCustomAccentBeatsChange " << value;

    QList<int> accentBeats = Audio::MetronomeUtils::getAccentBeatsFromString(ninjamPanel->getAccentBeatsText());
    qCDebug(jtNinjamGUI) << "NinjamRoomWindow::handleCustomAccentBeatsChange " << accentBeats;
    mainController->getNinjamController()->setMetronomeAccentBeats(accentBeats);
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
    if(!ninjamController)
        return;

    qCDebug(jtNinjamGUI) << "connecting signals in ninjamController...";
    connect(ninjamController, SIGNAL(channelAdded(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(addChannel(Ninjam::User, Ninjam::UserChannel, long)));

    connect(ninjamController, SIGNAL(channelRemoved(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(removeChannel(Ninjam::User, Ninjam::UserChannel, long)));

    connect(ninjamController, SIGNAL(channelNameChanged(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(changeChannelName(Ninjam::User, Ninjam::UserChannel, long)));

    connect(ninjamController, SIGNAL(channelAudioChunkDownloaded(long)), this, SLOT(updateIntervalDownloadingProgressBar(long)));

    connect(ninjamController, SIGNAL(channelAudioFullyDownloaded(long)), this, SLOT(hideIntervalDownloadingProgressBar(long)));

    connect(ninjamController, SIGNAL(chatMsgReceived(Ninjam::User, QString)), this, SLOT(addChatMessage(Ninjam::User, QString)));

    connect(ninjamController, SIGNAL(topicMessageReceived(QString)), this, SLOT(addServerTopicMessage(QString)));

    connect(ninjamController, SIGNAL(channelXmitChanged(long, bool)), this, SLOT(setChannelXmitStatus(long, bool)));

    connect(ninjamController, SIGNAL(userLeave(QString)), this, SLOT(handleUserLeaving(QString)));

    connect(ninjamController, SIGNAL(userEnter(QString)), this, SLOT(handleUserEntering(QString)));

    connect(ninjamController, SIGNAL(currentBpiChanged(int)), this, SLOT(handleBpiChanges()));
    connect(ninjamController, SIGNAL(currentBpmChanged(int)), this, SLOT(handleBpmChanges()));

    connect(ninjamController, &Controller::NinjamController::userBlockedInChat, this, &NinjamRoomWindow::showFeedbackAboutBlockedUserInChat);
    connect(ninjamController, &Controller::NinjamController::userUnblockedInChat, this, &NinjamRoomWindow::showFeedbackAboutUnblockedUserInChat);

    connect(chatPanel, SIGNAL(userSendingNewMessage(QString)), this, SLOT(sendNewChatMessage(QString)));

    connect(chatPanel, SIGNAL(userConfirmingVoteToBpiChange(int)), this, SLOT(voteToChangeBpi(int)));

    connect(chatPanel, SIGNAL(userConfirmingVoteToBpmChange(int)), this, SLOT(voteToChangeBpm(int)));

    connect(chatPanel, SIGNAL(userBlockingChatMessagesFrom(QString)), this, SLOT(blockUserInChat(QString)));

    connect(ui->licenceButton, SIGNAL(clicked(bool)), this, SLOT(showServerLicence()));

    connect(ninjamPanel, SIGNAL(intervalShapeChanged(int)), this, SLOT(setNewIntervalShape(int)));


    connect(mainController->getNinjamService(), &Ninjam::Service::videoIntervalCompleted, this, &NinjamRoomWindow::setVideoInterval);

}

void NinjamRoomWindow::setVideoInterval(const Ninjam::User &user, const QByteArray &encodedVideoData)
{
    NinjamTrackGroupView *group = trackGroups[user.getFullName()];
    if (group) {
        group->addVideoInterval(encodedVideoData);
    }
}

void NinjamRoomWindow::handleBpiChanges()
{
    setEstimatatedChunksPerIntervalInAllTracks();
    updateBpmBpiLabel();
    resetBpiComboBox();
}

void NinjamRoomWindow::handleBpmChanges()
{
    setEstimatatedChunksPerIntervalInAllTracks();
    updateBpmBpiLabel();
    resetBpmComboBox();
}

void NinjamRoomWindow::showFeedbackAboutBlockedUserInChat(const QString &userName)
{
    if (chatPanel)
        chatPanel->removeMessagesFrom(userName);
        chatPanel->addMessage(JAMTABA_CHAT_BOT_NAME, tr("%1 is blocked in the chat").arg(userName));
}

void NinjamRoomWindow::showFeedbackAboutUnblockedUserInChat(const QString &userName)
{
    if (chatPanel)
        chatPanel->addMessage(JAMTABA_CHAT_BOT_NAME, tr("%1 is unblocked in the chat").arg(userName));
}

void NinjamRoomWindow::blockUserInChat(const QString &userNameToBlock)
{
    Controller::NinjamController *ninjamController = mainController->getNinjamController();
    Ninjam::User user = ninjamController->getUserByName(userNameToBlock);
    if (user.getName() == userNameToBlock)
        ninjamController->blockUserInChat(user);
}

void NinjamRoomWindow::setNewIntervalShape(int newShape)
{
    mainController->storeIntervalProgressShape(newShape);
}

void NinjamRoomWindow::setEstimatatedChunksPerIntervalInAllTracks()
{
    int estimatedChunksPerInterval = calculateEstimatedChunksPerInterval();
    foreach (NinjamTrackGroupView *trackGroup, trackGroups.values()) {
        trackGroup->setEstimatedChunksPerInterval(estimatedChunksPerInterval);
    }
}

void NinjamRoomWindow::setTracksSize(TracksSize newTracksSize)
{
    if (newTracksSize == tracksSize)
        return;

    tracksSize = newTracksSize;

    bool usingNarrowedTracks = tracksSize == TracksSize::NARROW;
    foreach (NinjamTrackGroupView *trackGroupView, trackGroups) {
        trackGroupView->setNarrowStatus(usingNarrowedTracks);
    }

    mainController->storeTracksSize(usingNarrowedTracks);

    updateGeometry();
}

void NinjamRoomWindow::setTracksOrientation(Qt::Orientation newOrientation)
{
    if(newOrientation == tracksOrientation)
        return;

    tracksOrientation = newOrientation;
    foreach (NinjamTrackGroupView *group, trackGroups) {
        group->setOrientation(newOrientation);
    }

    QBoxLayout *tracksLayout = qobject_cast<QBoxLayout *>(ui->tracksPanel->layout());
    if(tracksLayout){
        if(newOrientation == Qt::Horizontal){
            tracksLayout->setDirection(QBoxLayout::TopToBottom);
            tracksLayout->setAlignment(Qt::AlignTop);
        }
        else{
            tracksLayout->setDirection(QBoxLayout::LeftToRight);
            tracksLayout->setAlignment(Qt::AlignLeft);
        }
    }

    updateGeometry();

    mainController->storeTracksLayoutOrientation(newOrientation);
}

void NinjamRoomWindow::updateTracksSizeButtons()
{
    //tracks size buttons are disabled if all users are using 2 tracks
    bool enableButtons = false;
    foreach (NinjamTrackGroupView *trackGroup, trackGroups) {
        if (trackGroup->getTracksCount() < 2){
            enableButtons = true;
            break;
        }
    }

    narrowButton->setEnabled(enableButtons);
    wideButton->setEnabled(enableButtons);
}


bool NinjamRoomWindow::metronomeFloatingWindowIsVisible() const
{
    return ninjamPanel && ninjamPanel->metronomeFloatingWindowIsVisible();
}

void NinjamRoomWindow::closeMetronomeFloatingWindow()
{
    if (metronomeFloatingWindowIsVisible()) {
        ninjamPanel->setMetronomeFloatingWindowVisibility(false);
    }
}
