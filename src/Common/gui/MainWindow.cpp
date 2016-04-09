#include "MainWindow.h"
#include "PreferencesDialog.h"
#include "LocalTrackView.h"
#include "audio/core/LocalInputNode.h"
#include "NinjamRoomWindow.h"
#include "Highligther.h"
#include "PrivateServerDialog.h"
#include "NinjamController.h"
#include "Utils.h"
#include "UserNameDialog.h"
#include "log/Logging.h"
#include "JamRoomViewPanel.h"
#include "ChordsPanel.h"

#include <QDesktopWidget>
#include <QDesktopServices>
#include "MainController.h"
// #include "performance/PerformanceMonitor.h"

using namespace Audio;
using namespace Persistence;
using namespace Controller;
using namespace Ninjam;
using namespace Persistence;

const QSize MainWindow::MINI_MODE_MIN_SIZE = QSize(800, 600);
const QSize MainWindow::FULL_VIEW_MODE_MIN_SIZE = QSize(1180, 790);
const int MainWindow::MINI_MODE_MAX_LOCAL_TRACKS_WIDTH = 182;

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

    setWindowTitle("JamTaba " + QApplication::applicationVersion());

    initializeLoginService();
    initializeMainTabWidget();
    initializeViewModeMenu();
    initializeMasterFader();
    initializeLanguageMenu();
    initializeTranslator();
    setupWidgets();
    setupSignals();

    qCInfo(jtGUI) << "MainWindow created!";
}

void MainWindow::initializeTranslator()
{
    //always install the translator
    QApplication::instance()->installTranslator(&translator);

    //remember the last selected language
    QString localeName = mainController->getSettings().getTranslation();
    loadTranslationFile(localeName);
}

void MainWindow::setLanguage(QAction *languageMenuAction)
{
    QString locale = languageMenuAction->data().toString();
    loadTranslationFile(locale);
    mainController->setTranslationLanguage(locale);
    if (mainController->isPlayingInNinjamRoom()) {
        ninjamWindow->getChatPanel()->setPreferredTranslationLanguage(locale);
    }
}

void MainWindow::loadTranslationFile(const QString &locale)
{
    translator.load(locale, ":/tr");
}

// ++++++++++++++++++++++++=
void MainWindow::initializeLanguageMenu()
{
    //create a menu action for each translation resource
    QDir translationsDir(":/tr");
    if (translationsDir.exists()) {
        QStringList locales = translationsDir.entryList();
        foreach (const QString &translationFile, locales) {
            QLocale loc(translationFile);
            QString actionText = loc.nativeLanguageName();
            QAction *action = ui.menuLanguage->addAction(actionText);
            action->setData(translationFile); //using the locale (pt_BR, jp_JP) as data
        }
    }
    else{
        qCritical() << "translations dir not exist! Can't create the Language menu!";
    }
}

// ++++++++++++++++++++++++=
void MainWindow::initialize()
{
    timerID = startTimer(1000/50);// timer used to animate audio peaks, midi activity, public room wave audio plot, etc.

    showBusyDialog(tr("Loading rooms list ..."));

    // initialize using last track input settings (using QTimer::singleShot to initialize the tracks (and load the plugins) after Jamtaba is opened).
    QTimer::singleShot(1, this, SLOT(initializeLocalInputChannels()));

    // set window mode: mini mode or full view mode
    setFullViewStatus(mainController->getSettings().windowsWasFullViewMode());
}

// ++++++++++++++++++++++++=
void MainWindow::showPeakMetersOnlyInLocalControls(bool showPeakMetersOnly)
{
    foreach (LocalTrackGroupView *channel, localGroupChannels)
        channel->setPeakMeterMode(showPeakMetersOnly);
    ui.userNameLineEdit->setVisible(!showPeakMetersOnly);

    ui.localControlsCollapseButton->setChecked(showPeakMetersOnly);
    updateLocalInputChannelsGeometry();
}

void MainWindow::updateLocalInputChannelsGeometry()
{
    if (localGroupChannels.isEmpty())
        return;

    int minWidth = ui.localTracksWidget->sizeHint().width();
    minWidth += 12;
    int maxWidth = minWidth;
    bool showingPeakMeterOnly = localGroupChannels.first()->isShowingPeakMeterOnly();
    Qt::ScrollBarPolicy scrollPolicy = Qt::ScrollBarAlwaysOff;

    // limit the local inputs widget in mini mode
    if (!showingPeakMeterOnly && isRunningInMiniMode()) {
        if (minWidth > MINI_MODE_MAX_LOCAL_TRACKS_WIDTH) {
            minWidth = maxWidth = MINI_MODE_MAX_LOCAL_TRACKS_WIDTH;
            scrollPolicy = Qt::ScrollBarAlwaysOn;
        }
    }
    ui.leftPanel->setMaximumWidth(maxWidth);
    ui.leftPanel->setMinimumWidth(minWidth);
    ui.scrollArea->setHorizontalScrollBarPolicy(scrollPolicy);

    if (isRunningInMiniMode() && localGroupChannels.count() > 1) {
        foreach (LocalTrackGroupView *trackGroup, localGroupChannels)
            trackGroup->setToNarrow();
    }
}

void MainWindow::toggleLocalInputsCollapseStatus()
{
    bool isShowingPeakMetersOnly = localGroupChannels.first()->isShowingPeakMeterOnly();
    showPeakMetersOnlyInLocalControls(!isShowingPeakMetersOnly);// toggle
}

// ++++++++++++++++++++++++=
Persistence::LocalInputTrackSettings MainWindow::getInputsSettings() const
{
    LocalInputTrackSettings settings;
    foreach (LocalTrackGroupView *trackGroupView, localGroupChannels) {
        Channel channel(trackGroupView->getGroupName());
        foreach (LocalTrackView *trackView, trackGroupView->getTracks<LocalTrackView *>()) {
            LocalInputNode *inputNode = trackView->getInputNode();
            ChannelRange inputNodeRange = inputNode->getAudioInputRange();
            int firstInput = inputNodeRange.getFirstChannel();
            int channels = inputNodeRange.getChannels();
            int midiDevice = inputNode->getMidiDeviceIndex();
            int midiChannel = inputNode->getMidiChannelIndex();
            float gain = Utils::poweredGainToLinear(inputNode->getGain());
            float boost = Utils::linearToDb(inputNode->getBoost());
            float pan = inputNode->getPan();
            bool muted = inputNode->isMuted();
            qint8 transpose = inputNode->getTranspose();
            quint8 lowerNote = inputNode->getMidiLowerNote();
            quint8 higherNote = inputNode->getMidiHigherNote();

            Subchannel sub(firstInput, channels, midiDevice, midiChannel, gain, boost, pan, muted, transpose, lowerNote, higherNote);
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
            roomViewPanels[roomID]->clear(true);
        mainController->stopRoomStream();
    }
}

void MainWindow::showMessageBox(const QString &title, const QString &text, QMessageBox::Icon icon)
{
    QMessageBox *messageBox = new QMessageBox(nullptr);
    messageBox->setWindowTitle(title);
    messageBox->setText(text);
    messageBox->setIcon(icon);
    messageBox->setAttribute(Qt::WA_DeleteOnClose, true);
    messageBox->show();
}

void MainWindow::handlePublicRoomStreamError(const QString &msg)
{
    stopCurrentRoomStream();
    showMessageBox(tr("Error!"), msg, QMessageBox::Critical);
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

            // TODO Refactoring: emit a signal 'localChannel removed' and catch this signal in NinjamController
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

void MainWindow::addChannelsGroup(const QString &name)
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
LocalTrackGroupView *MainWindow::addLocalChannel(int channelGroupIndex, const QString &channelName,
                                                 bool createFirstSubchannel)
{
    LocalTrackGroupView *localChannel = createLocalTrackGroupView(channelGroupIndex);

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

    if (createFirstSubchannel)
        localChannel->addTrackView(channelGroupIndex);

    ui.localTracksWidget->updateGeometry();

    return localChannel;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::loadPreset(const Preset &preset)
{
    if (preset.isValid()) {
        removeAllInputLocalTracks();
        initializeLocalInputChannels(preset.inputTrackSettings);
        QApplication::processEvents();
    }
}

void MainWindow::removeAllInputLocalTracks()
{
    while (!localGroupChannels.isEmpty()) {
        LocalTrackGroupView *view = localGroupChannels.first();
        ui.localTracksLayout->removeWidget(view);
        view->deleteLater();
        localGroupChannels.pop_front();
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// this function is overrided in MainWindowStandalone to load input selections and plugins
void MainWindow::initializeLocalSubChannel(LocalTrackView *localTrackView,
                                           const Subchannel &subChannel)
{
    BaseTrackView::Boost boostValue = BaseTrackView::intToBoostValue(subChannel.boost);
    localTrackView->setInitialValues(subChannel.gain, boostValue, subChannel.pan, subChannel.muted);
}

void MainWindow::initializeLocalInputChannels()
{
    initializeLocalInputChannels(mainController->getSettings().getInputsSettings());
}

void MainWindow::initializeLocalInputChannels(const LocalInputTrackSettings &inputsSettings)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    qCInfo(jtGUI) << "Initializing local inputs...";
    int channelIndex = 0;
    foreach (const Persistence::Channel &channel, inputsSettings.channels) {
        qCInfo(jtGUI) << "\tCreating channel "<< channel.name;
        bool createFirstSubChannel = channel.subChannels.isEmpty();
        LocalTrackGroupView *channelView = addLocalChannel(channelIndex, channel.name,
                                                           createFirstSubChannel);
        foreach (const Persistence::Subchannel &subChannel, channel.subChannels) {
            qCInfo(jtGUI) << "\t\tCreating sub-channel ";
            LocalTrackView *subChannelView = channelView->addTrackView(channelIndex);
            initializeLocalSubChannel(subChannelView, subChannel);
        }
        channelIndex++;
    }
    if (channelIndex == 0)// no channels in settings file or no settings file...
        addLocalChannel(0, tr("your channel"), true);
    qCInfo(jtGUI) << "Initializing local inputs done!";

    QApplication::restoreOverrideCursor();
}

void MainWindow::initializeLoginService()
{
    Login::LoginService *loginService = this->mainController->getLoginService();

    connect(loginService, SIGNAL(roomsListAvailable(QList<Login::RoomInfo>)), this,
            SLOT(refreshPublicRoomsList(const QList<Login::RoomInfo> &)));

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
        showBusyDialog(tr("disconnecting ..."));
        if (mainController->getNinjamController()->isRunning())
            mainController->stopNinjamController();
    }
}

void MainWindow::changeTab(int index)
{
    if (index > 0) {// click in room tab?
        if (mainController->isPlayingInNinjamRoom() && mainController->isPlayingRoomStream())
            stopCurrentRoomStream();
    } else {// click in the public rooms tab?
        updatePublicRoomsListLayout();
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::showBusyDialog()
{
    showBusyDialog("");
}

void MainWindow::showBusyDialog(const QString &message)
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
    int newY = parentSize.height()/2;// - busyDialogSize.height()/2;
    busyDialog.move(newX, newY);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool MainWindow::jamRoomLessThan(const Login::RoomInfo &r1, const Login::RoomInfo &r2)
{
    return r1.getNonBotUsersCount() > r2.getNonBotUsersCount();
}

void MainWindow::handleIncompatiblity()
{
    hideBusyDialog();
    QString text = tr("Your Jamtaba version is not compatible with previous versions!");
    QMessageBox::warning(this, tr("Server : Compatibility problem"), text);
    close();
}

void MainWindow::detachMainController()
{
    mainController = nullptr;
}

void MainWindow::handleServerConnectionError(const QString &errorMsg)
{
    hideBusyDialog();
    QMessageBox::warning(this, tr("Error!"), tr("Error connecting in Jamtaba server!\n") + errorMsg);
    close();
}

void MainWindow::showNewVersionAvailableMessage()
{
    hideBusyDialog();
    QString text
        =
            tr("A new Jamtaba version is available for download! Please use the <a href='http://www.jamtaba.com'>new version</a>!");
    QMessageBox::information(this, tr("New Jamtaba version available!"), text);
}

JamRoomViewPanel *MainWindow::createJamRoomViewPanel(const Login::RoomInfo &roomInfo)
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

bool MainWindow::canUseTwoColumnLayout() const
{
    if (isFullScreen() || isMaximized())
        return true;

    if (mainController->isPlayingInNinjamRoom())
        return false;
    else
        return fullViewMode;// if is in mini mode (!fullViewMode) we are using just 1 collumn layout

    return true;
}

void MainWindow::refreshPublicRoomsList(const QList<Login::RoomInfo> &publicRooms)
{
    if (!isVisible() || publicRooms.isEmpty())
        return;

    hideBusyDialog();

    QList<Login::RoomInfo> sortedRooms(publicRooms);
    qSort(sortedRooms.begin(), sortedRooms.end(), jamRoomLessThan);

    int index = 0;
    foreach (const Login::RoomInfo &roomInfo, sortedRooms) {
        if (roomInfo.getType() == Login::RoomTYPE::NINJAM) {// skipping other rooms at moment
            bool twoCollumns = canUseTwoColumnLayout();
            int rowIndex = twoCollumns ? (index / 2) : (index);
            int collumnIndex = twoCollumns ? (index % 2) : 0;
            JamRoomViewPanel *roomViewPanel = roomViewPanels[roomInfo.getID()];
            if (roomViewPanel) {
                roomViewPanel->refresh(roomInfo);
                //check if is playing a public room stream but this room is empty now
                if( mainController->isPlayingRoomStream() ){
                    if(roomInfo.isEmpty() && mainController->getCurrentStreamingRoomID() == roomInfo.getID())
                        stopCurrentRoomStream();
                }
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
void MainWindow::playPublicRoomStream(const Login::RoomInfo &roomInfo)
{
    // clear all plots
    foreach (JamRoomViewPanel *viewPanel, this->roomViewPanels.values())
        viewPanel->clear(roomInfo.getID() != viewPanel->getRoomInfo().getID());

    if (roomInfo.hasStream())// just in case...
        mainController->playRoomStream(roomInfo);
}

void MainWindow::stopPublicRoomStream(const Login::RoomInfo &roomInfo)
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
void MainWindow::tryEnterInRoom(const Login::RoomInfo &roomInfo, const QString &password)
{
    // stop room stream before enter in a room
    if (mainController->isPlayingRoomStream()) {
        long long roomID = mainController->getCurrentStreamingRoomID();
        if (roomViewPanels[roomID])
            roomViewPanels[roomID]->clear(true);
        mainController->stopRoomStream();
    }

    // show the user name dialog
    if (!mainController->userNameWasChoosed()) {
        UserNameDialog dialog(ui.centralWidget, getGuessedUserName());
        centerDialog(&dialog);
        if (dialog.exec() == QDialog::Accepted) {
            QString userName = dialog.getUserName().trimmed();
            if (!userName.isEmpty()) {
                mainController->setUserName(userName);
                ui.userNameLineEdit->setText(userName); //show the user name in top of local tracks

                //change the window title to include user name
                QString version = QApplication::applicationVersion();
                QString windowTitle = "JamTaba " + version + " (" + userName + ")";
                setWindowTitle(windowTitle);
            } else {
                QMessageBox::warning(this, tr("Warning!"), tr("Empty name is not allowed!"));
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
        showBusyDialog(tr("Connecting in %1 ... ").arg(roomInfo.getName()));
        mainController->enterInRoom(roomInfo, getChannelsNames(), password);
    }
}

QString MainWindow::getGuessedUserName() const
{
    //guess user name using homePath
    QString homePath = QDir::homePath();
    int separatorIndex = homePath.lastIndexOf(QRegularExpression("[/\\\\]")); //QDir::separator() as parameter for 'lastIndexOf' not work in my windows 10
    if (separatorIndex >= 0) {
        return homePath.right(homePath.size() - (separatorIndex+1));
    }
    return homePath;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::enterInRoom(const Login::RoomInfo &roomInfo)
{
    qCDebug(jtGUI) << "hidding busy dialog...";
    hideBusyDialog();

    //lock the user name field, user name can't be changed when jamming
    ui.userNameLineEdit->setReadOnly(true);
    if (ui.userNameLineEdit->hasFocus())
        ui.userNameLineEdit->clearFocus();

    qCDebug(jtGUI) << "creating NinjamRoomWindow...";
    ninjamWindow.reset(createNinjamWindow(roomInfo, mainController));
    QString tabName = roomInfo.getName() + " (" + QString::number(roomInfo.getPort()) + ")";
    ninjamWindow->setFullViewStatus(this->fullViewMode);
    int index = ui.tabWidget->addTab(ninjamWindow.data(), tabName);
    ui.tabWidget->setCurrentIndex(index);

    // add the chat panel in main window
    qCDebug(jtGUI) << "adding ninjam chat panel...";
    ChatPanel *chatPanel = ninjamWindow->getChatPanel();
    ui.chatTabWidget->addTab(chatPanel, "");
    updateChatTabTitle(roomInfo.getName());//set and translate the chat tab title

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

void MainWindow::updateChatTabTitle(const QString &roomName)
{
    int chatTabIndex = 0; //assuming chat is the first tab
    ui.chatTabWidget->setTabText(chatTabIndex, tr("chat %1").arg(roomName));
}

void MainWindow::showMetronomePreferencesDialog()
{
    openPreferencesDialog(ui.actionMetronome);
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
void MainWindow::exitFromRoom(bool normalDisconnection, QString disconnectionMessage)
{
    hideBusyDialog();

    //unlock the user name field
    ui.userNameLineEdit->setReadOnly(false);

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
    if (ninjamWindow)
        ui.bottomPanel->layout()->removeWidget(ninjamWindow->getNinjamPanel());
    dynamic_cast<QVBoxLayout *>(ui.bottomPanel->layout())->addWidget(ui.masterControlsPanel, 0,
                                                                     Qt::AlignHCenter);
    hideChordsPanel();

    ninjamWindow.reset();

    // hide chat area
    ui.chatArea->setVisible(false);

    setInputTracksPreparingStatus(false);/** reset the prepating status when user leave the room. This is specially necessary if user enter in a room and leave before the track is prepared to transmit.*/

    if (!normalDisconnection) {
        if (!disconnectionMessage.isEmpty())
            showMessageBox(tr("Error"), disconnectionMessage, QMessageBox::Warning);
        else
            showMessageBox(tr("Error"), tr("Disconnected from ninjam server"), QMessageBox::Warning);
    } else {
        if (roomToJump) {// waiting the disconnection to connect in a new room?
            showBusyDialog(tr("Connecting in %1").arg(roomToJump->getName()));
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
            bool buffering = mainController->getRoomStreamer()->isBuffering();
            roomView->setShowBufferingState(buffering);
            if (!buffering) {
                Audio::AudioPeak peak = mainController->getRoomStreamPeak();
                roomView->addPeak(peak.getMax());
            }
            else{
                int percentage = mainController->getRoomStreamer()->getBufferingPercentage();
                roomView->setBufferingPercentage(percentage);
            }
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
        showPeakMetersOnlyInLocalControls(
            isRunningInMiniMode() && width() <= MINI_MODE_MIN_SIZE.width());
        updatePublicRoomsListLayout();
    }
    else if (ev->type() == QEvent::LanguageChange) {
        ui.retranslateUi(this);
        if (ninjamWindow) {
            updateChatTabTitle(ninjamWindow.data()->getRoomName()); //translate the chat tab title
        }
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
void MainWindow::connectInPrivateServer(const QString &server, int serverPort,
                                        const QString &password)
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
        else if (action == ui.actionMetronome)
            initialTab = PreferencesDialog::TAB_METRONOME;

        stopCurrentRoomStream();

        PreferencesDialog *dialog = createPreferencesDialog();// factory method, overrided in derived classes MainWindowStandalone and MainWindowVST
        dialog->initialize(initialTab, &mainController->getSettings());// initializing here to avoid call virtual methods inside PreferencesDialog constructor
        dialog->show();
    }
}

//+++++++++++++++++++++++++
void MainWindow::setupPreferencesDialogSignals(PreferencesDialog *dialog)
{
    Q_ASSERT(dialog);

    connect(dialog, SIGNAL(multiTrackRecordingStatusChanged(bool)), this, SLOT(setMultiTrackRecordingStatus(bool)));
    connect(dialog, SIGNAL(recordingPathSelected(const QString &)), this, SLOT(setRecordingPath(const QString &)));
    connect(dialog, SIGNAL(builtInMetronomeSelected(QString)), this, SLOT(setBuiltInMetronome(QString)));
    connect(dialog, SIGNAL(customMetronomeSelected(QString,QString)), this, SLOT(setCustomMetronome(QString,QString)));
}

void MainWindow::setBuiltInMetronome(const QString &metronomeAlias)
{
    mainController->setBuiltInMetronome(metronomeAlias);
}

void MainWindow::setCustomMetronome(const QString &primaryBeatFile, const QString &secondaryBeatFile)
{
    mainController->setCustomMetronome(primaryBeatFile, secondaryBeatFile);
}

void MainWindow::setRecordingPath(const QString &newRecordingPath)
{
    mainController->storeRecordingPath(newRecordingPath);
}

void MainWindow::setMultiTrackRecordingStatus(bool recording)
{
    mainController->storeRecordingMultiTracksStatus(recording);
}

//++++++++++++++++++++++

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

void MainWindow::updatePublicRoomsListLayout()
{
    QList<Login::RoomInfo> roomInfos;
    foreach (JamRoomViewPanel *roomView, roomViewPanels)
        roomInfos.append(roomView->getRoomInfo());
    refreshPublicRoomsList(roomInfos);
}

void MainWindow::setFullViewStatus(bool fullViewActivated)
{
    this->fullViewMode = fullViewActivated;
    if (isRunningInMiniMode())
        setMinimumSize(MINI_MODE_MIN_SIZE);
    else // full view
        setMinimumSize(FULL_VIEW_MODE_MIN_SIZE);
    if (!isMaximized() && !isFullScreen()) {
        showNormal();
        resize(minimumSize());
    }

    int tabLayoutMargim = isRunningInFullViewMode() ? 9 : 5;
    ui.tabLayout->setContentsMargins(tabLayoutMargim, tabLayoutMargim, tabLayoutMargim,
                                     tabLayoutMargim);

    // show only the peak meters if user is in mini mode and is not maximized or full screen
    showPeakMetersOnlyInLocalControls(isRunningInMiniMode() && !isMaximized() && !isFullScreen());

    ui.chatArea->setMinimumWidth(isRunningInFullViewMode() ? 280 : 180); // TODO Refactoring: remove these 'Magic Numbers'

    // refresh the public rooms list
    if (!mainController->isPlayingInNinjamRoom()) {
        updatePublicRoomsListLayout();
    } else {
        if (ninjamWindow)
            ninjamWindow->setFullViewStatus(isRunningInFullViewMode());
    }

    // local tracks are narrowed in mini mode if user is using more than 1 subchannel
    foreach (LocalTrackGroupView *localTrackGroup, localGroupChannels) {
        if (isRunningInMiniMode()
            && (localTrackGroup->getTracksCount() > 1 || localGroupChannels.size() > 1))
            localTrackGroup->setToNarrow();
        else
            localTrackGroup->setToWide();
    }

    ui.actionFullView->setChecked(isRunningInFullViewMode());
    ui.actionMiniView->setChecked(isRunningInMiniMode());

    int margim = isRunningInFullViewMode() ? 6 : 2;
    ui.bottomPanelLayout->setContentsMargins(margim, margim, margim, margim);
    ui.bottomPanelLayout->setSpacing(isRunningInFullViewMode() ? 6 : 2);

    if (ninjamWindow) {
        NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
        if (ninjamPanel)
            ninjamPanel->setFullViewStatus(isRunningInFullViewMode());

        ChatPanel *chatPanel = ninjamWindow->getChatPanel();
        if (chatPanel)
            chatPanel->updateMessagesGeometry();
    }
}

// +++++++++++++++++++++++++++
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::Resize) {
        if (target == ui.localTracksWidget)
            updateLocalInputChannelsGeometry();
        else if (target == ui.userNameLineEdit)
            updateUserNameLabel();
        return true;
    } else {
        if (target == ui.masterFader && event->type() == QEvent::MouseButtonDblClick) {
            ui.masterFader->setValue(100);
            return true;
        }
    }
    return QMainWindow::eventFilter(target, event);
}

void MainWindow::updateUserNameLabel()
{
    Qt::Alignment alignment = Qt::AlignCenter;
    QFontMetrics fontMetrics = ui.userNameLineEdit->fontMetrics();
    int maxWidth = ui.userNameLineEdit->sizeHint().width();
    int textWidth = fontMetrics.width(ui.userNameLineEdit->text());
    if (textWidth > maxWidth) {
        alignment = Qt::AlignVCenter | Qt::AlignLeft;
    }
    ui.userNameLineEdit->setAlignment(alignment);
}

void MainWindow::resetLocalChannels()
{
    foreach (LocalTrackGroupView *localChannel, localGroupChannels)
        localChannel->resetTracks();
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
    QString title = tr("About Jamtaba");
    QString text = tr("Jamtaba version is %1").arg(QApplication::applicationVersion());
    QMessageBox::about(this, title, text);
}

// ++++++++++++++++++
void MainWindow::setMasterGain(int faderPosition)
{
    float newGain = faderPosition/100.0;
    mainController->setMasterGain(newGain);
}

// ++++++++++++++++++=
ChordsPanel *MainWindow::createChordsPanel()
{
    ChordsPanel *chordsPanel = new ChordsPanel();
    connect(chordsPanel, SIGNAL(sendingChordsToChat()), this,
            SLOT(sendCurrentChordProgressionToChat()));
    connect(chordsPanel, SIGNAL(chordsDiscarded()), this, SLOT(hideChordsPanel()));
    return chordsPanel;
}

void MainWindow::showChordProgression(const ChordProgression &progression)
{
    int currentBpi = mainController->getNinjamController()->getCurrentBpi();
    if (progression.canBeUsed(currentBpi)) {
        if (!chordsPanel)
            chordsPanel = createChordsPanel();
        else
            chordsPanel->setVisible(true);
        bool needStrech = progression.getBeatsPerInterval() != currentBpi;
        if (needStrech)
            chordsPanel->setChords(progression.getStretchedVersion(currentBpi));
        else
            chordsPanel->setChords(progression);

        // add the chord panel in top of bottom panel in main window
        dynamic_cast<QVBoxLayout *>(ui.bottomPanel->layout())->insertWidget(0, chordsPanel);
        if (ninjamWindow)
            ninjamWindow->getNinjamPanel()->setLowContrastPaintInIntervalPanel(true);
    } else {
        int measures = progression.getMeasures().size();
        QString msg = tr("These chords (%1 measures) can't be used in a %2 bpi interval!")
                .arg(QString::number(measures))
                .arg(QString::number(currentBpi));
        QMessageBox::warning(this, tr("Problem..."), msg);
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

    if (ninjamWindow)
        ninjamWindow->getNinjamPanel()->setLowContrastPaintInIntervalPanel(false);
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
    if (!ninjamWindow)
        return;

    NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
    if (!ninjamPanel)
        return;
    ninjamPanel->setBpm(bpm);
}

void MainWindow::updateCurrentIntervalBeat(int beat)
{
    if (!ninjamWindow)
        return;
    NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
    if (!ninjamPanel)
        return;
    ninjamPanel->setCurrentBeat(beat);

    if (chordsPanel)
        chordsPanel->setCurrentBeat(beat);
}

void MainWindow::setupWidgets()
{
    ui.masterMeterL->setOrientation(Qt::Horizontal);
    ui.masterMeterR->setOrientation(Qt::Horizontal);
    ui.masterFader->installEventFilter(this);// handle double click in master fader

    ui.chatArea->setVisible(false);// hide chat area until connect in a server to play

    ui.allRoomsContent->setLayout(new QGridLayout());
    ui.allRoomsContent->layout()->setContentsMargins(0, 0, 0, 0);
    ui.allRoomsContent->layout()->setSpacing(24);

    ui.localTracksWidget->installEventFilter(this);

    ui.userNameLineEdit->installEventFilter(this);

    QString lastUserName = mainController->getUserName();
    if (!lastUserName.isEmpty()) {
        ui.userNameLineEdit->setText(lastUserName);
    }
}

void MainWindow::setupSignals()
{
    connect(ui.menuPreferences, SIGNAL(triggered(QAction *)), this,
                     SLOT(openPreferencesDialog(QAction *)));

    connect(ui.actionNinjam_community_forum, SIGNAL(triggered(bool)), this,
                     SLOT(showNinjamCommunityWebPage()));

    connect(ui.actionNinjam_Official_Site, SIGNAL(triggered(bool)), this,
                     SLOT(showNinjamOfficialWebPage()));

    connect(ui.actionPrivate_Server, SIGNAL(triggered(bool)), this,
                     SLOT(showPrivateServerDialog()));

    connect(ui.actionReportBugs, SIGNAL(triggered(bool)), this,
                     SLOT(showJamtabaIssuesWebPage()));

    connect(ui.actionWiki, SIGNAL(triggered(bool)), this,
                     SLOT(showJamtabaWikiWebPage()));

    connect(ui.actionUsersManual, SIGNAL(triggered(bool)), this,
                     SLOT(showJamtabaUsersManual()));

    connect(ui.actionCurrentVersion, SIGNAL(triggered(bool)), this,
                     SLOT(showJamtabaCurrentVersion()));

    connect(ui.localControlsCollapseButton, SIGNAL(clicked()), this,
                     SLOT(toggleLocalInputsCollapseStatus()));

    connect(mainController->getRoomStreamer(), SIGNAL(error(QString)), this,
                     SLOT(handlePublicRoomStreamError(QString)));

    connect(ui.masterFader, SIGNAL(valueChanged(int)), this,
                     SLOT(setMasterGain(int)));

    connect(ui.actionQuit, SIGNAL(triggered(bool)), this, SLOT(close()));

    connect(ui.menuLanguage, SIGNAL(triggered(QAction*)), this, SLOT(setLanguage(QAction*)));

    connect(ui.userNameLineEdit, SIGNAL(editingFinished()), this, SLOT(updateUserName()));
}

void MainWindow::updateUserName()
{
    QString newUserName = ui.userNameLineEdit->text();
    qDebug() << "Setting user name to:" << newUserName;
    mainController->setUserName(newUserName);
}

void MainWindow::initializeMasterFader()
{
    float lastMasterGain = mainController->getSettings().getLastMasterGain();
    int faderPosition = lastMasterGain * 100;
    ui.masterFader->setValue(faderPosition);
    setMasterGain(faderPosition);
}
