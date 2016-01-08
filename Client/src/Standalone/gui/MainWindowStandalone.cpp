#include "MainWindowStandalone.h"
#include "PreferencesDialogStandalone.h"
#include "NinjamRoomWindow.h"
#include "LocalTrackView.h"
#include "log/Logging.h"
#include "audio/core/PluginDescriptor.h"

#include <QTimer>

using namespace Persistence;
using namespace Controller;

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

void MainWindowStandalone::setupShortcuts(){
    ui.actionAudioPreferences->setShortcut(QKeySequence(Qt::Key_F5));
    ui.actionMidiPreferences->setShortcut(QKeySequence(Qt::Key_F6));
    ui.actionVstPreferences->setShortcut(QKeySequence(Qt::Key_F7));
    ui.actionRecording->setShortcut(QKeySequence(Qt::Key_F8));
    ui.actionUsersManual->setShortcut(QKeySequence(Qt::Key_F1));
    ui.actionQuit->setShortcut(QKeySequence(Qt::Key_Escape));
    ui.actionFullscreenMode->setShortcut(QKeySequence(Qt::Key_F11));
}

void MainWindowStandalone::setupSignals()
{
    connect(ui.actionFullscreenMode, SIGNAL(triggered(bool)), this, SLOT(toggleFullScreen()));

    Vst::PluginFinder *pluginFinder = controller->getPluginFinder();
    Q_ASSERT(pluginFinder);
    connect(pluginFinder, SIGNAL(scanStarted()), this, SLOT(showPluginScanDialog()));
    connect(pluginFinder, SIGNAL(scanFinished(bool)), this, SLOT(hidePluginScanDialog(bool)));
    connect(pluginFinder, SIGNAL(badPluginDetected(QString)), this, SLOT(addPluginToBlackList(QString)));
    connect(pluginFinder, SIGNAL(pluginScanFinished(QString, QString, QString)), this, SLOT(addFoundedPlugin(QString, QString, QString)));
    connect(pluginFinder, SIGNAL(pluginScanStarted(QString)), this, SLOT(setCurrentScanningPlugin(QString)));
}

void MainWindowStandalone::initialize()
{
    MainWindow::initialize();

    Persistence::Settings settings = mainController->getSettings();
    if (settings.windowsWasFullScreenViewMode()) {
        setFullScreenStatus(true);
    } else {// not full screen. Is maximized or normal?
        if (mainController->getSettings().windowWasMaximized()) {
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
    controller->cancelPluginFinder();
    pluginScanDialog.reset();// reset to null pointer
}

void MainWindowStandalone::hidePluginScanDialog(bool finishedWithoutError)
{
    Q_UNUSED(finishedWithoutError);
    if (pluginScanDialog)
        pluginScanDialog->close();
    pluginScanDialog.reset();
}

void MainWindowStandalone::addPluginToBlackList(QString pluginPath)
{
    QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(pluginPath);
    QWidget *parent = this;
    if (pluginScanDialog)
        parent = pluginScanDialog.data();
    QString message = pluginName + " can't be loaded and will be black listed!";
    QMessageBox::warning(parent, "Plugin Error!", message);
    controller->addBlackVstToSettings(pluginPath);
}

void MainWindowStandalone::addFoundedPlugin(QString name, QString group, QString path)
{
    Q_UNUSED(path);
    Q_UNUSED(group);
    if (pluginScanDialog)
        pluginScanDialog->addFoundedPlugin(name);
}

void MainWindowStandalone::setCurrentScanningPlugin(QString pluginPath)
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
}

void MainWindowStandalone::toggleFullScreen()
{
    setFullScreenStatus(!fullScreenViewMode);// toggle
}

// sanitize the input selection for each loaded subchannel
void MainWindowStandalone::sanitizeSubchannelInputSelections(LocalTrackView *subChannelView,
                                                             Subchannel subChannel)
{
    int trackID = subChannelView->getInputIndex();
    if (subChannel.isMidi()) {
        if (midiDeviceIsValid(subChannel.midiChannel)) {
            controller->setInputTrackToMIDI(trackID, subChannel.midiDevice, subChannel.midiChannel);
        } else {
            if (controller->getMidiDriver()->hasInputDevices()) {
                // use the first midi device and receiving from all channels
                controller->setInputTrackToMIDI(trackID, 0, -1);
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
    LocalTrackViewStandalone *subChannelView, Subchannel subChannel)
{
    // create the plugins list
    foreach (Persistence::Plugin plugin, subChannel.getPlugins()) {
        QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(plugin.path);
        Audio::PluginDescriptor descriptor(pluginName, "VST", plugin.path);
        Audio::Plugin *pluginInstance = controller->addPlugin(subChannelView->getInputIndex(), descriptor);
        if (pluginInstance) {
            try{
                pluginInstance->restoreFromSerializedData(plugin.data);
            }
            catch (...) {
                qWarning() << "Exception restoring " << pluginInstance->getName();
            }
            subChannelView->addPlugin(pluginInstance, plugin.bypassed);
        }
        QApplication::processEvents();
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


LocalTrackGroupViewStandalone *MainWindowStandalone::geTrackGroupViewByName(QString trackGroupName)
const
{
    foreach (LocalTrackGroupViewStandalone *trackGroupView, getLocalChannels<LocalTrackGroupViewStandalone *>()) {
        if (trackGroupView->getGroupName() == trackGroupName)
            return trackGroupView;
    }
    return nullptr;
}

QList<Persistence::Plugin> buildPersistentPluginList(QList<const Audio::Plugin *> trackPlugins)
{
    QList<Persistence::Plugin> persistentPlugins;
    foreach (const Audio::Plugin *p, trackPlugins) {
        QByteArray serializedData = p->getSerializedData();
        persistentPlugins.append(Persistence::Plugin(p->getPath(), p->isBypassed(),
                                                     serializedData));
    }
    return persistentPlugins;
}

LocalInputTrackSettings MainWindowStandalone::getInputsSettings() const
{
    // the base class is returning just the basic: gain, mute, pan , etc for each channel and subchannel
    LocalInputTrackSettings baseSettings = MainWindow::getInputsSettings();

    // recreate the settings including the plugins
    LocalInputTrackSettings settings;
    foreach (Channel channel, baseSettings.channels) {
        LocalTrackGroupViewStandalone *trackGroupView = geTrackGroupViewByName(channel.name);
        if (!trackGroupView)
            continue;
        Channel newChannel = channel;
        newChannel.subChannels.clear();
        int subChannelID = 0;
        QList<LocalTrackViewStandalone *> trackViews = trackGroupView->getTracks<LocalTrackViewStandalone *>();
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

//+++++++++++++++++++=+
void MainWindowStandalone::showPreferencesDialog(int initialTab)
{
    stopCurrentRoomStream();

    Midi::MidiDriver *midiDriver = controller->getMidiDriver();
    Audio::AudioDriver *audioDriver = controller->getAudioDriver();
    if (audioDriver)
        audioDriver->stop();
    if (midiDriver)
        midiDriver->stop();

    StandalonePreferencesDialog dialog(controller, this);
    dialog.initialize(initialTab);

    connect(&dialog,
            SIGNAL(ioPreferencesChanged(QList<bool>, int, int, int, int, int, int, int)),
            this,
            SLOT(setGlobalPreferences(QList<bool>, int, int, int, int, int, int, int)));

    int result = dialog.exec();
    if (result == QDialog::Rejected) {
        if (midiDriver)
            midiDriver->start();// restart audio and midi drivers if user cancel the preferences menu
        if (audioDriver)
            audioDriver->start();
    }
    /** audio driver parameters are changed in on_IOPropertiesChanged. This slot is always invoked when AudioIODialog is closed.*/
}

// ++++++++++++++++++++++

void MainWindowStandalone::initializePluginFinder()
{
    const Persistence::Settings settings = controller->getSettings();

    controller->initializePluginsList(settings.getVstPluginsPaths());// load the cached plugins. The cache can be empty.

    // checking for new plugins...
    if (controller->pluginsScanIsNeeded()) {// no vsts in database cache or new plugins detected in scan folders?
        if (settings.getVstScanFolders().isEmpty())
            controller->addDefaultPluginsScanPath();
        controller->saveLastUserSettings(getInputsSettings());// save config file before scan
        controller->scanPlugins(true);
    }
}

void MainWindowStandalone::handleServerConnectionError(QString msg)
{
    MainWindow::handleServerConnectionError(msg);
    controller->quit();
}

void MainWindowStandalone::setGlobalPreferences(QList<bool> midiInputsStatus, int audioDevice,
                                                int firstIn, int lastIn, int firstOut, int lastOut,
                                                int sampleRate, int bufferSize)
{
    Audio::AudioDriver *audioDriver = controller->getAudioDriver();

#ifdef Q_OS_WIN
    audioDriver->setProperties(audioDevice, firstIn, lastIn, firstOut, lastOut, sampleRate,
                               bufferSize);
#endif
#ifdef Q_OS_MACX
    audioDriver->setProperties(sampleRate, bufferSize);
#endif
    controller->storeIOSettings(firstIn, lastIn, firstOut, lastOut, audioDevice, sampleRate,
                                bufferSize, midiInputsStatus);

    Midi::MidiDriver *midiDriver = controller->getMidiDriver();
    midiDriver->setInputDevicesStatus(midiInputsStatus);

    controller->updateInputTracksRange();

    foreach (LocalTrackGroupViewStandalone *channel, getLocalChannels<LocalTrackGroupViewStandalone *>())
        channel->refreshInputSelectionNames();

    midiDriver->start();
    try{
        audioDriver->start();
    }
    catch (const std::runtime_error &error) {
        qCritical() << "Error starting audio device: " << QString::fromUtf8(error.what());
        QMessageBox::warning(this, "Audio error!",
                             "The audio device can't be started! Please check your audio device and try restart Jamtaba!");
        controller->useNullAudioDriver();
    }
}

// input selection changed by user or by system
void MainWindowStandalone::refreshTrackInputSelection(int inputTrackIndex)
{
    foreach (LocalTrackGroupViewStandalone *channel, getLocalChannels<LocalTrackGroupViewStandalone *>())
        channel->refreshInputSelectionName(inputTrackIndex);
}

void MainWindowStandalone::addChannelsGroup(const QString &groupName)
{
    MainWindow::addChannelsGroup(groupName);
    controller->updateInputTracksRange();
}
