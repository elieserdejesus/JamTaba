#include "MainWindow.h"
#include "PreferencesDialog.h"
#include "LocalTrackView.h"
#include "NinjamRoomWindow.h"
#include "Highligther.h"
#include "PrivateServerDialog.h"
#include "NinjamController.h"
#include "Utils.h"
#include "UserNameDialog.h"
#include "log/Logging.h"
// #include "performance/PerformanceMonitor.h"

using namespace Audio;
using namespace Persistence;
using namespace Controller;
using namespace Ninjam;
using namespace Persistence;

const QSize MainWindow::MINI_MODE_MIN_SIZE = QSize(800, 600);
const QSize MainWindow::FULL_VIEW_MODE_MIN_SIZE = QSize(1180, 790);

// const int MainWindow::PERFORMANCE_MONITOR_REFRESH_TIME = 200;//in miliseconds

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainWindow::MainWindow(Controller::MainController *mainController, QWidget *parent) :
    QMainWindow(parent),
    busyDialog(0),
    mainController(mainController),
    ninjamWindow(nullptr),
    roomToJump(nullptr),
    fullViewMode(true),
    chordsPanel(nullptr)
    // lastPerformanceMonitorUpdate(0)
{
    qCInfo(jtGUI) << "Creating MainWindow...";

    ui.setupUi(this);

    setWindowTitle("Jamtaba v" + QApplication::applicationVersion());

    initializeLoginService();
    initializeMainTabWidget();
    initializeViewModeMenu();
    setupWidgets();
    setupSignals();

    qCInfo(jtGUI) << "MainWindow created!";
}

// ++++++++++++++++++++++++=
void MainWindow::initialize()
{
    timerID = startTimer(1000/50);// timer used to animate audio peaks, midi activity, public room wave audio plot, etc.

    // set window mode: mini mode or full view mode
    setFullViewStatus(mainController->getSettings().windowsWasFullViewMode());

    showBusyDialog("Loading rooms list ...");

    // initialize using last track input settings
    initializeLocalInputChannels(mainController->getSettings().getInputsSettings());
}

// ++++++++++++++++++++++++=
void MainWindow::showPeakMetersOnlyInLocalControls(bool showPeakMetersOnly)
{
    foreach (LocalTrackGroupView *channel, localGroupChannels)
        channel->setPeakMeterMode(showPeakMetersOnly);
    ui.labelSectionTitle->setVisible(!showPeakMetersOnly);

    ui.localControlsCollapseButton->setChecked(showPeakMetersOnly);
    updateLocalInputChannelsGeometry();
}

void MainWindow::updateLocalInputChannelsGeometry()
{
    if (localGroupChannels.isEmpty())
        return;

    int sizeHintWidth = ui.localTracksWidget->sizeHint().width();
    int min = sizeHintWidth + 12;
    int max = min;
    bool showingPeakMeterOnly = localGroupChannels.first()->isShowingPeakMeterOnly();
    Qt::ScrollBarPolicy scrollPolicy = Qt::ScrollBarAlwaysOff;

    // limit the local inputs widget in mini mode
    if (!showingPeakMeterOnly && !fullViewMode) {
        max = 180;
        if (min > max) {
            min = max;
            scrollPolicy = Qt::ScrollBarAlwaysOn;
        }
    }
    ui.leftPanel->setMaximumWidth(max);
    ui.leftPanel->setMinimumWidth(min);
    ui.scrollArea->setHorizontalScrollBarPolicy(scrollPolicy);

    if (!fullViewMode && localGroupChannels.count() > 1) {
        foreach (LocalTrackGroupView *trackGroup, localGroupChannels){
            trackGroup->setToNarrow();
        }
    }
}

void MainWindow::toggleLocalInputsCollapseStatus()
{
    bool isShowingPeakMetersOnly = localGroupChannels.first()->isShowingPeakMeterOnly();
    showPeakMetersOnlyInLocalControls(!isShowingPeakMetersOnly);// toggle
    updateLocalInputChannelsGeometry();
}

// ++++++++++++++++++++++++=
Persistence::LocalInputTrackSettings MainWindow::getInputsSettings() const
{
    LocalInputTrackSettings settings;
    foreach (LocalTrackGroupView *trackGroupView, localGroupChannels) {
        trackGroupView->getTracks();
        Channel channel(trackGroupView->getGroupName());
        foreach (LocalTrackView *trackView, trackGroupView->getTracks()) {
            LocalInputAudioNode *inputNode = trackView->getInputNode();
            ChannelRange inputNodeRange = inputNode->getAudioInputRange();
            int firstInput = inputNodeRange.getFirstChannel();
            int channels = inputNodeRange.getChannels();
            int midiDevice = inputNode->getMidiDeviceIndex();
            int midiChannel = inputNode->getMidiChannelIndex();
            float gain = Utils::poweredGainToLinear(inputNode->getGain());
            float boost = Utils::linearToDb(inputNode->getBoost());
            float pan = inputNode->getPan();
            bool muted = inputNode->isMuted();

            Subchannel sub(firstInput, channels, midiDevice, midiChannel, gain, boost, pan, muted);
            channel.subChannels.append(sub);
        }
        settings.channels.append(channel);
    }
    return settings;
}

// ++++++++++++++++++++++++=
void MainWindow::stopCurrentRoomStream()
{
    if (mainController->isPlayingRoomStream()) {
        long long roomID = mainController->getCurrentStreamingRoomID();
        if (roomViewPanels[roomID])
            roomViewPanels[roomID]->clearPeaks(true);
    }
    mainController->stopRoomStream();
}

void MainWindow::showMessageBox(QString title, QString text, QMessageBox::Icon icon)
{
    QMessageBox *messageBox = new QMessageBox(this);
    messageBox->setWindowTitle(title);
    messageBox->setText(text);
    messageBox->setIcon(icon);
    messageBox->setAttribute(Qt::WA_DeleteOnClose, true);
    messageBox->show();
    centerDialog(messageBox);
}

void MainWindow::handlePublicRoomStreamError(QString msg)
{
    stopCurrentRoomStream();
    showMessageBox("Error!", msg, QMessageBox::Critical);
}

// ++++++++++++++++++++++++=

void MainWindow::removeChannelsGroup(int channelIndex)
{
    if (localGroupChannels.size() > 1) {// the first channel group can't be removed
        if (channelIndex >= 0 && channelIndex < localGroupChannels.size()) {
            LocalTrackGroupView *channel = localGroupChannels.at(channelIndex);
            ui.localTracksLayout->removeWidget(channel);
            localGroupChannels.removeAt(channelIndex);

            localGroupChannels.first()->setToWide();

            channel->deleteLater();

            //TODO Refactoring: emit a signal 'localChannel removed' and catch this signal in NinjamController
            mainController->sendRemovedChannelMessage(channelIndex);
            update();
        }
        updateLocalInputChannelsGeometry();
    }
}

void MainWindow::highlightChannelGroup(int index) const
{
    if (index >= 0 && index < localGroupChannels.size())
        Highligther::getInstance()->highlight(localGroupChannels.at(index));
}

void MainWindow::addChannelsGroup(QString name)
{
    int channelIndex = localGroupChannels.size();
    addLocalChannel(channelIndex, name, true);

    if (mainController->isPlayingInNinjamRoom()) {
        mainController->sendNewChannelsNames(getChannelsNames());

        // create an encoder for this channel in next interval
        mainController->getNinjamController()->scheduleEncoderChangeForChannel(channelIndex);
    }
    updateLocalInputChannelsGeometry();
}

// ++++++++++++++++++++++++=
void MainWindow::initializeMainTabWidget()
{
    // the rooms list tab bar is not closable
    QWidget *tabBar = nullptr;
    tabBar = ui.tabWidget->tabBar()->tabButton(0, QTabBar::RightSide);// try get the tabBar in right side (Windows)
    if (!tabBar)// try get the tabBar in left side (MAC OSX)
        tabBar = ui.tabWidget->tabBar()->tabButton(0, QTabBar::LeftSide);
    if (tabBar) {
        tabBar->resize(0, 0);
        tabBar->hide();
    }

    connect(ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));
}

// ++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::updateChannelsNames()
{
    mainController->sendNewChannelsNames(getChannelsNames());
}

// This is a factory method and is overrided in derived classes to create more specific instances.
LocalTrackGroupView *MainWindow::createLocalTrackGroupView(int channelGroupIndex)
{
    return new LocalTrackGroupView(channelGroupIndex, this);
}

// ++++++++++++++++++=
LocalTrackGroupView *MainWindow::addLocalChannel(int channelGroupIndex, QString channelName,
                                                 bool createFirstSubchannel)
{
    LocalTrackGroupView *localChannel = createLocalTrackGroupView(channelGroupIndex);

    QObject::connect(localChannel, SIGNAL(nameChanged()), this, SLOT(updateChannelsNames()));

    QObject::connect(localChannel, SIGNAL(trackAdded()), this, SLOT(updateLocalInputChannelsGeometry()));

    QObject::connect(localChannel, SIGNAL(trackRemoved()), this, SLOT(updateLocalInputChannelsGeometry()));

    if (!localGroupChannels.isEmpty())// the second channel?
        localChannel->setPreparingStatus(localGroupChannels.at(0)->isPreparingToTransmit());

    localGroupChannels.append(localChannel);

    localChannel->setGroupName(channelName);
    ui.localTracksLayout->addWidget(localChannel);

    if (createFirstSubchannel)
        localChannel->addTrackView(channelGroupIndex);

    ui.localTracksWidget->updateGeometry();

    return localChannel;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::loadPresetToTrack(Persistence::Preset preset)
{
    if (preset.isValid()) {
        removeAllInputLocalTracks();
        initializeLocalInputChannels(preset.inputTrackSettings);
        QApplication::processEvents();
    }
}

void MainWindow::removeAllInputLocalTracks()
{
    while(!localGroupChannels.isEmpty()){
        LocalTrackGroupView* view = localGroupChannels.first();
        ui.localTracksLayout->removeWidget(view);
        view->deleteLater();
        localGroupChannels.pop_front();
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// this function is overrided in MainWindowStandalone to load input selections and plugins
void MainWindow::initializeLocalSubChannel(LocalTrackView *localTrackView,
                                                Persistence::Subchannel subChannel)
{
    BaseTrackView::Boost boostValue = BaseTrackView::intToBoostValue(subChannel.boost);
    localTrackView->setInitialValues(subChannel.gain, boostValue, subChannel.pan, subChannel.muted);
}

void MainWindow::initializeLocalInputChannels(LocalInputTrackSettings inputsSettings)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    qCInfo(jtGUI) << "Initializing local inputs...";
    int channelIndex = 0;
    foreach (Persistence::Channel channel, inputsSettings.channels) {
        qCInfo(jtGUI) << "\tCreating channel "<< channel.name;
        bool createFirstSubChannel = channel.subChannels.isEmpty();
        LocalTrackGroupView *channelView = addLocalChannel(channelIndex, channel.name,
                                                           createFirstSubChannel);
        foreach (Persistence::Subchannel subChannel, channel.subChannels) {
            qCInfo(jtGUI) << "\t\tCreating sub-channel ";
            LocalTrackView *subChannelView
                = dynamic_cast<LocalTrackView *>(channelView->addTrackView(channelIndex));
            initializeLocalSubChannel(subChannelView, subChannel);
        }
        channelIndex++;
    }
    if (channelIndex == 0)// no channels in settings file or no settings file...
        addLocalChannel(0, "your channel", true);
    qCInfo(jtGUI) << "Initializing local inputs done!";

    QApplication::restoreOverrideCursor();
}

void MainWindow::initializeLoginService()
{
    Login::LoginService *loginService = this->mainController->getLoginService();

    connect(loginService, SIGNAL(roomsListAvailable(QList<Login::RoomInfo>)), this,
            SLOT(refreshPublicRoomsList(QList<Login::RoomInfo>)));

    connect(loginService, SIGNAL(incompatibilityWithServerDetected()), this,
            SLOT(handleIncompatiblity()));

    connect(loginService, SIGNAL(newVersionAvailableForDownload()), this,
            SLOT(showNewVersionAvailableMessage()));

    connect(loginService, SIGNAL(errorWhenConnectingToServer(QString)), this,
            SLOT(handleServerConnectionError(QString)));
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::closeTab(int index)
{
    if (index > 0) {// the first tab is not closable
        showBusyDialog("disconnecting ...");
        if (mainController->getNinjamController()->isRunning())
            mainController->stopNinjamController();
    }
}

void MainWindow::changeTab(int index)
{
    if (index > 0) {// click in room tab?
        if (mainController->isPlayingInNinjamRoom() && mainController->isPlayingRoomStream())
            stopCurrentRoomStream();
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::showBusyDialog()
{
    showBusyDialog("");
}

void MainWindow::showBusyDialog(QString message)
{
    busyDialog.setParent(this);
    centerBusyDialog();
    busyDialog.show(message);
}

void MainWindow::hideBusyDialog()
{
    busyDialog.hide();
}

void MainWindow::centerBusyDialog()
{
    if (!busyDialog.parentWidget())
        return;
    QSize parentSize = busyDialog.parentWidget()->size();
    QSize busyDialogSize = busyDialog.size();
    int newX = parentSize.width()/2 - busyDialogSize.width()/2;
    int newY = parentSize.height()/2 - busyDialogSize.height()/2;
    busyDialog.move(newX, newY);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool MainWindow::jamRoomLessThan(Login::RoomInfo r1, Login::RoomInfo r2)
{
    return r1.getNonBotUsersCount() > r2.getNonBotUsersCount();
}

void MainWindow::handleIncompatiblity()
{
    hideBusyDialog();
    QString text = "Your Jamtaba version is not compatible with previous versions!";
    QMessageBox::warning(this, "Server : Compatibility problem", text);
    close();
}

void MainWindow::detachMainController()
{
    mainController = nullptr;
}

void MainWindow::handleServerConnectionError(QString errorMsg)
{
    hideBusyDialog();
    QMessageBox::warning(this, "Error!", "Error connecting in Jamtaba server!\n" + errorMsg);
    close();
}

void MainWindow::showNewVersionAvailableMessage()
{
    hideBusyDialog();
    QString text
        =
            "A new Jamtaba version is available for download! Please use the <a href='http://www.jamtaba.com'>new version</a>!";
    QMessageBox::information(this, "New Jamtaba version available!", text);
}

JamRoomViewPanel *MainWindow::createJamRoomViewPanel(Login::RoomInfo roomInfo)
{
    JamRoomViewPanel *newJamRoomView = new JamRoomViewPanel(roomInfo, mainController);

    connect(newJamRoomView, SIGNAL(startingListeningTheRoom(Login::RoomInfo)), this,
            SLOT(playPublicRoomStream(Login::RoomInfo)));

    connect(newJamRoomView, SIGNAL(finishingListeningTheRoom(Login::RoomInfo)), this,
            SLOT(stopPublicRoomStream(Login::RoomInfo)));

    connect(newJamRoomView, SIGNAL(enteringInTheRoom(Login::RoomInfo)), this,
            SLOT(tryEnterInRoom(Login::RoomInfo)));

    return newJamRoomView;
}

void MainWindow::refreshPublicRoomsList(QList<Login::RoomInfo> publicRooms)
{
    if (!isVisible())
        return;

    hideBusyDialog();

    qSort(publicRooms.begin(), publicRooms.end(), jamRoomLessThan);
    int index = 0;
    foreach (Login::RoomInfo roomInfo, publicRooms) {
        if (roomInfo.getType() == Login::RoomTYPE::NINJAM) {// skipping other rooms at moment
            int rowIndex = fullViewMode ? (index / 2) : (index);
            int collumnIndex = fullViewMode ? (index % 2) : 0;// use one collumn if user choosing mini view mode
            JamRoomViewPanel *roomViewPanel = roomViewPanels[roomInfo.getID()];
            if (roomViewPanel) {
                roomViewPanel->refresh(roomInfo);
                ui.allRoomsContent->layout()->removeWidget(roomViewPanel); // the widget is removed but added again
            } else {
                roomViewPanel = createJamRoomViewPanel(roomInfo);
                roomViewPanels.insert(roomInfo.getID(), roomViewPanel);
            }
            QGridLayout *layout = dynamic_cast<QGridLayout *>(ui.allRoomsContent->layout());
            layout->addWidget(roomViewPanel, rowIndex, collumnIndex);
            index++;
        }
    }

    if (mainController->isPlayingInNinjamRoom())
        this->ninjamWindow->updateGeoLocations();
    /** updating country flag and country names after refresh the public rooms list. This is necessary because the call to webservice used to get country codes and  country names is not synchronous. So, if country code and name are not cached we receive these data from the webservice after some seconds.*/
}

// +++++++++++++++++++++++++++++++++++++
void MainWindow::playPublicRoomStream(Login::RoomInfo roomInfo)
{
    // clear all plots
    foreach (JamRoomViewPanel *viewPanel, this->roomViewPanels.values())
        viewPanel->clearPeaks(roomInfo.getID() != viewPanel->getRoomInfo().getID());

    if (roomInfo.hasStream())// just in case...
        mainController->playRoomStream(roomInfo);
}

void MainWindow::stopPublicRoomStream(Login::RoomInfo roomInfo)
{
    Q_UNUSED(roomInfo)
    stopCurrentRoomStream();
}

QStringList MainWindow::getChannelsNames() const
{
    QStringList channelsNames;
    foreach (LocalTrackGroupView *channel, localGroupChannels)
        channelsNames.append(channel->getGroupName());
    return channelsNames;
}

// user trying enter in a room
void MainWindow::tryEnterInRoom(Login::RoomInfo roomInfo, QString password)
{
    // stop room stream before enter in a room
    if (mainController->isPlayingRoomStream()) {
        long long roomID = mainController->getCurrentStreamingRoomID();
        if (roomViewPanels[roomID])
            roomViewPanels[roomID]->clearPeaks(true);
        mainController->stopRoomStream();
    }

    // show the user name dialog
    if (!mainController->userNameWasChoosed()) {
        QString lastUserName = mainController->getUserName();
        UserNameDialog dialog(ui.centralWidget, lastUserName);
        centerDialog(&dialog);
        if (dialog.exec() == QDialog::Accepted) {
            QString userName = dialog.getUserName().trimmed();
            if (!userName.isEmpty()) {
                mainController->setUserName(userName);
                QString version = QApplication::applicationVersion();
                setWindowTitle("Jamtaba v" + version + " (" + userName + ")");
            } else {
                QMessageBox::warning(this, "Warning!", "Empty name is not allowed!");
            }
        } else {
            qWarning() << "name dialog canceled";
        }
    }

    if (mainController->isPlayingInNinjamRoom()) {
        // store the room to jump and wait for disconnectedFromServer event to connect in this new room
        roomToJump.reset(new Login::RoomInfo(roomInfo));
        passwordToJump = password;

        mainController->stopNinjamController();// disconnect from current ninjam server
    } else if (mainController->userNameWasChoosed()) {
        showBusyDialog("Connecting in " + roomInfo.getName() + " ...");
        mainController->enterInRoom(roomInfo, getChannelsNames(), password);
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::enterInRoom(Login::RoomInfo roomInfo)
{
    qCDebug(jtGUI) << "hidding busy dialog...";
    hideBusyDialog();

    qCDebug(jtGUI) << "creating NinjamRoomWindow...";
    ninjamWindow.reset(createNinjamWindow(roomInfo, mainController));
    QString tabName = roomInfo.getName() + " (" + QString::number(roomInfo.getPort()) + ")";
    ninjamWindow->setFullViewStatus(this->fullViewMode);
    int index = ui.tabWidget->addTab(ninjamWindow.data(), tabName);
    ui.tabWidget->setCurrentIndex(index);

    // add the chat panel in main window
    qCDebug(jtGUI) << "adding ninjam chat panel...";
    ChatPanel *chatPanel = ninjamWindow->getChatPanel();
    ui.chatTabWidget->addTab(chatPanel, "chat " + roomInfo.getName());
    QObject::connect(chatPanel, SIGNAL(userConfirmingChordProgression(
                                           ChordProgression)), this,
                     SLOT(showChordProgression(ChordProgression)));

    // add the ninjam panel in main window (bottom panel)
    qCDebug(jtGUI) << "adding ninjam panel...";
    NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
    ui.bottomPanel->layout()->removeWidget(ui.masterControlsPanel);
    dynamic_cast<QVBoxLayout *>(ui.bottomPanel->layout())->addWidget(ninjamPanel, 0,
                                                                     Qt::AlignHCenter);
    ninjamPanel->addMasterControls(ui.masterControlsPanel);
    ninjamPanel->setFullViewStatus(fullViewMode);

    // show chat area
    ui.chatArea->setVisible(true);

    ui.leftPanel->adjustSize();
    qCDebug(jtGUI) << "MainWindow::enterInRoom() done!";

    QObject::connect(mainController->getNinjamController(), SIGNAL(
                         preparedToTransmit()), this, SLOT(startTransmission()));
    QObject::connect(mainController->getNinjamController(), SIGNAL(
                         preparingTransmission()), this, SLOT(prepareTransmission()));
    QObject::connect(mainController->getNinjamController(), SIGNAL(currentBpiChanged(
                                                                       int)), this,
                     SLOT(updateBpi(int)));
    QObject::connect(mainController->getNinjamController(), SIGNAL(currentBpmChanged(
                                                                       int)), this,
                     SLOT(updateBpm(int)));
    QObject::connect(mainController->getNinjamController(), SIGNAL(intervalBeatChanged(
                                                                       int)), this,
                     SLOT(updateCurrentIntervalBeat(int)));
}

// +++++++++++++++ PREPARING TO XMIT +++++++++++
// this signal is received when ninjam controller is ready to transmit (after the 'preparing' intervals).
void MainWindow::startTransmission()
{
    setInputTracksPreparingStatus(false);// tracks are prepared to transmit
}

void MainWindow::prepareTransmission()
{
    // tracks are waiting to start transmiting
    setInputTracksPreparingStatus(true);
}

// +++++++++++++++++++++++++++++
void MainWindow::exitFromRoom(bool normalDisconnection)
{
    hideBusyDialog();

    // remove the jam room tab (the last tab)
    if (ui.tabWidget->count() > 1) {
        ui.tabWidget->widget(1)->deleteLater();// delete the room window
        ui.tabWidget->removeTab(1);
    }

    if (ui.chatTabWidget->count() > 0) {
        ui.chatTabWidget->widget(0)->deleteLater();
        ui.chatTabWidget->removeTab(0);
    }

    // remove ninjam panel from main window
    ui.bottomPanel->layout()->removeWidget(ninjamWindow->getNinjamPanel());
    dynamic_cast<QVBoxLayout *>(ui.bottomPanel->layout())->addWidget(ui.masterControlsPanel, 0,
                                                                     Qt::AlignHCenter);
    hideChordsPanel();

    ninjamWindow.reset();

    // hide chat area
    ui.chatArea->setVisible(false);

    setInputTracksPreparingStatus(false);/** reset the prepating status when user leave the room. This is specially necessary if user enter in a room and leave before the track is prepared to transmit.*/

    if (!normalDisconnection) {
        showMessageBox("Error", "Disconnected from ninjam server", QMessageBox::Warning);
    } else {
        if (roomToJump) {// waiting the disconnection to connect in a new room?
            showBusyDialog("Connecting in " + roomToJump->getName());
            mainController->enterInRoom(*roomToJump, getChannelsNames(),
                                        (passwordToJump.isNull()
                                         || passwordToJump.isEmpty()) ? "" : passwordToJump);
            roomToJump.reset();
            passwordToJump = "";
        }
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::setInputTracksPreparingStatus(bool preparing)
{
    foreach (LocalTrackGroupView *trackGroup, localGroupChannels)
        trackGroup->setPreparingStatus(preparing);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::timerEvent(QTimerEvent *)
{
    if (!mainController)
        return;

    // update local input track peaks
    foreach (TrackGroupView *channel, localGroupChannels)
        channel->updateGuiElements();

    // update metronome peaks
    if (mainController->isPlayingInNinjamRoom()) {
        // update tracks peaks
        if (ninjamWindow)
            ninjamWindow->updatePeaks();
    }

    // update cpu and memmory usage
// qint64 now = QDateTime::currentMSecsSinceEpoch();
// if(now - lastPerformanceMonitorUpdate >= PERFORMANCE_MONITOR_REFRESH_TIME){
// ui.tabWidget->setResourcesUsage(performanceMonitor.getCpuUsage(), performanceMonitor.getMemmoryUsage());
// lastPerformanceMonitorUpdate = now;
// }

    // update room stream plot
    if (mainController->isPlayingRoomStream()) {
        long long roomID = mainController->getCurrentStreamingRoomID();
        JamRoomViewPanel *roomView = roomViewPanels[roomID];
        if (roomView) {
            Audio::AudioPeak peak = mainController->getRoomStreamPeak();
            roomView->addPeak(peak.getMax());
        }
    }

    // update master peaks
    Audio::AudioPeak masterPeak = mainController->getMasterPeak();
    ui.masterMeterL->setPeak(masterPeak.getLeft());
    ui.masterMeterR->setPeak(masterPeak.getRight());
}

// ++++++++++++=
void MainWindow::resizeEvent(QResizeEvent *ev)
{
    Q_UNUSED(ev)
    if (busyDialog.isVisible())
        centerBusyDialog();
}

void MainWindow::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::WindowStateChange && mainController) {
        mainController->storeWindowSettings(isMaximized(), fullViewMode, computeLocation());

        // show only the peak meters if user is in mini mode and is not maximized or full screen
        showPeakMetersOnlyInLocalControls(!fullViewMode && width() <= MINI_MODE_MIN_SIZE.width());
    }
    QMainWindow::changeEvent(ev);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QPointF MainWindow::computeLocation() const
{
    QRect screen = QApplication::desktop()->screenGeometry();
    float x = (float)this->pos().x()/screen.width();
    float y = (float)this->pos().y()/screen.height();
    return QPointF(x, y);
}

void MainWindow::closeEvent(QCloseEvent *)
{
    if (mainController)
        mainController->storeWindowSettings(isMaximized(), fullViewMode, computeLocation());
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainWindow::~MainWindow()
{
    qCDebug(jtGUI) << "MainWindow destructor...";
    setParent(nullptr);
    if (mainController)
        mainController->stop();

    foreach (LocalTrackGroupView *groupView, this->localGroupChannels)
        groupView->detachMainControllerInSubchannels();
    mainController = nullptr;

    killTimer(timerID);
    qCDebug(jtGUI) << "Main frame timer killed!";
    qCDebug(jtGUI) << "MainWindow destructor finished.";
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::connectInPrivateServer(QString server, int serverPort, QString password)
{
    mainController->storePrivateServerSettings(server, serverPort, password);
    Login::RoomInfo roomInfo(server, serverPort, Login::RoomTYPE::NINJAM, 32, 32);
    tryEnterInRoom(roomInfo, password);
}

void MainWindow::showJamtabaIssuesWebPage()
{
    QDesktopServices::openUrl(QUrl("https://github.com/elieserdejesus/JamTaba/issues"));
}

void MainWindow::showJamtabaWikiWebPage()
{
    QDesktopServices::openUrl(QUrl("https://github.com/elieserdejesus/JamTaba/wiki"));
}

void MainWindow::showJamtabaUsersManual()
{
    QString url = "https://github.com/elieserdejesus/JamTaba/wiki/Jamtaba's-user-guide";
    QDesktopServices::openUrl(QUrl(url));
}

void MainWindow::showPrivateServerDialog()
{
    Settings settings = mainController->getSettings();
    QString server = settings.getLastPrivateServer();
    QString password = settings.getLastPrivateServerPassword();
    int port = settings.getLastPrivateServerPort();
    PrivateServerDialog *privateServerDialog
        = new PrivateServerDialog(ui.centralWidget, server, port, password);
    QObject::connect(privateServerDialog, SIGNAL(connectionAccepted(QString, int,
                                                                    QString)), this,
                     SLOT(connectInPrivateServer(QString, int, QString)));
    centerDialog(privateServerDialog);
    privateServerDialog->show();
}

void MainWindow::centerDialog(QWidget *dialog)
{
    QPoint globalPosition = mapToGlobal(dialog->parentWidget()->pos());
    QSize parentSize = dialog->parentWidget()->size();
    int x = globalPosition.x() + parentSize.width()/2 - dialog->width()/2;
    int y = globalPosition.y();
    if (dialog->height() >= parentSize.height()/2)// big dialog like preferences panel
        y += parentSize.height()/2 - dialog->height()/2;
    else // small dialogs
        y += parentSize.height()/3;

    dialog->move(dialog->mapFromGlobal(QPoint(x, y)));
}

void MainWindow::showNinjamCommunityWebPage()
{
    QDesktopServices::openUrl(QUrl("http://www.ninbot.com"));
}

void MainWindow::showNinjamOfficialWebPage()
{
    QDesktopServices::openUrl(QUrl("http://www.cockos.com/ninjam/"));
}

// preferences menu
void MainWindow::openPreferencesDialog(QAction *action)
{
    if (action == ui.actionQuit) { // TODO put the Quit action in a better place. A 'Quit' entry in the 'Preferences' menu is not good. A Quit action is not related with Preferences.
        close();
    } else {
        int initialTab = PreferencesDialog::TAB_RECORDING;
        if (action == ui.actionAudioPreferences)
            initialTab = PreferencesDialog::TAB_AUDIO;
        else if (action == ui.actionMidiPreferences)
            initialTab = PreferencesDialog::TAB_MIDI;
        else if (action == ui.actionVstPreferences)
            initialTab = PreferencesDialog::TAB_VST;

        showPreferencesDialog(initialTab);
    }
}

void MainWindow::initializeViewModeMenu()
{
    QObject::connect(ui.menuViewMode, SIGNAL(triggered(QAction *)), this,
                     SLOT(changeViewMode(QAction *)));

    QActionGroup *group = new QActionGroup(this);
    ui.actionFullView->setActionGroup(group);
    ui.actionMiniView->setActionGroup(group);
}

void MainWindow::changeViewMode(QAction *)
{
    setFullViewStatus(ui.actionFullView->isChecked());
}

void MainWindow::setFullViewStatus(bool fullViewActivated)
{
    this->fullViewMode = fullViewActivated;
    if (!fullViewActivated)// mini view
        setMinimumSize(MINI_MODE_MIN_SIZE);
    else // full view
        setMinimumSize(FULL_VIEW_MODE_MIN_SIZE);
    if (!isMaximized() && !isFullScreen()) {
        showNormal();
        resize(minimumSize());
    }

    int tabLayoutMargim = fullViewMode ? 9 : 0;
    ui.tabLayout->setContentsMargins(tabLayoutMargim, tabLayoutMargim, tabLayoutMargim,
                                     tabLayoutMargim);

    // show only the peak meters if user is in mini mode and is not maximized or full screen
    showPeakMetersOnlyInLocalControls(!fullViewMode && !isMaximized() && !isFullScreen());

    ui.chatArea->setMinimumWidth(fullViewMode ? 280 : 180); //TODO Refactoring: remove these 'Magic Numbers'

    // refresh the public rooms list
    if (!mainController->isPlayingInNinjamRoom()) {
        QList<Login::RoomInfo> roomInfos;
        foreach (JamRoomViewPanel *roomView, roomViewPanels)
            roomInfos.append(roomView->getRoomInfo());
        refreshPublicRoomsList(roomInfos);
    } else {
        if (ninjamWindow)
            ninjamWindow->setFullViewStatus(fullViewMode);
    }

    // local tracks are narrowed in mini mode if user is using more than 1 subchannel
    foreach (LocalTrackGroupView *localTrackGroup, localGroupChannels) {
        if (!fullViewMode
            && (localTrackGroup->getTracksCount() > 1 || localGroupChannels.size() > 1))
            localTrackGroup->setToNarrow();
        else
            localTrackGroup->setToWide();
    }

    ui.actionFullView->setChecked(fullViewMode);
    ui.actionMiniView->setChecked(!fullViewMode);

    int margim = fullViewMode ? 6 : 2;
    ui.bottomPanelLayout->setContentsMargins(margim, margim, margim, margim);
    ui.bottomPanelLayout->setSpacing(fullViewMode ? 6 : 2);

    if (ninjamWindow) {
        NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
        if (ninjamPanel)
            ninjamPanel->setFullViewStatus(fullViewMode);

        ChatPanel *chatPanel = ninjamWindow->getChatPanel();
        if (chatPanel)
            chatPanel->updateMessagesGeometry();
    }
}

// +++++++++++++++++++++++++++
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui.localTracksWidget && event->type() == QEvent::Resize) {
        updateLocalInputChannelsGeometry();
        return true;
    } else {
        if (target == ui.masterFader && event->type() == QEvent::MouseButtonDblClick) {
            ui.masterFader->setValue(100);
            return true;
        }
    }
    return QMainWindow::eventFilter(target, event);
}



// PRESETS STUFF
void MainWindow::resetGroupChannel(LocalTrackGroupView *group)
{
    group->resetTracksControls();
}

// ++++++++++++++++++++++++
void MainWindow::setTransmitingStatus(int channelID, bool xmitStatus)
{
    mainController->setTransmitingStatus(channelID, xmitStatus);
}

bool MainWindow::isTransmiting(int channelID) const
{
    return mainController->isTransmiting(channelID);
}

// ++++++++++++

void MainWindow::showJamtabaCurrentVersion()
{
    QString title = "About Jamtaba";
    QString text = "Jamtaba version is " + QApplication::applicationVersion();
    QMessageBox::about(this, title, text);
}

// ++++++++++++++++++
void MainWindow::setMasterFaderPosition(int value)
{
    float newGain = (float)value/ui.masterFader->maximum();
    mainController->setMasterGain(newGain);
}

// ++++++++++++++++++=
ChordsPanel *MainWindow::createChordsPanel()
{
    ChordsPanel *chordsPanel = new ChordsPanel();
    QObject::connect(chordsPanel, SIGNAL(buttonSendChordsToChatClicked()), this,
                     SLOT(on_buttonSendChordsToChatClicked()));
    return chordsPanel;
}

void MainWindow::showChordProgression(ChordProgression progression)
{
    int currentBpi = mainController->getNinjamController()->getCurrentBpi();
    if (progression.canBeUsed(currentBpi)) {
        bool needStrech = progression.getBeatsPerInterval() != currentBpi;
        if (needStrech)
            progression = progression.getStretchedVersion(currentBpi);

        if (!chordsPanel)
            chordsPanel = createChordsPanel();
        else
            chordsPanel->setVisible(true);
        chordsPanel->setChords(progression);

        // add the chord panel in top of bottom panel in main window
        dynamic_cast<QVBoxLayout *>(ui.bottomPanel->layout())->insertWidget(0, chordsPanel);
    } else {
        int measures = progression.getMeasures().size();
        QString msg = "These chords (" + QString::number(measures)
                      + " measures) can't be used in a " + QString::number(currentBpi)
                      + " bpi interval!";
        QMessageBox::warning(this, "Problem...", msg);
    }
}

void MainWindow::sendCurrentChordProgressionToChat()
{
    if (chordsPanel && mainController) {// just in case
        ChordProgression chordProgression = chordsPanel->getChordProgression();
        mainController->getNinjamController()->sendChatMessage(chordProgression.toString());
    }
}

void MainWindow::hideChordsPanel()
{
    if (chordsPanel) {
        ui.bottomPanel->layout()->removeWidget(chordsPanel);
        chordsPanel->setVisible(false);
        chordsPanel->deleteLater();
        chordsPanel = nullptr;
    }
}

// ++++++
// ninjam controller events
void MainWindow::updateBpi(int bpi)
{
    if (!ninjamWindow)
        return;
    NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
    if (!ninjamPanel)
        return;

    ninjamPanel->setBpi(bpi);
    if (chordsPanel) {
        bool bpiWasAccepted = chordsPanel->setBpi(bpi);
        if (!bpiWasAccepted)
            hideChordsPanel();
    }
}

void MainWindow::updateBpm(int bpm)
{
    NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
    if (!ninjamPanel)
        return;
    ninjamPanel->setBpm(bpm);
}

void MainWindow::updateCurrentIntervalBeat(int beat)
{
    NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
    if (!ninjamPanel)
        return;
    ninjamPanel->setCurrentBeat(beat);

    if (chordsPanel)
        chordsPanel->setCurrentBeat(beat);
}

void MainWindow::setupWidgets()
{
    ui.masterMeterL->setOrientation(PeakMeter::HORIZONTAL);
    ui.masterMeterR->setOrientation(PeakMeter::HORIZONTAL);
    ui.masterFader->installEventFilter(this);// handle double click in master fader

    ui.chatArea->setVisible(false);// hide chat area until connect in a server to play

    ui.allRoomsContent->setLayout(new QGridLayout());
    ui.allRoomsContent->layout()->setContentsMargins(0, 0, 0, 0);
    ui.allRoomsContent->layout()->setSpacing(24);

    ui.localTracksWidget->installEventFilter(this);
}

void MainWindow::setupSignals()
{
    QObject::connect(ui.menuPreferences, SIGNAL(triggered(QAction *)), this,
                     SLOT(openPreferencesDialog(QAction *)));

    QObject::connect(ui.actionNinjam_community_forum, SIGNAL(triggered(bool)), this,
                     SLOT(showNinjamCommunityWebPage()));

    QObject::connect(ui.actionNinjam_Official_Site, SIGNAL(triggered(bool)), this,
                     SLOT(showNinjamOfficialWebPage()));

    QObject::connect(ui.actionPrivate_Server, SIGNAL(triggered(bool)), this,
                     SLOT(showPrivateServerDialog()));

    QObject::connect(ui.actionReportBugs, SIGNAL(triggered(bool)), this,
                     SLOT(showJamtabaIssuesWebPage()));

    QObject::connect(ui.actionWiki, SIGNAL(triggered(bool)), this,
                     SLOT(showJamtabaWikiWebPage()));

    QObject::connect(ui.actionUsersManual, SIGNAL(triggered(bool)), this,
                     SLOT(showJamtabaUsersManual()));

    QObject::connect(ui.actionCurrentVersion, SIGNAL(triggered(bool)), this,
                     SLOT(showJamtabaCurrentVersion()));

    QObject::connect(ui.localControlsCollapseButton, SIGNAL(clicked()), this,
                     SLOT(toggleLocalInputsCollapseStatus()));

    QObject::connect(mainController->getRoomStreamer(), SIGNAL(error(QString)), this,
                     SLOT(handlePublicRoomStreamError(QString)));

    QObject::connect(ui.masterFader, SIGNAL(valueChanged(int)), this,
                     SLOT(setMasterFaderPosition(int)));

    QObject::connect(ui.actionQuit, SIGNAL(triggered(bool)), this, SLOT(close()));
}
