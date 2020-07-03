#include "MainWindowStandalone.h"
#include "PreferencesDialogStandalone.h"
#include "gui/NinjamRoomWindow.h"
#include "gui/LocalTrackView.h"
#include "gui/CrashReportDialog.h"
#include "gui/PluginScanDialog.h"
#include "log/Logging.h"
#include "audio/core/PluginDescriptor.h"
#include "vst/VstPluginFinder.h"
#include "vst/VstPlugin.h"

#include <QTimer>
#include <QDesktopWidget>
#include <QSharedPointer>
#include <QShortcut>
#include <QSettings>

using persistence::SubChannel;
using persistence::Channel;
using persistence::LocalInputTrackSettings;

MainWindowStandalone::MainWindowStandalone(MainControllerStandalone *mainController) :
    MainWindow(mainController),
    controller(mainController),
    fullScreenViewMode(false),
    pluginScanDialog(nullptr),
    preferencesDialog(nullptr)
{
    setupSignals();

    setupShortcuts();

    initializePluginFinder();
}

void MainWindowStandalone::initialize()
{
    MainWindow::initialize();

#ifdef Q_OS_WIN
    if (MainController::crashedInLastExecution()) {
        CrashReportDialog dialog(this);
        dialog.setLogDetails(Configurator::getInstance()->getPreviousLogContent());
        dialog.exec();
    }
#endif
}

TextEditorModifier *MainWindowStandalone::createTextEditorModifier()
{
    return nullptr;
}

void MainWindowStandalone::setupShortcuts()
{
    ui.actionAudioPreferences->setShortcut(QKeySequence(Qt::Key_F5));
    ui.actionMidiPreferences->setShortcut(QKeySequence(Qt::Key_F6));
    ui.actionVstPreferences->setShortcut(QKeySequence(Qt::Key_F7));
    ui.actionRecording->setShortcut(QKeySequence(Qt::Key_F8));
    ui.actionMetronome->setShortcut(QKeySequence(Qt::Key_F9));
    ui.actionLooper->setShortcut(QKeySequence(Qt::Key_F10));
    ui.actionUsersManual->setShortcut(QKeySequence(Qt::Key_F1));
    ui.actionConnectWithPrivateServer->setShortcut(QKeySequence(Qt::Key_F2));
    ui.actionShowRmsOnly->setShortcut(QKeySequence(Qt::Key_F3));
    ui.actionShowPeaksOnly->setShortcut(QKeySequence(Qt::Key_F4));
    ui.actionFullscreenMode->setShortcut(QKeySequence(Qt::Key_F11));

    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(tryClose()));
}

void MainWindowStandalone::tryClose() // this function is called when ESC is pressed
{
    if (!mainController)
        return;

    if (mainController->isPlayingInNinjamRoom())
        mainController->stopNinjamController(); // exit from server if user is jamming
    else
        close(); // close JamTaba if user is not jamming
}

void MainWindowStandalone::setupSignals()
{
    connect(ui.actionFullscreenMode, &QAction::triggered, this,
            &MainWindowStandalone::toggleFullScreen);

    auto pluginFinder = controller->getVstPluginFinder();
    if (!pluginFinder)
        return;

    connect(pluginFinder, &VSTPluginFinder::scanStarted, this,
            &MainWindowStandalone::showPluginScanDialog);

    connect(pluginFinder, &VSTPluginFinder::scanFinished, this,
            &MainWindowStandalone::hidePluginScanDialog);

    connect(pluginFinder, &VSTPluginFinder::badPluginDetected, this,
            &MainWindowStandalone::addPluginToBlackList);

    connect(pluginFinder, &VSTPluginFinder::pluginScanFinished, this,
            &MainWindowStandalone::showFoundedVstPlugin);

    connect(pluginFinder, &VSTPluginFinder::pluginScanStarted, this,
            &MainWindowStandalone::setCurrentScanningPlugin);
}

void MainWindowStandalone::doWindowInitialization()
{
    MainWindow::doWindowInitialization();

    auto settings = mainController->getSettings();
    if (settings.windowsWasFullScreenViewMode()) {
        setFullScreenStatus(true);
    } else { // not full screen. Is maximized or normal?
        if (settings.windowWasMaximized()) {
            qCDebug(jtGUI)<< "setting window state to maximized";
            showMaximized();
        }
        readWindowSettings(settings.windowWasMaximized()); // restore saved position, size and state settings for standalone app.
    }
}

void MainWindowStandalone::restoreWindowPosition()
{
    QPointF location = mainController->getSettings().getLastWindowLocation();
    auto desktop = QApplication::desktop();

    int desktopWidth = desktop->width();
    int desktopHeight = desktop->height();
    int x = desktopWidth * location.x();
    int y = desktopHeight * location.y();

    this->move(x, y);

    qCDebug(jtGUI)<< "Restoring window to position:" << x << ", " << y;
    qCDebug(jtGUI)<< "Window size:" << width() << ", " << height();
}

// plugin finder events
void MainWindowStandalone::showPluginScanDialog()
{
    if (preferencesDialog && preferencesDialog->isVisible())
        return; // only show the Plugin Scan dialog if PreferencesDialog is not opened (to avoid two modal dialogs)

    if (!pluginScanDialog) {
        pluginScanDialog = new PluginScanDialog(this);
        connect(pluginScanDialog, SIGNAL(rejected()), this, SLOT(closePluginScanDialog()));
    }

    pluginScanDialog->show();
}

void MainWindowStandalone::closePluginScanDialog()
{
    controller->cancelPluginFinders();
    pluginScanDialog = nullptr; // reset to null pointer
}

void MainWindowStandalone::hidePluginScanDialog(bool finishedWithoutError)
{
    Q_UNUSED(finishedWithoutError);
    if (pluginScanDialog) {
        pluginScanDialog->close();
        pluginScanDialog->deleteLater();
    }

    pluginScanDialog = nullptr;
}

void MainWindowStandalone::addPluginToBlackList(const QString &pluginPath)
{
    QString pluginName = audio::PluginDescriptor::getVstPluginNameFromPath(pluginPath);
    QWidget *parent = this;
    if (pluginScanDialog)
        parent = pluginScanDialog;
    QString message = tr("%1 can't be loaded and will be black listed!").arg(pluginName);
    QMessageBox::warning(parent, tr("Plugin Error!"), message);
    controller->addBlackVstToSettings(pluginPath);
}

void MainWindowStandalone::showFoundedVstPlugin(const QString &name, const QString &path)
{
    Q_UNUSED(path);

    if (pluginScanDialog)
        pluginScanDialog->addFoundedPlugin(name);
}

void MainWindowStandalone::setCurrentScanningPlugin(const QString &pluginPath)
{
    if (pluginScanDialog)
        pluginScanDialog->setCurrentScaning(pluginPath);
}

bool MainWindowStandalone::midiDeviceIsValid(int deviceIndex) const
{
    return deviceIndex < controller->getMidiDriver()->getMaxInputDevices();
}

void MainWindowStandalone::setFullScreenStatus(bool fullScreen)
{
    fullScreenViewMode = fullScreen;
    if (fullScreen)
        showFullScreen();
    else
        showNormal();
    mainController->setFullScreenView(fullScreenViewMode);
    ui.actionFullscreenMode->setChecked(fullScreen);

    QApplication::processEvents(); // process the window resize pending events before call setFullViewStatus and resize the JTB window

    // setFullViewStatus(isRunningInFullViewMode()); //update the window size

    updatePublicRoomsListLayout();
}

void MainWindowStandalone::toggleFullScreen()
{
    setFullScreenStatus(!fullScreenViewMode);// toggle
}

// sanitize the input selection for each loaded subchannel
void MainWindowStandalone::sanitizeSubchannelInputSelections(LocalTrackView *subChannelView, const SubChannel &subChannel)
{
    int trackID = subChannelView->getInputIndex();
    if (subChannel.isMidi()) {
        qint8 transpose = subChannel.transpose;
        quint8 lowerNote = subChannel.lowerMidiNote;
        quint8 higherNote = subChannel.higherMidiNote;

        if (midiDeviceIsValid(subChannel.midiDevice)) {
            controller->setInputTrackToMIDI(trackID, subChannel.midiDevice, subChannel.midiChannel,
                                            transpose, lowerNote, higherNote);
        } else {
            if (controller->getMidiDriver()->hasInputDevices()) {
                // use the first midi device and receiving from all channels
                controller->setInputTrackToMIDI(trackID, 0, -1, transpose, lowerNote, higherNote);
            } else {
                controller->setInputTrackToNoInput(trackID);
            }
        }
    } else if (subChannel.isNoInput()) {
        controller->setInputTrackToNoInput(trackID);
    } else if (subChannel.isMono()) {
        controller->setInputTrackToMono(trackID, subChannel.firstInput);
    } else {
        controller->setInputTrackToStereo(trackID, subChannel.firstInput);
    }
}

void MainWindowStandalone::restoreLocalSubchannelPluginsList(
    LocalTrackViewStandalone *subChannelView, const SubChannel &subChannel)
{
    // create the plugins list
    for (const auto &plugin : subChannel.getPlugins()) {
        auto category = static_cast<audio::PluginDescriptor::Category>(plugin.category);

        audio::PluginDescriptor descriptor(plugin.name, category, plugin.manufacturer, plugin.path);
        quint32 inputTrackIndex = subChannelView->getInputIndex();
        qint32 pluginSlotIndex = subChannelView->getPluginFreeSlotIndex();
        if (pluginSlotIndex >= 0) {
            auto pluginInstance
                = controller->addPlugin(inputTrackIndex, pluginSlotIndex, descriptor);
            if (pluginInstance) {
                try
                {
                    pluginInstance->restoreFromSerializedData(plugin.data);
                }
                catch (...)
                {
                    qWarning() << "Exception restoring " << pluginInstance->getName();
                }
                subChannelView->addPlugin(pluginInstance, pluginSlotIndex, plugin.bypassed);
            } else {
                qCritical() << "can´t create plugin instance! " << plugin.name;
            }
            // QApplication::processEvents();
        }
    }
}

void MainWindowStandalone::initializeLocalSubChannel(LocalTrackView *subChannelView, const SubChannel &subChannel)
{
    // load channels names, gain, pan, boost, mute
    MainWindow::initializeLocalSubChannel(subChannelView, subChannel);

    // check if the loaded input selections (midi, audio mono, audio stereo) are stil valid and fallback if not
    sanitizeSubchannelInputSelections(subChannelView, subChannel);

    auto trackView = dynamic_cast<LocalTrackViewStandalone *>(subChannelView);

    restoreLocalSubchannelPluginsList(trackView, subChannel);

    trackView->setMidiRouting(subChannel.routingMidiToFirstSubchannel);
}

LocalTrackGroupViewStandalone *MainWindowStandalone::createLocalTrackGroupView(int channelGroupIndex)
{
    return new LocalTrackGroupViewStandalone(channelGroupIndex, this);
}

QList<persistence::Plugin> buildPersistentPluginList(QList<const audio::Plugin *> trackPlugins)
{
    QList<persistence::Plugin> persistentPlugins;
    for (auto p : trackPlugins) {
        QByteArray serializedData = p->getSerializedData();
        persistence::Plugin plugin(p->getDescriptor(), p->isBypassed(), serializedData);
        persistentPlugins.append(plugin);
    }
    return persistentPlugins;
}

LocalInputTrackSettings MainWindowStandalone::getInputsSettings() const
{
    // the base class is returning just the basic: gain, mute, pan , etc for each channel and subchannel
    LocalInputTrackSettings baseSettings = MainWindow::getInputsSettings();

    // recreate the settings including the plugins
    LocalInputTrackSettings settings;
    QList<LocalTrackGroupViewStandalone *> groups
        = getLocalChannels<LocalTrackGroupViewStandalone *>();
    Q_ASSERT(groups.size() == baseSettings.channels.size());

    int channelID = 0;
    for (const Channel &channel : baseSettings.channels) {
        LocalTrackGroupViewStandalone *trackGroupView = groups.at(channelID++);
        if (!trackGroupView)
            continue;
        Channel newChannel = channel;
        newChannel.subChannels.clear();
        int subChannelID = 0;
        QList<LocalTrackViewStandalone *> trackViews
            = trackGroupView->getTracks<LocalTrackViewStandalone *>();
        for (SubChannel subchannel : channel.subChannels) {
            SubChannel newSubChannel = subchannel;
            LocalTrackViewStandalone *trackView = trackViews.at(subChannelID);
            if (trackView)
                newSubChannel.setPlugins(buildPersistentPluginList(trackView->getInsertedPlugins()));

            subChannelID++;
            newChannel.subChannels.append(newSubChannel);
        }
        settings.channels.append(newChannel);
    }

    return settings;
}

NinjamRoomWindow *MainWindowStandalone::createNinjamWindow(const login::RoomInfo &roomInfo, MainController *mainController)
{
    return new NinjamRoomWindow(this, roomInfo, mainController);
}

void MainWindowStandalone::closeEvent(QCloseEvent *e)
{
    writeWindowSettings(); // save windows pos, size and state using qt high level API for the standalone

    MainWindow::closeEvent(e);
    hide(); // hide before stop main controller and disconnect from login server

    for (LocalTrackGroupView *trackGroup : localGroupChannels)
        trackGroup->closePluginsWindows();
}

PreferencesDialog *MainWindowStandalone::createPreferencesDialog()
{
    qDebug(jtGUI) << "Creating preferences dialog";

    // stop midi and audio before show the preferences dialog
    auto midiDriver = controller->getMidiDriver();
    auto audioDriver = controller->getAudioDriver();

    Q_ASSERT(midiDriver);
    Q_ASSERT(audioDriver);

    audioDriver->stop(true);    // asking audio driver to refresh the devices list
    midiDriver->stop();

    bool showAudioControlPanelButton = controller->getAudioDriver()->hasControlPanel();
    auto dialog = new PreferencesDialogStandalone(this, showAudioControlPanelButton, audioDriver,
                                                  midiDriver);

    // setup signals related with recording and metronome preferences
    MainWindow::setupPreferencesDialogSignals(dialog);

    // setup standalone specific signals
    connect(dialog, &PreferencesDialogStandalone::ioPreferencesChanged, this,
            &MainWindowStandalone::setGlobalPreferences);

    connect(dialog, &PreferencesDialogStandalone::rejected, this,
            &MainWindowStandalone::restartAudioAndMidi);

    connect(dialog, &PreferencesDialogStandalone::sampleRateChanged, controller,
            &MainControllerStandalone::setSampleRate);
    connect(dialog, &PreferencesDialogStandalone::bufferSizeChanged, controller,
            &MainControllerStandalone::setBufferSize);

    VSTPluginFinder *vstFinder = controller->getVstPluginFinder();
    connect(vstFinder, &VSTPluginFinder::scanFinished, dialog,
            &PreferencesDialogStandalone::populateVstTab);
    connect(vstFinder, &VSTPluginFinder::scanStarted, dialog,
            &PreferencesDialogStandalone::clearVstList);
    connect(vstFinder, &VSTPluginFinder::pluginScanFinished, dialog,
            &PreferencesDialogStandalone::addFoundedVstPlugin);
    connect(vstFinder, &VSTPluginFinder::pluginScanStarted, dialog,
            &PreferencesDialogStandalone::setCurrentScannedVstPlugin);

    connect(dialog, &PreferencesDialogStandalone::vstScanDirRemoved, controller,
            &MainControllerStandalone::removePluginsScanPath);
    connect(dialog, &PreferencesDialogStandalone::vstScanDirAdded, controller,
            &MainControllerStandalone::addPluginsScanPath);

    connect(dialog, &PreferencesDialogStandalone::vstPluginAddedInBlackList, controller,
            &MainControllerStandalone::addBlackVstToSettings);
    connect(dialog, &PreferencesDialogStandalone::vstPluginRemovedFromBlackList, controller,
            &MainControllerStandalone::removeBlackVstFromSettings);

    connect(dialog, &PreferencesDialogStandalone::startingFullPluginsScan, controller,
            &MainControllerStandalone::scanAllVstPlugins);
    connect(dialog, &PreferencesDialogStandalone::startingOnlyNewPluginsScan, controller,
            &MainControllerStandalone::scanOnlyNewVstPlugins);

    connect(dialog, &PreferencesDialogStandalone::openingExternalAudioControlPanel, controller,
            &MainControllerStandalone::openExternalAudioControlPanel);

    preferencesDialog = dialog; // store the dialog instance to use when showing Vst Plugin Scan Dialog - issue #670

    return dialog;
}

void MainWindowStandalone::restartAudioAndMidi()
{
    // restart audio and midi drivers when user is cancelling the preferences dialog
    auto midiDriver = controller->getMidiDriver();
    auto audioDriver = controller->getAudioDriver();

    Q_ASSERT(midiDriver);
    Q_ASSERT(audioDriver);

    midiDriver->start(controller->getSettings().getMidiInputDevicesStatus());
    audioDriver->start();
}

void MainWindowStandalone::initializePluginFinder()
{
    const auto &settings = controller->getSettings();

    controller->clearPluginsList();

    controller->initializeVstPluginsList(settings.getVstPluginsPaths()); // load the cached plugins. The cache can be empty.

#ifdef Q_OS_MAC
    controller->initializeAudioUnitPluginsList(settings.getAudioUnitsPaths());

    // always checking for new AU plugins
    controller->scanAudioUnitPlugins();
#endif

    // checking for new plugins...
    if (controller->vstScanIsNeeded()) { // no vsts in database cache or new plugins detected in scan folders?
        if (settings.getVstScanFolders().isEmpty())
            controller->addDefaultPluginsScanPath();
        controller->scanOnlyNewVstPlugins();
    }
}

void MainWindowStandalone::setGlobalPreferences(const QList<bool> &midiInputsStatus, QString audioInputDevice, QString audioOutputDevice, int firstIn, int lastIn,
                                                int firstOut, int lastOut)
{
    qDebug(jtGUI) << "Setting global preferences ...";

    auto audioDriver = controller->getAudioDriver();

    audioDriver->setProperties(firstIn, lastIn, firstOut, lastOut);
    controller->storeIOSettings(firstIn, lastIn, firstOut, lastOut, audioInputDevice,
                                audioOutputDevice, midiInputsStatus);

    auto midiDriver = controller->getMidiDriver();
    midiDriver->setDevicesStatus(midiInputsStatus);

    controller->updateInputTracksRange();

    for (auto channel : getLocalChannels<LocalTrackGroupViewStandalone *>())
        channel->refreshInputSelectionNames();

    midiDriver->start(midiInputsStatus);
    if (!audioDriver->start()) {
        qCritical() << "Error starting audio device";
        QMessageBox::warning(this, tr("Audio error!"),
                             tr(
                                 "The audio device can't be started! Please check your audio device and try restart Jamtaba!"));
        // controller->useNullAudioDriver();
    }
}

// input selection changed by user or by system
void MainWindowStandalone::refreshTrackInputSelection(int inputTrackIndex)
{
    for (auto channel : getLocalChannels<LocalTrackGroupViewStandalone *>())
        channel->refreshInputSelectionName(inputTrackIndex);
}

void MainWindowStandalone::addChannelsGroup(int instrumentIndex)
{
    MainWindow::addChannelsGroup(instrumentIndex);
    controller->updateInputTracksRange();
}

void MainWindowStandalone::readWindowSettings(bool isWindowMaximized)
{
    QSettings settings(QCoreApplication::applicationName());

    QPoint pos = settings.value("pos", QPoint(50, 50)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    QByteArray state = settings.value("state", QByteArray())
                       .toByteArray();
    restoreState(state);
    if (!isWindowMaximized) {
        resize(size);
        move(pos);
    }
}

void MainWindowStandalone::writeWindowSettings()
{
    /* Save position/size of main window */
    QSettings settings(QCoreApplication::applicationName());

    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("state", saveState());
}
