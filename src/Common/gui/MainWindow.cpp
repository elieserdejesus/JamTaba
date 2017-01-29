#include "MainWindow.h"
#include "PreferencesDialog.h"
#include "LocalTrackView.h"
#include "audio/core/LocalInputNode.h"
#include "NinjamRoomWindow.h"
#include "Highligther.h"
#include "PrivateServerDialog.h"
#include "NinjamController.h"
#include "Utils.h" // TODO refactoring to AudioUtils.h and use namespace
#include "GuiUtils.h"
#include "UserNameDialog.h"
#include "log/Logging.h"
#include "JamRoomViewPanel.h"
#include "ChordsPanel.h"
#include "MapWidget.h"

#include <QDesktopWidget>
#include <QDesktopServices>
#include <QRect>
#include <QDateTime>
#include "MainController.h"
#include "ThemeLoader.h"
#include "performance/PerformanceMonitor.h"

using namespace Audio;
using namespace Persistence;
using namespace Controller;
using namespace Ninjam;
using namespace Persistence;

const QSize MainWindow::MAIN_WINDOW_MIN_SIZE = QSize(1100, 665);
const QString MainWindow::NIGHT_MODE_SUFFIX = "_nm";

const quint8 MainWindow::DEFAULT_REFRESH_RATE = 30; // in Hertz
const quint8 MainWindow::MAX_REFRESH_RATE = 60; // in Hertz

const int MainWindow::PERFORMANCE_MONITOR_REFRESH_TIME = 200;//in miliseconds

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainWindow::MainWindow(Controller::MainController *mainController, QWidget *parent) :
    QMainWindow(parent),
    busyDialog(0),
    mainController(mainController),
    ninjamWindow(nullptr),
    roomToJump(nullptr),
    chordsPanel(nullptr),
    lastPerformanceMonitorUpdate(0)
{
    qCDebug(jtGUI) << "Creating MainWindow...";

    ui.setupUi(this);

    setWindowTitle("JamTaba");

    initializeLoginService();
    initializeMainTabWidget();
    initializeViewMenu();
    initializeMasterFader();
    initializeLanguageMenu();
    initializeTranslator();
    initializeThemeMenu();
    initializeMeteringOptions();
    setupWidgets();
    setupSignals();

    qCDebug(jtGUI) << "MainWindow created!";
}

void MainWindow::initializeMeteringOptions()
{
    const Persistence::Settings &settings = mainController->getSettings();
    AudioMeter::setPaintMaxPeakMarker(settings.isShowingMaxPeaks());
    quint8 meterOption = settings.getMeterOption();
    switch (meterOption) {
    case 0:
        AudioMeter::paintPeaksAndRms();
        break;
    case 1:
        AudioMeter::paintPeaksOnly();
        break;
    case 2:
        AudioMeter::paintRmsOnly();
        break;
    }
}

void MainWindow::initializeTranslator()
{
    // always install the translators (one for Jamtaba strings and another for Qt strings)
    qApp->installTranslator(&jamtabaTranslator);
    qApp->installTranslator(&qtTranslator);

    // remember the last selected language
    QString localeName = mainController->getSettings().getTranslation();
    loadTranslationFile(localeName);
}

void MainWindow::setLanguage(QAction *languageMenuAction)
{
    QString locale = languageMenuAction->data().toString();
    loadTranslationFile(locale);
    mainController->setTranslationLanguage(locale);
    updatePublicRoomsListLayout();
    if (mainController->isPlayingInNinjamRoom()) {
        ninjamWindow->getChatPanel()->setPreferredTranslationLanguage(locale);
        ninjamWindow->updateGeoLocations();
    }
}

void MainWindow::updateNightModeInWorldMaps()
{
    QString themeName = mainController->getTheme();
    MapWidget::setNightMode(MainWindow::themeCanUseNightModeWorldMaps(themeName));
}

void MainWindow::setTheme(const QString &themeName)
{
    if(!mainController->setTheme(themeName)) {
        QString errorMessage = tr("Error loading the theme %1").arg(themeName);
        QMessageBox::critical(this, tr("Error!"), errorMessage);
    }

}

void MainWindow::changeTheme(QAction *action)
{
    QString themeName = action->data().toString();
    setTheme(themeName);
}

bool MainWindow::themeCanUseNightModeWorldMaps(const QString &themeName)
{
    return themeName.endsWith(NIGHT_MODE_SUFFIX);
}

QString MainWindow::getStripedThemeName(const QString &fullThemeName)
{
    if (themeCanUseNightModeWorldMaps(fullThemeName))
        return fullThemeName.left(fullThemeName.size() - 3); // remove the last 3 letters in theme name

    return fullThemeName;
}

void MainWindow::initializeThemeMenu()
{
    connect(ui.menuTheme, &QMenu::triggered, this, &MainWindow::changeTheme);
    connect(ui.menuTheme, &QMenu::aboutToShow, this, &MainWindow::translateThemeMenu); // the menu is translated before open

    // create a menu action for each theme
    QString themesDir = Configurator::getInstance()->getThemesDir().absolutePath();
    QStringList themes = Theme::Loader::getAvailableThemes(themesDir);
    foreach (const QString &themeDir, themes) {
        QString themeName = QFileInfo(themeDir).baseName();
        QAction *action = ui.menuTheme->addAction(getStripedThemeName(themeName));
         action->setData(themeName);
    }
}

void MainWindow::translateThemeMenu()
{
    foreach (QAction *action, ui.menuTheme->actions()) {
        QString themeName = getStripedThemeName(action->data().toString()); // use themeName as default text
        QString translatedName = getTranslatedThemeName(themeName);
        if (translatedName != themeName) // maybe we don't have a translation entry for the theme ...
            themeName = translatedName + " (" + themeName + ")";
        action->setText(themeName);
    }
}

QString MainWindow::getTranslatedThemeName(const QString &themeName)
{
    QMap<QString, QString> translatedNames;
    translatedNames.insert("Black",   tr("Black"));
    translatedNames.insert("Flat",    tr("Flat"));
    translatedNames.insert("Rounded", tr("Rounded"));
    translatedNames.insert("Volcano", tr("Volcano"));
    translatedNames.insert("Ice", tr("Ice"));

    if (translatedNames.contains(themeName))
        return translatedNames[themeName];

    return themeName; //returning the original name if we have not a translate entry in the map
}

void MainWindow::loadTranslationFile(const QString &locale)
{
    if( !jamtabaTranslator.load(locale, ":/tr"))
        qWarning() << "Can't load the Jamtaba translation for " << locale;

    if (locale != "en") { // avoid load en Qt translation (this file is obviously missing)
        QString fileName = "qtbase_" + locale;
        if (!qtTranslator.load(fileName, ":/qt_tr"))
            qWarning() << "Can't load Qt translation for " << locale;
    }
}

// ++++++++++++++++++++++++=
void MainWindow::initializeLanguageMenu()
{
    // create a menu action for each translation resource
    QDir translationsDir(":/tr");
    if (translationsDir.exists()) {
        QStringList locales = translationsDir.entryList();
        foreach (const QString &translationFile, locales) {
            QLocale loc(translationFile);
            QString nativeLanguageName = Gui::capitalize(loc.nativeLanguageName());
            QString englishLanguageName
                = Gui::capitalize(QLocale::languageToString(loc.language()));                         // QLocale::languageToString is returning capitalized String, but just to be shure (Qt can change in future) I'm using capitalize here too.
            QAction *action = ui.menuLanguage->addAction(
                nativeLanguageName + " (" + englishLanguageName + ")");
            action->setData(translationFile); // using the locale (pt_BR, ja_JP) as data
        }
    } else {
        qCritical() << "translations dir not exist! Can't create the Language menu!";
    }
}

// ++++++++++++++++++++++++=
void MainWindow::initializeGuiRefreshTimer()
{
    quint8 refreshRate = mainController->getSettings().getMeterRefreshRate();
    if (refreshRate <= 0)
        refreshRate = DEFAULT_REFRESH_RATE;
    else if (refreshRate > MAX_REFRESH_RATE)
        refreshRate = MAX_REFRESH_RATE;

    timerID = startTimer(1000/refreshRate);// timer used to animate audio peaks, midi activity, public room wave audio plot, etc.
}

void MainWindow::initialize()
{
    initializeGuiRefreshTimer();

    if (qApp->styleSheet().isEmpty()) { // allow custom stylesheet via app arguments
        QString themeName = mainController->getTheme();
        QString themesDir = Configurator::getInstance()->getThemesDir().absolutePath();
        if(!Theme::Loader::canLoad(themesDir, themeName))
            themeName = "Flat"; // fallback to Flat theme
        setTheme(themeName);
    }

    showBusyDialog(tr("Loading rooms list ..."));

    doWindowInitialization();

}

void MainWindow::doWindowInitialization()
{

    initializeLocalInputChannels(); // create the local tracks, load plugins, etc.

    initializeWindowSize();
}

// ++++++++++++++++++++++++=
void MainWindow::showPeakMetersOnlyInLocalControls(bool showPeakMetersOnly)
{
    foreach (LocalTrackGroupView *channel, localGroupChannels)
        channel->setPeakMeterMode(showPeakMetersOnly);

    ui.localControlsCollapseButton->setChecked(showPeakMetersOnly);
    updateLocalInputChannelsGeometry();

    ui.userNamePanel->setVisible(!showPeakMetersOnly);
    ui.labelYourControls->setVisible(!showPeakMetersOnly);
}

void MainWindow::updateLocalInputChannelsGeometry()
{
    if (localGroupChannels.isEmpty())
        return;

    int minWidth = ui.localTracksWidget->sizeHint().width();
    minWidth += 12;
    int maxWidth = minWidth;

    ui.leftPanel->setMaximumWidth(maxWidth);
    ui.leftPanel->setMinimumWidth(minWidth);
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
        int subchannelsCount = 0;
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
            bool stereoInverted = inputNode->isStereoInverted();
            qint8 transpose = inputNode->getTranspose();
            quint8 lowerNote = inputNode->getMidiLowerNote();
            quint8 higherNote = inputNode->getMidiHigherNote();
            bool routindMidiInput = subchannelsCount > 0 && inputNode->isRoutingMidiInput(); // midi routing is not allowed in first subchannel

            Subchannel sub(firstInput, channels, midiDevice, midiChannel, gain, boost, pan, muted, stereoInverted,
                                                                    transpose, lowerNote, higherNote, routindMidiInput);
            channel.subChannels.append(sub);

            subchannelsCount++;
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
    messageBox->exec();
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
    tabBar = ui.contentTabWidget->tabBar()->tabButton(0, QTabBar::RightSide);// try get the tabBar in right side (Windows)
    if (!tabBar)// try get the tabBar in left side (MAC OSX)
        tabBar = ui.contentTabWidget->tabBar()->tabButton(0, QTabBar::LeftSide);
    if (tabBar) {
        tabBar->resize(0, 0);
        tabBar->hide();
    }

    connect(ui.contentTabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(ui.contentTabWidget, SIGNAL(tabBarClicked(int)), this, SLOT(changeTab(int)));
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

        //set all loaded channels xmit to ON
        foreach (LocalTrackGroupView *trackGroupView,  localGroupChannels ) {
            mainController->setTransmitingStatus(trackGroupView->getChannelIndex(), true);
        }

        QApplication::processEvents();
    }
}

void MainWindow::removeAllInputLocalTracks()
{
    mainController->removeAllInputTracks();

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
    localTrackView->setInitialValues(subChannel.gain, boostValue, subChannel.pan, subChannel.muted, subChannel.stereoInverted);
}

void MainWindow::initializeLocalInputChannels()
{
    initializeLocalInputChannels(mainController->getSettings().getInputsSettings());
}

void MainWindow::initializeLocalInputChannels(const LocalInputTrackSettings &inputsSettings)
{
    QApplication::setOverrideCursor(Qt::WaitCursor); // this line was hanging/freezing in Mac

    int channelIndex = 0;
    foreach (const Persistence::Channel &channel, inputsSettings.channels) {
        qCDebug(jtGUI) << "\tCreating channel "<< channel.name;
        bool createFirstSubChannel = channel.subChannels.isEmpty();
        LocalTrackGroupView *channelView = addLocalChannel(channelIndex, channel.name,
                                                           createFirstSubChannel);
        foreach (const Persistence::Subchannel &subChannel, channel.subChannels) {
            qCDebug(jtGUI) << "\t\tCreating sub-channel ";
            LocalTrackView *subChannelView = channelView->addTrackView(channelIndex);
            initializeLocalSubChannel(subChannelView, subChannel);
        }
        channelIndex++;
    }
    if (channelIndex == 0)// no channels in settings file or no settings file...
        addLocalChannel(0, "", true); // create a channel using an empty name

    qCDebug(jtGUI) << "Initializing local inputs done!";

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
    QMessageBox::warning(this, tr("Error!"),
                         tr("Error connecting with Jamtaba server!\n") + errorMsg);
    close();
}

void MainWindow::showNewVersionAvailableMessage()
{
    hideBusyDialog();
    QString text
        =
            tr(
                "A new Jamtaba version is available for download! Please use the <a href='http://www.jamtaba.com'>new version</a>!");
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
    return ui.contentTabWidget->width() >= 830;
}

void MainWindow::refreshPublicRoomsList(const QList<Login::RoomInfo> &publicRooms)
{
    if (publicRooms.isEmpty())
        return;

    hideBusyDialog();

    QList<Login::RoomInfo> sortedRooms(publicRooms);
    qSort(sortedRooms.begin(), sortedRooms.end(), jamRoomLessThan);

    int index = 0;
    bool twoCollumns = canUseTwoColumnLayout();
    foreach (const Login::RoomInfo &roomInfo, sortedRooms) {
        if (roomInfo.getType() == Login::RoomTYPE::NINJAM) {// skipping other rooms at moment
            int rowIndex = twoCollumns ? (index / 2) : (index);
            int collumnIndex = twoCollumns ? (index % 2) : 0;
            JamRoomViewPanel *roomViewPanel = roomViewPanels[roomInfo.getID()];
            if (roomViewPanel) {
                roomViewPanel->refresh(roomInfo);
                // check if is playing a public room stream but this room is empty now
                if (mainController->isPlayingRoomStream()) {
                    if (roomInfo.isEmpty()
                        && mainController->getCurrentStreamingRoomID() == roomInfo.getID())
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
        UserNameDialog dialog(ui.centralWidget, MainController::getSuggestedUserName());
        centerDialog(&dialog);
        if (dialog.exec() == QDialog::Accepted) {
            QString userName = dialog.getUserName().trimmed();
            if (!userName.isEmpty()) {
                mainController->setUserName(userName);
                ui.userNameLineEdit->setText(userName); // show the user name in top of local tracks
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
        showBusyDialog(tr("Connecting with %1 ... ").arg(roomInfo.getName()));
        mainController->enterInRoom(roomInfo, getChannelsNames(), password);
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::enterInRoom(const Login::RoomInfo &roomInfo)
{
    qCDebug(jtGUI) << "hidding busy dialog...";
    hideBusyDialog();

    // lock the user name field, user name can't be changed when jamming
    setUserNameReadOnlyStatus(true);

    qCDebug(jtGUI) << "creating NinjamRoomWindow...";
    ninjamWindow.reset(createNinjamWindow(roomInfo, mainController));
    QString tabName = roomInfo.getName() + " (" + QString::number(roomInfo.getPort()) + ")";
    int index = ui.contentTabWidget->addTab(ninjamWindow.data(), tabName);
    ui.contentTabWidget->setCurrentIndex(index);

    // add the chat panel in main window
    qCDebug(jtGUI) << "adding ninjam chat panel...";
    ChatPanel *chatPanel = ninjamWindow->getChatPanel();
    ui.chatTabWidget->addTab(chatPanel, "");
    updateChatTabTitle();// set and translate the chat tab title

    QObject::connect(chatPanel, SIGNAL(userConfirmingChordProgression(
                                           ChordProgression)), this,
                     SLOT(acceptChordProgression(ChordProgression)));

    // add the ninjam panel in main window (bottom panel)
    qCDebug(jtGUI) << "adding ninjam panel...";
    NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
    ui.bottomPanel->layout()->removeWidget(ui.masterControlsPanel);

    dynamic_cast<QGridLayout *>(ui.bottomPanel->layout())->addWidget(ninjamPanel, 0, 0, 1, 1, Qt::AlignHCenter);
    ninjamPanel->addMasterControls(ui.masterControlsPanel) ;

    // show chat area
    setChatVisibility(true);

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

void MainWindow::setUserNameReadOnlyStatus(bool readOnly)
{
    ui.userNameLineEdit->setReadOnly(readOnly);

    QString icon = readOnly ? QStringLiteral("pencil-read-only.png") : QStringLiteral("pencil.png");
    ui.labelUserNameIcon->setPixmap(QPixmap(":/images/" + icon));

    QString toolTip = readOnly ? tr("Your name cannot be edited while jamming!") : tr(
        "Only Letters, numbers, hyphen and underscore allowed! Spaces will be replaced by an underscore.");
    ui.userNameLineEdit->setToolTip(toolTip);
    ui.labelUserNameIcon->setToolTip(toolTip);

    if (readOnly) {
        if (ui.userNameLineEdit->hasFocus())
            ui.userNameLineEdit->clearFocus();
    }
}

void MainWindow::updateChatTabTitle()
{
    int chatTabIndex = 0; // assuming chat is the first tab
    ui.chatTabWidget->setTabText(chatTabIndex, tr("Chat"));
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

    // unlock the user name field
    setUserNameReadOnlyStatus(false);

    // remove the jam room tab (the last tab)
    if (ui.contentTabWidget->count() > 1) {
        ui.contentTabWidget->widget(1)->deleteLater();// delete the room window
        ui.contentTabWidget->removeTab(1);
    }

    if (ui.chatTabWidget->count() > 0) {
        ui.chatTabWidget->widget(0)->deleteLater();
        ui.chatTabWidget->removeTab(0);
    }

    // remove ninjam panel from main window
    if (ninjamWindow)
        ui.bottomPanel->layout()->removeWidget(ninjamWindow->getNinjamPanel());
    dynamic_cast<QGridLayout *>(ui.bottomPanel->layout())->addWidget(ui.masterControlsPanel, 0, 0, 1, 1, Qt::AlignHCenter);
    hideChordsPanel();

    ninjamWindow.reset();

    setChatVisibility(false);

    setInputTracksPreparingStatus(false);/** reset the prepating status when user leave the room. This is specially necessary if user enter in a room and leave before the track is prepared to transmit.*/

    if (!normalDisconnection) {
        if (!disconnectionMessage.isEmpty())
            showMessageBox(tr("Error"), disconnectionMessage, QMessageBox::Warning);
        else
            showMessageBox(tr("Error"), tr("Disconnected from ninjam server"),
                           QMessageBox::Warning);
    } else {
        if (roomToJump) {// waiting the disconnection to connect in a new room?
            showBusyDialog(tr("Connecting with %1").arg(roomToJump->getName()));
            mainController->enterInRoom(*roomToJump, getChannelsNames(),
                                        (passwordToJump.isNull()
                                         || passwordToJump.isEmpty()) ? "" : passwordToJump);
            roomToJump.reset();
            passwordToJump = "";
        }
    }

    updatePublicRoomsListLayout();
}

void MainWindow::setChatVisibility(bool chatVisible)
{
    ui.chatTabWidget->setVisible(chatVisible);

    // adjust bottom panel colspan
    int colSpan = chatVisible ? 3 : 2;
    ui.gridLayout->addWidget(ui.bottomPanel, 1, 0, 1, colSpan);
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
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now - lastPerformanceMonitorUpdate >= PERFORMANCE_MONITOR_REFRESH_TIME) {
        ui.contentTabWidget->setResourcesUsage(performanceMonitor.getMemmoryUsed());
        lastPerformanceMonitorUpdate = now;
    }

    if (mainController->isPlayingInNinjamRoom())
        screensaverBlocker.update(); // prevent screen saver if user is playing

    // update room stream plot
    if (mainController->isPlayingRoomStream()) {
        long long roomID = mainController->getCurrentStreamingRoomID();
        JamRoomViewPanel *roomView = roomViewPanels[roomID];
        if (roomView) {
            bool buffering = mainController->getRoomStreamer()->isBuffering();
            roomView->setShowBufferingState(buffering);
            if (!buffering) {
                Audio::AudioPeak peak = mainController->getRoomStreamPeak();
                roomView->addPeak(peak.getMaxPeak());
            } else {
                int percentage = mainController->getRoomStreamer()->getBufferingPercentage();
                roomView->setBufferingPercentage(percentage);
            }
        }
    }

    // update master peaks
    Audio::AudioPeak masterPeak = mainController->getMasterPeak();
    ui.masterMeterL->setPeak(masterPeak.getLeftPeak(), masterPeak.getLeftRMS());
    ui.masterMeterR->setPeak(masterPeak.getRightPeak(), masterPeak.getRightRMS());
}

// ++++++++++++=
void MainWindow::resizeEvent(QResizeEvent *ev)
{
    Q_UNUSED(ev)
    if (busyDialog.isVisible())
        centerBusyDialog();

    if (ninjamWindow) {
        NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
        if (ninjamPanel) {
            bool usingLargeWindow = width() >= MAIN_WINDOW_MIN_SIZE.width();
            ninjamPanel->maximizeControlsWidget(usingLargeWindow);
        }
    }
}

void MainWindow::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::WindowStateChange && mainController) {
        mainController->storeWindowSettings(isMaximized(), computeLocation(), size());
    }
    else if (ev->type() == QEvent::LanguageChange) {
        ui.retranslateUi(this);
        if (ninjamWindow)
            updateChatTabTitle(); // translate the chat tab title
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
        mainController->storeWindowSettings(isMaximized(), computeLocation(), size());
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
                                            const QString &userName, const QString &password)
{
    mainController->storePrivateServerSettings(server, serverPort, password);
    mainController->setUserName(userName);
    ui.userNameLineEdit->setText(userName);

    Login::RoomInfo roomInfo(server, serverPort, Login::RoomTYPE::NINJAM, 32, 32);
    tryEnterInRoom(roomInfo, password);
}

void MainWindow::showJamtabaIssuesWebPage()
{
    openUrlInUserBrowser("https://github.com/elieserdejesus/JamTaba/issues");
}

void MainWindow::showJamtabaWikiWebPage()
{
    openUrlInUserBrowser("https://github.com/elieserdejesus/JamTaba/wiki");
}

void MainWindow::showJamtabaUsersManual()
{
    QString url = "https://github.com/elieserdejesus/JamTaba/wiki/Jamtaba's-user-guide";
    openUrlInUserBrowser(url);
}

void MainWindow::showJamtabaTranslators()
{
    QString url = "http://www.jamtaba.com/#translators";
    openUrlInUserBrowser(url);
}

void MainWindow::openUrlInUserBrowser(const QString &url)
{
    if (!QDesktopServices::openUrl(QUrl(url))) {
        QMessageBox::warning(this, tr("Error"), tr("Can't open the link!"), QMessageBox::Cancel, QMessageBox::Ok );
    }
}

void MainWindow::showPrivateServerDialog()
{
    PrivateServerDialog *privateServerDialog
        = new PrivateServerDialog(ui.centralWidget, mainController);

    connect(privateServerDialog, &PrivateServerDialog::connectionAccepted, this, &MainWindow::connectInPrivateServer);

    centerDialog(privateServerDialog);
    privateServerDialog->show();
    privateServerDialog->exec();
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
        PreferencesDialog::PreferencesTab initialTab = PreferencesDialog::TAB_RECORDING;
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

        qDebug(jtGUI) << "Initializing preferences dialog";
        dialog->initialize(initialTab, &mainController->getSettings(), mainController->getJamRecoders());// initializing here to avoid call virtual methods inside PreferencesDialog constructor

        qCDebug(jtGUI) << "Showing preferences dialog";
        dialog->show();
        dialog->exec();
    }
}

// +++++++++++++++++++++++++
void MainWindow::setupPreferencesDialogSignals(PreferencesDialog *dialog)
{
    Q_ASSERT(dialog);

    connect(dialog, SIGNAL(multiTrackRecordingStatusChanged(bool)), this,
            SLOT(setMultiTrackRecordingStatus(bool)));

    connect(dialog, SIGNAL(jamRecorderStatusChanged(QString, bool)), this,
            SLOT(setJamRecorderStatus(QString, bool)));

    connect(dialog, SIGNAL(recordingPathSelected(const QString &)), this,
            SLOT(setRecordingPath(const QString &)));

    connect(dialog, SIGNAL(builtInMetronomeSelected(QString)), this,
            SLOT(setBuiltInMetronome(QString)));

    connect(dialog, SIGNAL(customMetronomeSelected(QString, QString)), this,
            SLOT(setCustomMetronome(QString, QString)));

    connect(dialog, &PreferencesDialog::encodingQualityChanged, mainController, &MainController::setEncodingQuality);
}

void MainWindow::setBuiltInMetronome(const QString &metronomeAlias)
{
    mainController->setBuiltInMetronome(metronomeAlias);
}

void MainWindow::setCustomMetronome(const QString &primaryBeatFile,
                                    const QString &secondaryBeatFile)
{
    mainController->setCustomMetronome(primaryBeatFile, secondaryBeatFile);
}

void MainWindow::setMultiTrackRecordingStatus(bool recording)
{
    mainController->storeRecordingMultiTracksStatus(recording);
}

void MainWindow::setJamRecorderStatus(QString writerId, bool status)
{
    mainController->storeJamRecorderStatus(writerId, status);
}

void MainWindow::setRecordingPath(const QString &newRecordingPath)
{
    mainController->storeRecordingPath(newRecordingPath);
}

// ++++++++++++++++++++++

void MainWindow::initializeViewMenu()
{
    connect(ui.menuMetering, SIGNAL(aboutToShow()), this, SLOT(updateMeteringMenu()));

    connect(ui.menuMetering, SIGNAL(triggered(QAction*)), this, SLOT(handleMenuMeteringAction(QAction*)));

    QActionGroup *meteringActionGroup = new QActionGroup(this);
    meteringActionGroup->addAction(ui.actionShowPeaksOnly);
    meteringActionGroup->addAction(ui.actionShowRmsOnly);
    meteringActionGroup->addAction(ui.actionShowPeakAndRMS);
}

void MainWindow::handleMenuMeteringAction(QAction *action)
{
    if (action == ui.actionShowMaxPeaks){
        AudioMeter::setPaintMaxPeakMarker(ui.actionShowMaxPeaks->isChecked());
    }
    else{
        if (action == ui.actionShowPeakAndRMS){
            AudioMeter::paintPeaksAndRms();
        }
        else if (action == ui.actionShowPeaksOnly){
            AudioMeter::paintPeaksOnly();
        }
        else{ //RMS only
            AudioMeter::paintRmsOnly();
        }
    }
    quint8 meterOption = 0; //rms + peaks
    if (AudioMeter::isPaintingPeaksOnly())
        meterOption = 1;
    else if (AudioMeter::isPaintingRmsOnly())
        meterOption = 2;

    mainController->storeMeteringSettings(AudioMeter::isPaintintMaxPeakMarker(), meterOption);
}

void MainWindow::updateMeteringMenu()
{
    ui.actionShowMaxPeaks->setChecked(AudioMeter::isPaintintMaxPeakMarker());
    bool showingPeakAndRms = AudioMeter::isPaintingRMS() && AudioMeter::isPaintingPeaks();
    if (showingPeakAndRms) {
        ui.actionShowPeakAndRMS->setChecked(true);
    }
    else{
        ui.actionShowPeaksOnly->setChecked(AudioMeter::isPaintingPeaks());
        ui.actionShowRmsOnly->setChecked(AudioMeter::isPaintingRMS());
    }
}

void MainWindow::updatePublicRoomsListLayout()
{
    QList<Login::RoomInfo> roomInfos;

    for (JamRoomViewPanel *roomView: roomViewPanels)
        roomInfos.append(roomView->getRoomInfo());

    refreshPublicRoomsList(roomInfos);
}

QSize MainWindow::getSanitizedWindowSize(const QSize &size, const QSize &minimumSize) const
{
    // fixing #343. If MainWindow::showFullScreen() is called after a setMinimumSize(), and the current
    // minimum size is less then desktop size the fullScreen is buggy because we are forcing an
    // impossible minimum size (the minimum size is too large). This code is ensuring the minimum
    // window size is a valid window size.
    QDesktopWidget desktop;

    QSize screenSize = desktop.availableGeometry().size();

    static int topBarHeight = 0;
    if (topBarHeight == 0) // when the window is fullscreen the topBarHeight is zero (no title window).
        topBarHeight = frameGeometry().height() - geometry().height(); // geometry is the 'window client area', frameGeometry contain the window title bar area. http://doc.qt.io/qt-4.8/application-windows.html#window-geometry

    const int minimumWidth = minimumSize.width();
    const int maximumWidth = screenSize.width();
    const int minimumHeight = minimumSize.height();
    const int maximumHeight = screenSize.height() - topBarHeight;

    QSize finalSize(size);
    if (finalSize.width() < minimumWidth)
        finalSize.setWidth(minimumWidth);
    else
        if (finalSize.width() > maximumWidth)
            finalSize.setWidth(maximumWidth);

    if (finalSize.height() < minimumHeight)
        finalSize.setHeight(minimumHeight);
    else
        if (finalSize.height() > maximumHeight)
            finalSize.setHeight(maximumHeight);

    return finalSize;
}

void MainWindow::initializeWindowSize()
{
    setMinimumSize(getMinimumWindowSize());

    if (!isMaximized() && !isFullScreen()) {
        QSize lastSize = mainController->getSettings().getLastWindowSize();
        QSize newSize = getSanitizedWindowSize(lastSize, minimumSize());
        resize(newSize);
        showNormal();
    }

    const int spaces = 6;
    ui.tabLayout->setContentsMargins(spaces, spaces, spaces, spaces);
    ui.allRoomsContent->layout()->setSpacing(spaces);

    ui.chatTabWidget->setMinimumWidth(230); // TODO Refactoring: remove these 'Magic Numbers'

    // local tracks are narrowed in mini mode if user is using more than 1 subchannel
    bool usingSmallWindow = width() < MAIN_WINDOW_MIN_SIZE.width();
    foreach (LocalTrackGroupView *localTrackGroup, localGroupChannels) {
        if (usingSmallWindow && (localTrackGroup->getTracksCount() > 1 || localGroupChannels.size() > 1))
            localTrackGroup->setToNarrow();
        else
            localTrackGroup->setToWide();
    }

    if (ninjamWindow) {
        ChatPanel *chatPanel = ninjamWindow->getChatPanel();
        if (chatPanel)
            chatPanel->updateMessagesGeometry();
    }
}

// +++++++++++++++++++++++++++
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::Resize) {
        if (target == ui.localTracksWidget) {
            updateLocalInputChannelsGeometry();
            return true;
        }
        else if (target == ui.contentTabWidget) {
            updatePublicRoomsListLayout();
            return false;
        }
    } else {
        if (target == ui.masterFader && event->type() == QEvent::MouseButtonDblClick) {
            ui.masterFader->setValue(100);
            return true;
        }
    }
    return QMainWindow::eventFilter(target, event);
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
    QMessageBox *box = new QMessageBox();
    box->setWindowTitle(title);
    box->setText(text);
    box->deleteLater();
    box->exec();
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

void MainWindow::acceptChordProgression(const ChordProgression &progression)
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
        QGridLayout *bottomLayout = dynamic_cast<QGridLayout *>(ui.bottomPanel->layout());
        if (bottomLayout) {
            bottomLayout->addWidget(chordsPanel, 0, 0, 1, 1);
            if (ninjamWindow) {
                NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
                bottomLayout->addWidget(ninjamPanel, 1, 0, 1, 1, Qt::AlignCenter);
                ninjamPanel->setLowContrastPaintInIntervalPanel(true);
            }
        }

        sendAcceptedChordProgressionToServer(progression);

    } else {
        int measures = progression.getMeasures().size();
        QString msg = tr("These chords (%1 measures) can't be used in a %2 bpi interval!")
                      .arg(QString::number(measures))
                      .arg(QString::number(currentBpi));
        QMessageBox::warning(this, tr("Problem..."), msg);
    }
}

void MainWindow::sendAcceptedChordProgressionToServer(const ChordProgression &progression)
{
    Login::LoginService *service = mainController->getLoginService();
    Login::RoomInfo currentRoom = ninjamWindow->getRoomInfo();
    QString chords = progression.toString();
    bool chordProgressionIsOutdated = service->getChordProgressionFor(currentRoom) != chords;
    if (chordProgressionIsOutdated) {
        QString userName = mainController->getUserName();
        QString serverName = currentRoom.getName();
        quint32 serverPort = currentRoom.getPort();
        service->sendChordProgressionToServer(userName, serverName, serverPort, chords);
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

    setChatVisibility(false);// hide chat area until connect in a server to play

    if (ui.allRoomsContent->layout())
        delete ui.allRoomsContent->layout();

    ui.allRoomsContent->setLayout(new QGridLayout());
    ui.allRoomsContent->layout()->setContentsMargins(0, 0, 0, 0);

    ui.localTracksWidget->installEventFilter(this);

    QString lastUserName = mainController->getUserName();
    if (!lastUserName.isEmpty())
        ui.userNameLineEdit->setText(lastUserName);
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

    connect(ui.actionTranslators, SIGNAL(triggered(bool)), this, SLOT(showJamtabaTranslators()));

    connect(ui.actionCurrentVersion, SIGNAL(triggered(bool)), this,
            SLOT(showJamtabaCurrentVersion()));

    connect(ui.localControlsCollapseButton, SIGNAL(clicked()), this,
            SLOT(toggleLocalInputsCollapseStatus()));

    connect(mainController->getRoomStreamer(), SIGNAL(error(QString)), this,
            SLOT(handlePublicRoomStreamError(QString)));

    connect(ui.masterFader, SIGNAL(valueChanged(int)), this,
            SLOT(setMasterGain(int)));

    connect(ui.actionQuit, SIGNAL(triggered(bool)), this, SLOT(close()));

    connect(ui.menuLanguage, SIGNAL(triggered(QAction *)), this, SLOT(setLanguage(QAction *)));

    connect(ui.userNameLineEdit, SIGNAL(editingFinished()), this, SLOT(updateUserName()));

    connect(mainController, &Controller::MainController::themeChanged, this, &MainWindow::updateNightModeInWorldMaps);

    ui.contentTabWidget->installEventFilter(this);
}

void MainWindow::updateUserName()
{
    QString newUserName = ui.userNameLineEdit->text();
    mainController->setUserName(newUserName);
}

void MainWindow::initializeMasterFader()
{
    float lastMasterGain = mainController->getSettings().getLastMasterGain();
    int faderPosition = lastMasterGain * 100;
    ui.masterFader->setValue(faderPosition);
    setMasterGain(faderPosition);
}

