#include "MainWindow.h"

#include <QCloseEvent>
#include <QDebug>
#include <QDesktopWidget>
#include <QLayout>
#include <QList>
#include <QAction>
#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QFuture>
#include <QDesktopServices>

#include "PreferencesDialog.h"
#include "JamRoomViewPanel.h"
#include "LocalTrackView.h"
#include "plugins/Guis.h"
#include "FxPanel.h"
#include "PluginScanDialog.h"
#include "NinjamRoomWindow.h"
#include "Highligther.h"
#include "ChatPanel.h"
#include "BusyDialog.h"
#include "PrivateServerDialog.h"
// #include "../performance/PerformanceMonitor.h"

#include "NinjamController.h"
#include "ninjam/Server.h"
#include "persistence/Settings.h"
#include "audio/core/AudioDriver.h"
#include "audio/vst/PluginFinder.h"
#include "audio/core/PluginDescriptor.h"
#include "midi/MidiDriver.h"
#include "MainController.h"
#include "loginserver/LoginService.h"
#include "Utils.h"
#include "audio/RoomStreamerNode.h"
#include "UserNameDialog.h"

#include "log/Logging.h"
#include <QShortcut>

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
    pluginScanDialog(nullptr),
    ninjamWindow(nullptr),
    roomToJump(nullptr),
    fullViewMode(true),
    fullScreenViewMode(false),
    chordsPanel(nullptr)
    // lastPerformanceMonitorUpdate(0)
{
    qCInfo(jtGUI) << "Creating MainWindow...";

    ui.setupUi(this);

    setWindowTitle("Jamtaba v" + QApplication::applicationVersion());

    initializeLoginService();
    // initializePluginFinder(); //called in derived classes
    initializeMainTabWidget();
    initializeViewModeMenu();

    ui.masterMeterL->setOrientation(PeakMeter::HORIZONTAL);
    ui.masterMeterR->setOrientation(PeakMeter::HORIZONTAL);
    QObject::connect(ui.masterFader, SIGNAL(valueChanged(int)), this,
                     SLOT(setMasterFaderPosition(int)));
    ui.masterFader->installEventFilter(this);// handle double click in master fader

    timerID = startTimer(1000/50);

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

    initializeLocalInputChannels();

    ui.chatArea->setVisible(false);// hide chat area until connect in a server to play

    ui.allRoomsContent->setLayout(new QGridLayout());
    ui.allRoomsContent->layout()->setContentsMargins(0, 0, 0, 0);
    ui.allRoomsContent->layout()->setSpacing(24);

    //foreach (LocalTrackGroupView *channel, localGroupChannels)
      //  channel->refreshInputSelectionNames();

    initializeWindowState();// window size, maximization ...

    ui.localTracksWidget->installEventFilter(this);

    QObject::connect(ui.actionQuit, SIGNAL(triggered(bool)), this, SLOT(close()));

    showBusyDialog("Loading rooms list ...");
    qCInfo(jtGUI) << "MainWindow created!";
}

// ++++++++++++++++++++++++=
void MainWindow::initializePluginFinder()
{
    Vst::PluginFinder *pluginFinder = mainController->getPluginFinder();
    if (pluginFinder) {
        QObject::connect(pluginFinder, SIGNAL(scanStarted()), this, SLOT(showPluginScanDialog()));
        QObject::connect(pluginFinder, SIGNAL(scanFinished(bool)), this,
                         SLOT(hidePluginScanDialog(bool)));
        QObject::connect(pluginFinder, SIGNAL(badPluginDetected(QString)), this,
                         SLOT(addPluginToBlackList(QString)));
        QObject::connect(pluginFinder, SIGNAL(pluginScanFinished(QString, QString,
                                                                 QString)), this,
                         SLOT(addFoundedPlugin(QString, QString, QString)));
        QObject::connect(pluginFinder, SIGNAL(pluginScanStarted(QString)), this,
                         SLOT(setCurrentScanningPlugin(QString)));
    }
}

// ++++++++++++++++++++++++=
// void MainWindow::on_newThemeSelected(QAction *a){
// QString css = a->data().toString();
// mainController->configureStyleSheet(css);
// }

// ++++++++++++++++++++++++=
void MainWindow::showPeakMetersOnlyInLocalControls(bool showPeakMetersOnly)
{
    foreach (LocalTrackGroupView *channel, localGroupChannels) {
        // channel->togglePeakMeterOnlyMode();
        channel->setPeakMeterMode(showPeakMetersOnly);
    }
    ui.labelSectionTitle->setVisible(!showPeakMetersOnly);

    ui.localControlsCollapseButton->setChecked(showPeakMetersOnly);
    updateLocalInputChannelsGeometry();
}

void MainWindow::updateLocalInputChannelsGeometry()
{
    int min = ui.localTracksWidget->sizeHint().width() + 12;
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
}

void MainWindow::toggleLocalInputsCollapseStatus()
{
    bool isShowingPeakMetersOnly = localGroupChannels.first()->isShowingPeakMeterOnly();
    showPeakMetersOnlyInLocalControls(!isShowingPeakMetersOnly);// toggle
    updateLocalInputChannelsGeometry();
}

// ++++++++++++++++++++++++=
Persistence::InputsSettings MainWindow::getInputsSettings() const
{
    InputsSettings settings;
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
    // if (isRunningAsVstPlugin())
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
    addLocalChannel(channelIndex, name, true, true);// create the first subchannel AND initialize as no input

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

LocalTrackGroupView *MainWindow::addLocalChannel(int channelGroupIndex, QString channelName,
                                                 bool createFirstSubchannel,
                                                 bool initializeAsNoInput)
{
    LocalTrackGroupView *localChannel = new LocalTrackGroupView(channelGroupIndex, this);
    QObject::connect(localChannel, SIGNAL(nameChanged()), this, SLOT(updateChannelsNames()));
    QObject::connect(localChannel, SIGNAL(trackAdded()), this,
                     SLOT(updateLocalInputChannelsGeometry()));
    QObject::connect(localChannel, SIGNAL(trackRemoved()), this,
                     SLOT(updateLocalInputChannelsGeometry()));

    if (!localGroupChannels.isEmpty())// the second channel?
        localChannel->setPreparingStatus(localGroupChannels.at(0)->isPreparingToTransmit());

    localGroupChannels.append(localChannel);

    localChannel->setGroupName(channelName);
    ui.localTracksLayout->addWidget(localChannel);

    if (createFirstSubchannel) {
        localChannel->addTrackView(channelGroupIndex);
    }

    if (!fullViewMode && localGroupChannels.count() > 1) {
        foreach (LocalTrackGroupView *trackGroup, localGroupChannels)
            trackGroup->setToNarrow();
    }

    return localChannel;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// USED BY PRESETS
void MainWindow::loadPresetToTrack()
{
    // we gonna assign each group of the console surface

    int groupSize = localGroupChannels.size();
    qCDebug(jtConfigurator) << "************PRESET LOADING ***********";

    qCInfo(jtConfigurator) << "Initializing ControlSurface...";
    qCInfo(jtConfigurator) << "Number of groups in controlSurface :"<<groupSize;

    QList< LocalTrackView * > tracks;
    Persistence::PresetsSettings preset = mainController->getSettings().getPresetSettings();
    // now the preset's group count ;
    int PRST_CH_COUNT = preset.channels.size();
    qCInfo(jtConfigurator) << "Number of groups in Preset :"<<PRST_CH_COUNT;

    // if there is more groups in the preset
    if (groupSize < PRST_CH_COUNT) {
        int count = PRST_CH_COUNT-groupSize;
        qCInfo(jtConfigurator) << "Creating :"<<count<<" group";

        for (int i = 0; i < count; i++) {
            addLocalChannel(0, " new Group", true, true);
            groupSize++;
            qCInfo(jtConfigurator) << "Group size is now :"<<groupSize<<" group";
            // groupSize++;
        }
    } else if (groupSize > PRST_CH_COUNT) {
        int count = groupSize-PRST_CH_COUNT;
        qCInfo(jtConfigurator) << "removing :"<<count<<" group";

        for (int i = 0; i < count; i++) {
            removeChannelsGroup(count);
            groupSize--;
            qCInfo(jtConfigurator) << "Group size is now :"<<groupSize<<" group";
        }
    }

    // LOOP inside the controlSurfaceJTB groups
    for (int group = 0; group < groupSize; group++) {
        // load the name of the group
        qCInfo(jtConfigurator) << "......................................";
        localGroupChannels.at(group)->setGroupName(preset.channels.at(group).name);
        // get the tracks of that group
        tracks = localGroupChannels.at(group)->getTracks();
        int tracksCount = tracks.size();
        qCInfo(jtConfigurator) << "Loading group :"<<group;
        qCInfo(jtConfigurator) << "Number of tracks in group :"<<tracksCount;

        // compute tracks to create ( if any ) in that group
        int TRK_TO_CREATE = 0;
        int PRST_TRK_COUNT = preset.channels.at(group).subChannels.size();
        qCInfo(jtConfigurator) << "Number of tracks in preset :"<<PRST_TRK_COUNT;

        // ADD OR DELETE TRACKS
        if (tracksCount < PRST_TRK_COUNT) {// must create a track
            TRK_TO_CREATE = PRST_TRK_COUNT-tracksCount;
            qCInfo(jtConfigurator) << "Number of tracks to create : "<<TRK_TO_CREATE;

            for (int i = 0; i < TRK_TO_CREATE; i++) {
                localGroupChannels.at(group)->addTrackView(i+1);
                qCInfo(jtConfigurator) << "SubTrack added in group : "<<group;

                // tracksCount++;
            }
        } else if (tracksCount > PRST_TRK_COUNT) {// must delete a track
            int TRK_TO_DEL = tracksCount-PRST_TRK_COUNT;
            qCInfo(jtConfigurator) << "Number of tracks to delete : "<<TRK_TO_DEL;

            for (int i = 0; i < TRK_TO_DEL; i++) {
                localGroupChannels.at(group)->removeTrackView(TRK_TO_DEL);
                qCInfo(jtConfigurator) << "SubTrack removed in group : "<<group;

                // tracksCount--;
            }
        }

        // now the preset's SUB track count ;
        tracks = localGroupChannels.at(group)->getTracks();
        tracksCount = tracks.size();
        // assign preset to indexed tracks
        for (int index = 0; index < tracksCount; index++) {
            // gain
            qCInfo(jtConfigurator) << "<<<<<<<<<<<<<<<<<<<<";
            float gain = preset.channels.at(group).subChannels.at(index).gain;
            tracks.at(index)->getInputNode()->setGain(gain);
            qCInfo(jtConfigurator) << "Track"<<index<<" gain : "<<gain<<" for"<<index;
            // pan
            float pan = preset.channels.at(group).subChannels.at(index).pan;
            tracks.at(index)->getInputNode()->setPan(pan);
            qCInfo(jtConfigurator) << "Track "<<index<<"Pan : "<<pan<<" for"<<index;
            // boost
            int boost = preset.channels.at(group).subChannels.at(index).boost;
            BaseTrackView::BoostValue boostValue = BaseTrackView::intToBoostValue(boost);
            tracks.at(index)->initializeBoostButtons(boostValue);
            qCInfo(jtConfigurator) << "Boost "<<index<<"index : "<<boostValue<<" for"<<index;
            // mute
            bool muted = preset.channels.at(group).subChannels.at(index).muted;
            tracks.at(index)->getInputNode()->setMute(muted);
            qCInfo(jtConfigurator) << "Mute "<<index<<"state : "<<muted<<" for"<<index;
        }
    }
    qCInfo(jtConfigurator) << "***********************************";
}

void MainWindow::initializeLocalInputChannels()
{
    qCInfo(jtGUI) << "Initializing local inputs...";
    Persistence::InputsSettings inputsSettings = mainController->getSettings().getInputsSettings();
    int channelIndex = 0;
    foreach (Persistence::Channel channel, inputsSettings.channels) {
        qCInfo(jtGUI) << "\tCreating channel "<< channel.name;
        LocalTrackGroupView *channelView = addLocalChannel(channelIndex, channel.name,
                                                           channel.subChannels.isEmpty(), false);
        foreach (Persistence::Subchannel subChannel, channel.subChannels) {
            qCInfo(jtGUI) << "\t\tCreating sub-channel ";
            BaseTrackView::BoostValue boostValue = BaseTrackView::intToBoostValue(subChannel.boost);
            LocalTrackView *subChannelView = dynamic_cast<LocalTrackView*>(channelView->addTrackView(channelIndex));
            subChannelView->setInitialValues(subChannel.gain, boostValue, subChannel.pan, subChannel.muted);
            if (canCreateSubchannels()) {
                //initializeSubChannel(subChannel, subChannelView);
                //TODO need the code removed here
                //Eu acho que esse if é totalmente desnecessáio, essa classe nunca vai criar subchannels?
            }
            else{
                break;// avoid hacking in config file to create more subchannels in VST plugin.
            }
        }
        channelIndex++;
    }
    if (channelIndex == 0)// no channels in settings file or no settings file...
        addLocalChannel(0, "your channel", true, true);// initialize to noInput
    qCInfo(jtGUI) << "Initializing local inputs done!";
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

void MainWindow::initializeWindowState()
{
    bool wasFullScreenInLastSession = mainController->getSettings().windowsWasFullScreenViewMode();
    this->fullScreenViewMode = wasFullScreenInLastSession && canUseFullScreen();

    // set window mode: mini mode or full view mode
    setFullViewStatus(mainController->getSettings().windowsWasFullViewMode());

    if (mainController->getSettings().windowWasMaximized()) {
        qCDebug(jtGUI)<< "setting window state to maximized";
        showMaximized();
    } else {
        if (canUseFullScreen()) { // avoid set plugin to full screen or move the plugin window
            if (mainController->getSettings().windowsWasFullScreenViewMode()) {
                showFullScreen();
            } else {// this else fix the cropped window when starting in full screen mode
                QPointF location = mainController->getSettings().getLastWindowLocation();
                QDesktopWidget *desktop = QApplication::desktop();
                int desktopWidth = desktop->width();
                int desktopHeight = desktop->height();
                int x = desktopWidth * location.x();
                int y = desktopHeight * location.y();
                this->move(x, y);
                qCDebug(jtGUI)<< "Restoring window to position:" << x << ", " << y;
                qCDebug(jtGUI)<< "Window size:" << width() << ", " << height();
            }
        }
    }
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
                roomViewPanel = new JamRoomViewPanel(roomInfo, ui.allRoomsContent, mainController);
                roomViewPanels.insert(roomInfo.getID(), roomViewPanel);
                connect(roomViewPanel, SIGNAL(startingListeningTheRoom(
                                                  Login::RoomInfo)), this,
                        SLOT(playPublicRoomStream(Login::RoomInfo)));
                connect(roomViewPanel, SIGNAL(finishingListeningTheRoom(
                                                  Login::RoomInfo)), this,
                        SLOT(stopPublicRoomStream(Login::RoomInfo)));
                connect(roomViewPanel, SIGNAL(enteringInTheRoom(Login::RoomInfo)), this,
                        SLOT(tryEnterInRoom(Login::RoomInfo)));
            }
            ((QGridLayout *)ui.allRoomsContent->layout())->addWidget(roomViewPanel, rowIndex,
                                                                     collumnIndex);
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

    if (!mainController->userNameWasChoosed()) {
        QString lastUserName = mainController->getUserName();
        UserNameDialog dialog(ui.centralWidget, lastUserName);
        centerDialog(&dialog);
        if (dialog.exec() == QDialog::Accepted) {
            QString userName = dialog.getUserName().trimmed();
            if (!userName.isEmpty()) {
                mainController->setUserName(userName);
                setWindowTitle(
                    "Jamtaba v" + QApplication::applicationVersion() + " (" + userName + ")");
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

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainWindow::~MainWindow()
{
    qCDebug(jtGUI) << "MainWindow destructor...";
    setParent(nullptr);
    if (mainController) {
        mainController->stop();
        mainController->saveLastUserSettings(getInputsSettings());
    }

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
    QDesktopServices::openUrl(QUrl(
                                  "https://github.com/elieserdejesus/JamTaba/wiki/Jamtaba's-user-guide"));
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
    if (action == ui.actionQuit) { // TODO put the Quit action in a better place. A 'Quit' entry in the 'Preferences' menu is not good.
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


// plugin finder events
void MainWindow::showPluginScanDialog()
{
    if (!pluginScanDialog) {
        pluginScanDialog.reset(new PluginScanDialog(this));
        QObject::connect(pluginScanDialog.data(), SIGNAL(rejected()), this,
                         SLOT(closePluginScanDialog()));
    }
    pluginScanDialog->show();
}

void MainWindow::closePluginScanDialog()
{
    mainController->cancelPluginFinder();
    pluginScanDialog.reset();// reset to null pointer
}

void MainWindow::hidePluginScanDialog(bool finishedWithoutError)
{
    Q_UNUSED(finishedWithoutError);
    if (pluginScanDialog)
        pluginScanDialog->close();
    pluginScanDialog.reset();
}

void MainWindow::addPluginToBlackList(QString pluginPath)
{
    QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(pluginPath);
    QWidget *parent = this;
    if (pluginScanDialog)
        parent = pluginScanDialog.data();
    QString message = pluginName + " can't be loaded and will be black listed!";
    QMessageBox::warning(parent, "Plugin Error!", message);
    mainController->addBlackVstToSettings(pluginPath);
}

void MainWindow::addFoundedPlugin(QString name, QString group, QString path)
{
    Q_UNUSED(path);
    Q_UNUSED(group);
    if (pluginScanDialog)
        pluginScanDialog->addFoundedPlugin(name);
}

void MainWindow::setCurrentScanningPlugin(QString pluginPath)
{
    if (pluginScanDialog)
        pluginScanDialog->setCurrentScaning(pluginPath);
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

    ui.chatArea->setMinimumWidth(fullViewMode ? 280 : 180);

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
    // ui.bottomPanel->setMinimumHeight(fullViewMode ? 130 : 130);

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

void MainWindow::setFullScreenStatus(bool fullScreen)
{
    fullScreenViewMode = fullScreen;
    if (fullScreen)
        showFullScreen();
    else
        showNormal();
    mainController->setFullScreenView(fullScreenViewMode);
    ui.actionFullscreenMode->setChecked(fullScreen);
}

void MainWindow::toggleFullScreen()
{
    setFullScreenStatus(!fullScreenViewMode);// toggle
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
    QMessageBox::about(this, "About Jamtaba",
                       "Jamtaba version is " + QApplication::applicationVersion());
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
