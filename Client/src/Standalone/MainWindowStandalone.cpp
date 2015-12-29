#include "MainWindowStandalone.h"
#include "StandAloneMainController.h"
#include "StandalonePreferencesDialog.h"
#include "StandAloneMainController.h"
#include "StandaloneLocalTrackView.h"
#include "StandaloneLocalTrackGroupView.h"
#include "NinjamRoomWindow.h"
#include "LocalTrackView.h"
#include "log/Logging.h"
#include "audio/core/PluginDescriptor.h"
#include <QTimer>

using namespace Persistence;
using namespace Controller;

MainWindowStandalone::MainWindowStandalone(StandaloneMainController *controller) :
    MainWindow(controller),
    controller(controller)
{
    initializePluginFinder();
}

bool MainWindowStandalone::midiDeviceIsValid(int deviceIndex) const
{
    return deviceIndex < controller->getMidiDriver()->getMaxInputDevices();
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

void MainWindowStandalone::restoreLocalSubchannelPluginsList(StandaloneLocalTrackView *subChannelView,
                                                             Subchannel subChannel)
{
    // create the plugins list
    foreach (Persistence::Plugin plugin, subChannel.getPlugins()) {
        QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(plugin.path);
        Audio::PluginDescriptor descriptor(pluginName, "VST", plugin.path);
        Audio::Plugin *pluginInstance = mainController->addPlugin(subChannelView->getInputIndex(), descriptor);
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
                                                     Subchannel subChannel)
{
    // load channels names, gain, pan, boost, mute
    MainWindow::initializeLocalSubChannel(subChannelView, subChannel);

    // check if the loaded input selections (midi, audio mono, audio stereo) are stil valid and fallback if not
    sanitizeSubchannelInputSelections(subChannelView, subChannel);

    restoreLocalSubchannelPluginsList( dynamic_cast<StandaloneLocalTrackView*>(subChannelView), subChannel);
}

LocalTrackGroupView *MainWindowStandalone::createLocalTrackGroupView(int channelGroupIndex)
{
    return new StandaloneLocalTrackGroupView(channelGroupIndex, this);
}

StandaloneLocalTrackGroupView *MainWindowStandalone::geTrackGroupViewByName(QString trackGroupName)
const
{
    foreach (LocalTrackGroupView *trackGroupView, localGroupChannels) {
        if (trackGroupView->getGroupName() == trackGroupName)
            return dynamic_cast<StandaloneLocalTrackGroupView *>(trackGroupView);
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
        StandaloneLocalTrackGroupView *trackGroupView = geTrackGroupViewByName(channel.name);
        if (!trackGroupView)
            continue;
        Channel newChannel = channel;
        newChannel.subChannels.clear();
        int subChannelID = 0;
        QList<LocalTrackView *> trackViews = trackGroupView->getTracks();
        foreach (Subchannel subchannel, channel.subChannels) {
            Subchannel newSubChannel = subchannel;
            StandaloneLocalTrackView *trackView
                = dynamic_cast<StandaloneLocalTrackView *>(trackViews.at(subChannelID));
            if (trackView)
                newSubChannel.setPlugins(buildPersistentPluginList(trackView->getInsertedPlugins()));


            subChannelID++;
            newChannel.subChannels.append(newSubChannel);
        }
        settings.channels.append(newChannel);
    }

    return settings;
}

NinjamRoomWindow *MainWindowStandalone::createNinjamWindow(Login::RoomInfo roomInfo,
                                                           MainController *mainController)
{
    return new NinjamRoomWindow(this, roomInfo, mainController);
}

// implementing the MainWindow methods
bool MainWindow::canCreateSubchannels() const
{
    return true;
}

bool MainWindow::canUseFullScreen() const
{
    return true;
}

// ++++++++++++++++++++++++++++

void MainWindowStandalone::closeEvent(QCloseEvent *e)
{
    MainWindow::closeEvent(e);
    hide();// hide before stop main controller and disconnect from login server

    foreach (LocalTrackGroupView *trackGroup, localGroupChannels)
        trackGroup->closePluginsWindows();
}

// ++++++++++++++++++++++
void MainWindowStandalone::showPreferencesDialog(int initialTab)
{
    stopCurrentRoomStream();

    Midi::MidiDriver *midiDriver = controller->getMidiDriver();
    Audio::AudioDriver *audioDriver = controller->getAudioDriver();
    if (audioDriver)
        audioDriver->stop();
    if (midiDriver)
        midiDriver->stop();

    StandalonePreferencesDialog dialog(mainController, this, initialTab);

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
    MainWindow::initializePluginFinder();

    StandaloneMainController *controller
        = dynamic_cast<StandaloneMainController *>(mainController);
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
    (dynamic_cast<StandaloneMainController *>(mainController))->quit();
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

    foreach (LocalTrackGroupView *channel, localGroupChannels)
        dynamic_cast<StandaloneLocalTrackGroupView *>(channel)->refreshInputSelectionNames();

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
    foreach (LocalTrackGroupView *channel, localGroupChannels)
        dynamic_cast<StandaloneLocalTrackGroupView *>(channel)->refreshInputSelectionName(
            inputTrackIndex);
}

void MainWindowStandalone::addChannelsGroup(QString groupName)
{
    MainWindow::addChannelsGroup(groupName);
    controller->updateInputTracksRange();
}
