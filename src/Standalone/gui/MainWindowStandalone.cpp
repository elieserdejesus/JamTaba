#include "MainWindowStandalone.h"
#include "PreferencesDialogStandalone.h"
#include "NinjamRoomWindow.h"
#include "LocalTrackView.h"
#include "log/Logging.h"
#include "audio/core/PluginDescriptor.h"

#include <QTimer>
#include <QDesktopWidget>

using namespace Persistence;
using namespace Controller;
using namespace audio;  // TODO rewrite namespaces using lower case

MainWindowStandalone::MainWindowStandalone(MainControllerStandalone *mainController) :
    MainWindow(mainController),
    controller(mainController),
    fullScreenViewMode(false),
    pluginScanDialog(nullptr)
{
    setupSignals();

    setupShortcuts();

    initializePluginFinder();
}

void MainWindowStandalone::setupShortcuts()
{
    ui.actionAudioPreferences->setShortcut(QKeySequence(Qt::Key_F5));
    ui.actionMidiPreferences->setShortcut(QKeySequence(Qt::Key_F6));
    ui.actionVstPreferences->setShortcut(QKeySequence(Qt::Key_F7));
    ui.actionRecording->setShortcut(QKeySequence(Qt::Key_F8));
    ui.actionMetronome->setShortcut(QKeySequence(Qt::Key_F9));
    ui.actionUsersManual->setShortcut(QKeySequence(Qt::Key_F1));
    ui.actionPrivate_Server->setShortcut(QKeySequence(Qt::Key_F2));
    ui.actionShowRmsOnly->setShortcut(QKeySequence(Qt::Key_F3));
    ui.actionShowPeaksOnly->setShortcut(QKeySequence(Qt::Key_F4));
    ui.actionQuit->setShortcut(QKeySequence(Qt::Key_Escape));
    ui.actionFullscreenMode->setShortcut(QKeySequence(Qt::Key_F11));
}

void MainWindowStandalone::setupSignals()
{
    connect(ui.actionFullscreenMode, &QAction::triggered, this, &MainWindowStandalone::toggleFullScreen);

     audio::PluginFinder *pluginFinder = controller->getVstPluginFinder();
    Q_ASSERT(pluginFinder);

    connect(pluginFinder, &VSTPluginFinder::scanStarted, this, &MainWindowStandalone::showPluginScanDialog);

    connect(pluginFinder, &VSTPluginFinder::scanFinished, this, &MainWindowStandalone::hidePluginScanDialog);

    connect(pluginFinder, &VSTPluginFinder::badPluginDetected, this, &MainWindowStandalone::addPluginToBlackList);

    connect(pluginFinder, &VSTPluginFinder::pluginScanFinished, this, &MainWindowStandalone::showFoundedVstPlugin);

    connect(pluginFinder, &VSTPluginFinder::pluginScanStarted, this, &MainWindowStandalone::setCurrentScanningPlugin);
}

void MainWindowStandalone::doWindowInitialization()
{
    MainWindow::doWindowInitialization();

    Persistence::Settings settings = mainController->getSettings();
    if (settings.windowsWasFullScreenViewMode()) {
        setFullScreenStatus(true);
    } else {// not full screen. Is maximized or normal?
        if (settings.windowWasMaximized()) {
            qCDebug(jtGUI)<< "setting window state to maximized";
            showMaximized();
        } else {
            restoreWindowPosition();
        }
    }

}

void MainWindowStandalone::restoreWindowPosition()
{
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

// plugin finder events
void MainWindowStandalone::showPluginScanDialog()
{
    if (!pluginScanDialog) {
        pluginScanDialog.reset(new PluginScanDialog(this));
        QObject::connect(pluginScanDialog.data(), SIGNAL(rejected()), this,
                         SLOT(closePluginScanDialog()));
    }
    pluginScanDialog->show();
}

void MainWindowStandalone::closePluginScanDialog()
{
    controller->cancelPluginFinders();
    pluginScanDialog.reset();// reset to null pointer
}

void MainWindowStandalone::hidePluginScanDialog(bool finishedWithoutError)
{
    Q_UNUSED(finishedWithoutError);
    if (pluginScanDialog)
        pluginScanDialog->close();
    pluginScanDialog.reset();
}

void MainWindowStandalone::addPluginToBlackList(const QString &pluginPath)
{
    QString pluginName = Audio::PluginDescriptor::getVstPluginNameFromPath(pluginPath);
    QWidget *parent = this;
    if (pluginScanDialog)
        parent = pluginScanDialog.data();
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

    QApplication::processEvents(); //process the window resize pending events before call setFullViewStatus and resize the JTB window

    setFullViewStatus(isRunningInFullViewMode()); //update the window size

    updatePublicRoomsListLayout();
}

void MainWindowStandalone::toggleFullScreen()
{
    setFullScreenStatus(!fullScreenViewMode);// toggle
}

// sanitize the input selection for each loaded subchannel
void MainWindowStandalone::sanitizeSubchannelInputSelections(LocalTrackView *subChannelView,
                                                             const Subchannel &subChannel)
{
    int trackID = subChannelView->getInputIndex();
    if (subChannel.isMidi()) {
        qint8 transpose = subChannel.transpose;
        quint8 lowerNote = subChannel.lowerMidiNote;
        quint8 higherNote = subChannel.higherMidiNote;

        if (midiDeviceIsValid(subChannel.midiDevice)) {
            controller->setInputTrackToMIDI(trackID, subChannel.midiDevice, subChannel.midiChannel, transpose, lowerNote, higherNote);
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
    LocalTrackViewStandalone *subChannelView, const Subchannel &subChannel)
{
    // create the plugins list
    foreach (const Persistence::Plugin &plugin, subChannel.getPlugins()) { 
        Audio::PluginDescriptor::Category category = static_cast<Audio::PluginDescriptor::Category>(plugin.category);

        Audio::PluginDescriptor descriptor(plugin.name, category, plugin.manufacturer, plugin.path);
        quint32 inputTrackIndex = subChannelView->getInputIndex();
        qint32 pluginSlotIndex = subChannelView->getPluginFreeSlotIndex();
        if (pluginSlotIndex >= 0) {
            Audio::Plugin *pluginInstance = controller->addPlugin(inputTrackIndex, pluginSlotIndex, descriptor);
            if (pluginInstance) {
                try{
                    pluginInstance->restoreFromSerializedData(plugin.data);
                }
                catch (...) {
                    qWarning() << "Exception restoring " << pluginInstance->getName();
                }
                subChannelView->addPlugin(pluginInstance, pluginSlotIndex, plugin.bypassed);
            }
            else {
                qCritical() << "can´t create plugin instance! " << plugin.name;
            }
            //QApplication::processEvents();
        }
    }
}

void MainWindowStandalone::initializeLocalSubChannel(LocalTrackView *subChannelView,
                                                     const Subchannel &subChannel)
{
    // load channels names, gain, pan, boost, mute
    MainWindow::initializeLocalSubChannel(subChannelView, subChannel);

    // check if the loaded input selections (midi, audio mono, audio stereo) are stil valid and fallback if not
    sanitizeSubchannelInputSelections(subChannelView, subChannel);

    restoreLocalSubchannelPluginsList(dynamic_cast<LocalTrackViewStandalone *>(subChannelView),
                                      subChannel);
}

LocalTrackGroupViewStandalone *MainWindowStandalone::createLocalTrackGroupView(int channelGroupIndex)
{
    return new LocalTrackGroupViewStandalone(channelGroupIndex, this);
}

QList<Persistence::Plugin> buildPersistentPluginList(QList<const Audio::Plugin *> trackPlugins)
{
    QList<Persistence::Plugin> persistentPlugins;
    foreach (const Audio::Plugin *p, trackPlugins) {
        QByteArray serializedData = p->getSerializedData();
        Persistence::Plugin plugin(p->getDescriptor(), p->isBypassed(), serializedData);
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
    foreach (const Channel &channel, baseSettings.channels) {
        LocalTrackGroupViewStandalone *trackGroupView = groups.at(channelID++);
        if (!trackGroupView)
            continue;
        Channel newChannel = channel;
        newChannel.subChannels.clear();
        int subChannelID = 0;
        QList<LocalTrackViewStandalone *> trackViews
            = trackGroupView->getTracks<LocalTrackViewStandalone *>();
        foreach (Subchannel subchannel, channel.subChannels) {
            Subchannel newSubChannel = subchannel;
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

NinjamRoomWindow *MainWindowStandalone::createNinjamWindow(const Login::RoomInfo &roomInfo,
                                                           MainController *mainController)
{
    return new NinjamRoomWindow(this, roomInfo, mainController);
}

// ++++++++++++++++++++++++++++

void MainWindowStandalone::closeEvent(QCloseEvent *e)
{
    MainWindow::closeEvent(e);
    hide();// hide before stop main controller and disconnect from login server

    foreach (LocalTrackGroupView *trackGroup, localGroupChannels)
        trackGroup->closePluginsWindows();
}

PreferencesDialog *MainWindowStandalone::createPreferencesDialog()
{

    qDebug(jtGUI) << "Creating preferences dialog";

    // stop midi and audio before show the preferences dialog
    Midi::MidiDriver *midiDriver = controller->getMidiDriver();
    Audio::AudioDriver *audioDriver = controller->getAudioDriver();

    Q_ASSERT(midiDriver);
    Q_ASSERT(audioDriver);

    audioDriver->stop(true);    // asking audio driver to refresh the devices list
    midiDriver->stop();

    bool showAudioControlPanelButton = controller->getAudioDriver()->hasControlPanel();
    StandalonePreferencesDialog *dialog = new StandalonePreferencesDialog(this,
                                                                          showAudioControlPanelButton,
                                                                          controller->getAudioDriver(),
                                                                          controller->getMidiDriver());

    // setup signals related with recording and metronome preferences
    MainWindow::setupPreferencesDialogSignals(dialog);

    // setup standalone specific signals
    connect(dialog, SIGNAL(ioPreferencesChanged(QList<bool>, int, int, int, int, int)),
            this, SLOT(setGlobalPreferences(QList<bool>, int, int, int, int, int)));

    connect(dialog, SIGNAL(rejected()), this, SLOT(restartAudioAndMidi()));

    connect(dialog, SIGNAL(sampleRateChanged(int)), controller, SLOT(setSampleRate(int)));
    connect(dialog, SIGNAL(bufferSizeChanged(int)), controller, SLOT(setBufferSize(int)));

    connect(controller->getVstPluginFinder(), SIGNAL(scanFinished(bool)), dialog, SLOT(
                populateVstTab()));
    connect(controller->getVstPluginFinder(), SIGNAL(scanStarted()), dialog, SLOT(clearVstList()));

    connect(dialog, SIGNAL(vstScanDirRemoved(const QString &)), controller,
            SLOT(removePluginsScanPath(const QString &)));
    connect(dialog, SIGNAL(vstScanDirAdded(const QString &)), controller,
            SLOT(addPluginsScanPath(const QString &)));

    connect(dialog, SIGNAL(vstPluginAddedInBlackList(const QString &)), controller,
            SLOT(addBlackVstToSettings(const QString &)));
    connect(dialog, SIGNAL(vstPluginRemovedFromBlackList(const QString &)), controller,
            SLOT(removeBlackVstFromSettings(const QString &)));

    connect(dialog, SIGNAL(startingFullPluginsScan()), controller, SLOT(scanAllPlugins()));
    connect(dialog, SIGNAL(startingOnlyNewPluginsScan()), controller, SLOT(scanOnlyNewPlugins()));

    connect(dialog, SIGNAL(openingExternalAudioControlPanel()), controller,
            SLOT(openExternalAudioControlPanel()));

    return dialog;
}

void MainWindowStandalone::restartAudioAndMidi()
{
    // restart audio and midi drivers when user is cancelling the preferences dialog
    Midi::MidiDriver *midiDriver = controller->getMidiDriver();
    Audio::AudioDriver *audioDriver = controller->getAudioDriver();

    Q_ASSERT(midiDriver);
    Q_ASSERT(audioDriver);

    midiDriver->start(controller->getSettings().getMidiInputDevicesStatus());
    audioDriver->start();
}

// ++++++++++++++++++++++

void MainWindowStandalone::initializePluginFinder()
{
    const Persistence::Settings settings = controller->getSettings();


    controller->clearPluginsList();

    controller->initializeVstPluginsList(settings.getVstPluginsPaths());// load the cached plugins. The cache can be empty.

#ifdef Q_OS_MAC
    controller->initializeAudioUnitPluginsList(settings.getAudioUnitsPaths());
#endif

    // checking for new plugins...
    if (controller->vstScanIsNeeded()) {// no vsts in database cache or new plugins detected in scan folders?
        if (settings.getVstScanFolders().isEmpty())
            controller->addDefaultPluginsScanPath();
        controller->scanOnlyNewPlugins();
    }
}

void MainWindowStandalone::handleServerConnectionError(const QString &msg)
{
    MainWindow::handleServerConnectionError(msg);
    controller->quit();
}

void MainWindowStandalone::setGlobalPreferences(const QList<bool> &midiInputsStatus,
                                                int audioDevice, int firstIn, int lastIn,
                                                int firstOut, int lastOut)
{
    qDebug(jtGUI) << "Setting global preferences ...";

    Audio::AudioDriver *audioDriver = controller->getAudioDriver();

    audioDriver->setProperties(firstIn, lastIn, firstOut, lastOut);
    controller->storeIOSettings(firstIn, lastIn, firstOut, lastOut, audioDevice, midiInputsStatus);

    Midi::MidiDriver *midiDriver = controller->getMidiDriver();
    midiDriver->setInputDevicesStatus(midiInputsStatus);

    controller->updateInputTracksRange();

    foreach (LocalTrackGroupViewStandalone *channel,
             getLocalChannels<LocalTrackGroupViewStandalone *>())
        channel->refreshInputSelectionNames();

    midiDriver->start(midiInputsStatus);
    if (!audioDriver->start()) {
        qCritical() << "Error starting audio device";
        QMessageBox::warning(this, tr("Audio error!"),
                             tr("The audio device can't be started! Please check your audio device and try restart Jamtaba!"));
        controller->useNullAudioDriver();
    }
}

// input selection changed by user or by system
void MainWindowStandalone::refreshTrackInputSelection(int inputTrackIndex)
{
    foreach (LocalTrackGroupViewStandalone *channel,
             getLocalChannels<LocalTrackGroupViewStandalone *>())
        channel->refreshInputSelectionName(inputTrackIndex);
}

void MainWindowStandalone::addChannelsGroup(const QString &groupName)
{
    MainWindow::addChannelsGroup(groupName);
    controller->updateInputTracksRange();
}
