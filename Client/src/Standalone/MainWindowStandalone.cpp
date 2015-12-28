#include "MainWindowStandalone.h"
#include "StandAloneMainController.h"
#include "StandalonePreferencesDialog.h"
#include "StandAloneMainController.h"
#include "StandaloneLocalTrackView.h"
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
    QTimer::singleShot(50, this, &MainWindowStandalone::restorePluginsList);
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

InputsSettings MainWindowStandalone::getInputsSettings() const
{
    // the base class is handling just the basic: gain, mute, pan , etc for each channel and subchannel
    InputsSettings settings = MainWindow::getInputsSettings();

    // adding the inserted plugins in each Persistence::SubChannel instance
    foreach (Persistence::Channel channel, settings.channels) {
        StandaloneLocalTrackGroupView *trackGroupView = geTrackGroupViewByName(channel.name);
        if (trackGroupView) {
            int subChannelID = 0;
            QList<LocalTrackView *> trackViews = trackGroupView->getTracks();
            foreach (Persistence::Subchannel subChannel, channel.subChannels) {
                StandaloneLocalTrackView *trackView
                    = dynamic_cast<StandaloneLocalTrackView *>(trackViews.at(subChannelID));
                if (trackView)
                    subChannel.setPlugins(buildPersistentPluginList(trackView->getInsertedPlugins()));


                subChannelID++;
            }
        }
    }
    return settings;
}

void MainWindowStandalone::loadSubChannel(Subchannel subChannel, LocalTrackView *subChannelView){
    //TODO perdi esse código. O nome do método não era esse, eu criei isso para carregar plugins
    //somente no standalone
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

void MainWindowStandalone::showEvent(QShowEvent *ent)
{
    Q_UNUSED(ent)
// TODO restorePluginsList(bool fromSettings)
    // wait 50 ms before restore the plugins list to avoid freeze the GUI in hidden state while plugins are loading
    // QTimer::singleShot(50, this, &MainWindowStandalone::restorePluginsList);
}

// ++++++++++++++++++++++
void MainWindowStandalone::initializeSubChannel(Persistence::Subchannel subChannel,
                                                LocalTrackView *subChannelView)
{
    if (subChannel.midiDevice >= 0) {// using midi
        qCInfo(jtGUI) << "\t\tSubchannel using MIDI";
        Midi::MidiDriver *midiDriver = controller->getMidiDriver();
        // check if midiDevice index is valid
        if (subChannel.midiDevice < midiDriver->getMaxInputDevices()) {
            controller->setInputTrackToMIDI(
                subChannelView->getInputIndex(), subChannel.midiDevice,
                subChannel.midiChannel);
        } else {
            if (midiDriver->hasInputDevices()) {
                // use the first midi device and all channels
                controller->setInputTrackToMIDI(
                    subChannelView->getInputIndex(), 0, -1);
            } else {
                controller->setInputTrackToNoInput(subChannelView->getInputIndex());
            }
        }
    } else if (subChannel.channelsCount <= 0) {
        qCInfo(jtGUI) << "\t\tsetting Subchannel to no noinput";
        controller->setInputTrackToNoInput(subChannelView->getInputIndex());
    } else if (subChannel.channelsCount == 1) {
        qCInfo(jtGUI) << "\t\tsetting Subchannel to mono input";
        controller->setInputTrackToMono(
            subChannelView->getInputIndex(), subChannel.firstInput);
    } else {
        qCInfo(jtGUI) << "\t\tsetting Subchannel to stereo input";
        controller->setInputTrackToStereo(
            subChannelView->getInputIndex(), subChannel.firstInput);
    }
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

void MainWindowStandalone::loadPresetToTrack()
{
    MainWindow::loadPresetToTrack();

    Persistence::PresetsSettings preset = mainController->getSettings().getPresetSettings();

    for (int group = 0; group < localGroupChannels.size(); ++group) {
        int tracksCount = localGroupChannels.at(group)->getTracksCount();
        QList<LocalTrackView *> trackViews = localGroupChannels.at(group)->getTracks();
        for (int index = 0; index < tracksCount; index++) {
            StandaloneLocalTrackView *trackView
                = dynamic_cast<StandaloneLocalTrackView *>(trackViews.at(index));
            trackView->reset(); // first we remove plugins
            // create the plugins list

            Persistence::Subchannel subChannel = preset.channels.at(group).subChannels.at(index);

            QList<Persistence::Plugin> plugins = subChannel.getPlugins();

            // load plugins
            int plugCount = plugins.size();
            if (plugCount > 0) {
                QApplication::setOverrideCursor(Qt::WaitCursor);
                QApplication::processEvents();

                for (int i = 0; i < plugCount; i++) {
                    QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(plugins.at(
                                                                                            i).path);
                    Audio::PluginDescriptor descriptor(pluginName, "VST", plugins.at(i).path);
                    Audio::Plugin *pluginInstance = mainController->addPlugin(
                        trackView->getInputIndex(), descriptor);
                    if (pluginInstance) {
                        try{
                            pluginInstance->restoreFromSerializedData(plugins.at(i).data);
                        }
                        catch (...) {
                            qWarning() << "Exception restoring " << pluginInstance->getName();
                        }

                        trackView->addPlugin(pluginInstance, plugins.at(i).bypassed);
                        qCInfo(jtConfigurator) << "Plugin Added :"<<pluginInstance->getName()
                                               <<" in track : "<<trackView->getInputIndex();
                    }
                }
                QApplication::restoreOverrideCursor();
            }

            if (canCreateSubchannels()) {
                // load subchannel
                StandaloneMainController *controller
                    = dynamic_cast<StandaloneMainController *>(mainController);

                // using midi
                if (subChannel.midiDevice >= 0) {
                    qCInfo(jtConfigurator) << "\t\tSubchannel using MIDI";
                    Midi::MidiDriver *midiDriver = controller->getMidiDriver();
                    // check if midiDevice index is valid
                    if (subChannel.midiDevice < midiDriver->getMaxInputDevices()) {
                        qCInfo(jtConfigurator) << "\t\tMidi device index : "<<subChannel.midiDevice;

                        controller->setInputTrackToMIDI(
                            trackView->getInputIndex(), subChannel.midiDevice,
                            subChannel.midiChannel);
                    } else {
                        if (midiDriver->hasInputDevices()) {
                            qCInfo(jtConfigurator) << "\t\tSubchannel using default Midi ";

                            // use the first midi device and all channels
                            controller->setInputTrackToMIDI(
                                trackView->getInputIndex(), 0, -1);
                        } else {
                            qCInfo(jtConfigurator) << "\t\tSubchannel using "
                                                   <<subChannel.midiDevice;

                            controller->setInputTrackToNoInput(trackView->getInputIndex());
                        }
                    }
                } else if (subChannel.channelsCount <= 0) {
                    qCInfo(jtConfigurator) << "\t\tsetting Subchannel to no noinput";
                    controller->setInputTrackToNoInput(trackView->getInputIndex());
                } else if (subChannel.channelsCount == 1) {
                    qCInfo(jtConfigurator) << "\t\tsetting Subchannel to mono input";
                    controller->setInputTrackToMono(trackView->getInputIndex(),
                                                    subChannel.firstInput);
                } else {
                    qCInfo(jtConfigurator) << "\t\tsetting Subchannel to stereo input";
                    controller->setInputTrackToStereo(trackView->getInputIndex(),
                                                      subChannel.firstInput);
                }
            }
        }
    }
}

void MainWindowStandalone::restorePluginsList()
{
    qCInfo(jtGUI) << "Restoring plugins list...";

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    Persistence::InputsSettings inputsSettings = mainController->getSettings().getInputsSettings();
    int channelIndex = 0;
    foreach (Persistence::Channel channel, inputsSettings.channels) {
        LocalTrackGroupView *channelView = localGroupChannels.at(channelIndex);
        if (channelView) {
            int subChannelIndex = 0;
            QList<LocalTrackView *> tracks = channelView->getTracks();
            foreach (Persistence::Subchannel subChannel, channel.subChannels) {
                StandaloneLocalTrackView *subChannelView
                    = dynamic_cast<StandaloneLocalTrackView *>(tracks.at(subChannelIndex));
                if (subChannelView) {
                    // create the plugins list
                    foreach (Persistence::Plugin plugin, subChannel.getPlugins()) {
                        QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(
                            plugin.path);
                        Audio::PluginDescriptor descriptor(pluginName, "VST", plugin.path);
                        Audio::Plugin *pluginInstance = mainController->addPlugin(
                            subChannelView->getInputIndex(), descriptor);
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
                subChannelIndex++;
            }
        }
        channelIndex++;
    }
    qCInfo(jtGUI) << "Restoring plugins list done!";

    QApplication::restoreOverrideCursor();
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
        dynamic_cast<StandaloneLocalTrackGroupView*>(channel)->refreshInputSelectionName(inputTrackIndex);
}


void MainWindowStandalone::addChannelsGroup(QString groupName){
    MainWindow::addChannelsGroup(groupName);
    controller->updateInputTracksRange();
}
