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
#include <QRegExp>
#include <QLocale>
#include <cassert>
#include <QComboBox>
#include <QDebug>
#include <QDateTime>
#include <QToolButton>
#include <QButtonGroup>

using namespace Persistence;

// +++++++++++++++++++++++++
NinjamRoomWindow::NinjamRoomWindow(MainWindow *parent, const Login::RoomInfo &roomInfo,
                                   Controller::MainController *mainController) :
    QWidget(parent),
    ui(new Ui::NinjamRoomWindow),
    mainWindow(parent),
    mainController(mainController),
    chatPanel(new ChatPanel(mainController->getBotNames(), &usersColorsPool)),
    fullViewMode(true),
    ninjamPanel(nullptr),
    tracksOrientation(Qt::Vertical),
    tracksSize(TracksSize::WIDE),
    roomName(roomInfo.getName())
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

    Qt::Orientation lastTracksLayoutOrientation = mainController->getLastTracksLayoutOrientation();
    createLayoutDirectionButtons(lastTracksLayoutOrientation);

    TracksSize lastTracksSize = mainController->isUsingNarrowedTracks() ? TracksSize::NARROW : TracksSize::WIDE;
    createTracksSizeButtons(lastTracksSize);

    setupSignals(mainController->getNinjamController());

    //remember the last tracks layout orientation and size (narrow or wide)
    setTracksOrientation(lastTracksLayoutOrientation);
    setTracksSize(lastTracksSize);

    retranslate();
}

void NinjamRoomWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        retranslate();
    }
    QWidget::changeEvent(e);
}

void NinjamRoomWindow::retranslate()
{
    ui->retranslateUi(this); //translate the fixed elements created in Qt ui designer

    //translate other elements
    horizontalLayoutButton->setToolTip(tr("Set tracks layout to horizontal"));
    verticalLayoutButton->setToolTip(tr("Set tracks layout to vertical"));
    wideButton->setToolTip(tr("Wide tracks"));
    narrowButton->setToolTip(tr("Narrow tracks"));
}

void NinjamRoomWindow::createLayoutDirectionButtons(Qt::Orientation initialOrientation)
{
    horizontalLayoutButton = new QToolButton();
    horizontalLayoutButton->setIcon(QIcon(":/images/horizontal_layout.png"));
    horizontalLayoutButton->setObjectName(QStringLiteral("buttonHorizontalLayout"));
    horizontalLayoutButton->setCheckable(true);

    verticalLayoutButton = new QToolButton();
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

    connect( buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toggleTracksLayoutOrientation(QAbstractButton*)));
}

void NinjamRoomWindow::createTracksSizeButtons(TracksSize initialTracksSize)
{
    wideButton = new QToolButton();
    wideButton->setIcon(QIcon(":/images/wide.png"));
    wideButton->setObjectName(QStringLiteral("wide"));
    wideButton->setCheckable(true);

    narrowButton = new QToolButton();
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

    connect( buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toggleTracksSize(QAbstractButton*)));


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
    NinjamPanel *panel = new NinjamPanel();
    float initialMetronomeGain = mainController->getSettings().getMetronomeGain();
    float initialMetronomePan = mainController->getSettings().getMetronomePan();
    bool initialMetronomeMuteStatus = mainController->getSettings().getMetronomeMuteStatus();

    panel->setGainSliderValue(100 * initialMetronomeGain);
    panel->setPanSliderValue(4 * initialMetronomePan);
    panel->setMuteButtonStatus(initialMetronomeMuteStatus);
    panel->setIntervalShape(mainController->getSettings().getIntervalProgressShape());

    connect(panel, SIGNAL(bpiComboActivated(QString)), this,
                     SLOT(setNewBpi(QString)));
    connect(panel, SIGNAL(bpmComboActivated(QString)), this,
                     SLOT(setNewBpm(QString)));
    connect(panel, SIGNAL(accentsComboChanged(int)), this,
                     SLOT(setNewBeatsPerAccent(int)));

    connect(panel, SIGNAL(gainSliderChanged(int)), this,
                     SLOT(setMetronomeFaderPosition(int)));
    connect(panel, SIGNAL(panSliderChanged(int)), this,
                     SLOT(setMetronomePanSliderPosition(int)));
    connect(panel, SIGNAL(muteButtonClicked()), this, SLOT(toggleMetronomeMuteStatus()));
    connect(panel, SIGNAL(soloButtonClicked()), this, SLOT(toggleMetronomeSoloStatus()));
    connect(panel, SIGNAL(preferencesButtonClicked()), this, SLOT(showMetronomePreferences()));

    return panel;
}

void NinjamRoomWindow::showMetronomePreferences()
{
    Q_ASSERT(mainWindow);

    mainWindow->showMetronomePreferencesDialog();
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
        chatPanel->addMessage("JamTaba", tr("%1 leave the room.").arg(userName));

    usersColorsPool.giveBack(userName); // reuse the color mapped to this 'leaving' user
}

void NinjamRoomWindow::handleUserEntering(const QString &userName)
{
    if (chatPanel)
        chatPanel->addMessage("JamTaba", tr("%1 enter in room.").arg(userName));
}

void NinjamRoomWindow::addChatMessage(const Ninjam::User &user, const QString &message)
{
    bool isVoteMessage = !message.isNull() && message.toLower().startsWith(
        "[voting system] leading candidate:");
    bool isChordProgressionMessage = false;
    try{ //TODO - remove this try catch?
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
        NinjamTrackGroupView *trackView = new NinjamTrackGroupView(mainController, channelID,
                                                                   channelName, userColor, cacheEntry);
        trackView->setOrientation(tracksOrientation);
        trackView->setNarrowStatus(tracksSize == TracksSize::NARROW);
        ui->tracksPanel->layout()->addWidget(trackView);
        trackGroups.insert(user.getFullName(), trackView);
        trackView->setEstimatedChunksPerInterval(calculateEstimatedChunksPerInterval());
    } else {// the second, or third channel from same user, group with other channels
        NinjamTrackGroupView *trackGroup = trackGroups[user.getFullName()];
        if (trackGroup) {
            NinjamTrackView *ninjamTrackView = trackGroup->addTrackView(channelID);
            ninjamTrackView->setChannelName(channel.getName());
            ninjamTrackView->setInitialValues(cacheEntry);
            ninjamTrackView->setEstimatedChunksPerInterval(calculateEstimatedChunksPerInterval());
            ninjamTrackView->setUnlightStatus(true);/** disabled/grayed until receive the first bytes. When the first bytes
            are downloaded the 'on_channelXmitChanged' slot is executed and this track is enabled.*/
        }
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
    qCDebug(jtNinjamGUI) << "NinjamRoomWindow destructor";
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
    QMessageBox *msgBox = new QMessageBox(parentWidget());

    msgBox->setTextFormat(Qt::RichText);
    msgBox->setText(licence.replace(brPattern, "<br>").replace(ccLink, anchorTag));
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

    connect(ninjamController, SIGNAL(currentBpiChanged(int)), this, SLOT(setEstimatatedChunksPerIntervalInAllTracks()));
    connect(ninjamController, SIGNAL(currentBpmChanged(int)), this, SLOT(setEstimatatedChunksPerIntervalInAllTracks()));

    connect(chatPanel, SIGNAL(userSendingNewMessage(QString)), this, SLOT(sendNewChatMessage(QString)));

    connect(chatPanel, SIGNAL(userConfirmingVoteToBpiChange(int)), this, SLOT(voteToChangeBpi(int)));

    connect(chatPanel, SIGNAL(userConfirmingVoteToBpmChange(int)), this, SLOT(voteToChangeBpm(int)));

    connect(ui->licenceButton, SIGNAL(clicked(bool)), this, SLOT(showServerLicence()));

    connect(ninjamPanel, SIGNAL(intervalShapeChanged(int)), this, SLOT(setNewIntervalShape(int)));

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
