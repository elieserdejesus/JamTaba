#include "MainControllerStandalone.h"

#include "midi/RtMidiDriver.h"
#include "midi/MidiMessage.h"
#include "audio/PortAudioDriver.h"
#include "audio/core/LocalInputNode.h"
#include "vst/VstPlugin.h"
#include "vst/VstHost.h"
#include "vst/VstPluginFinder.h"
#include "audio/core/PluginDescriptor.h"
#include "NinjamController.h"
#include "vst/VstPluginChecker.h"
#include "gui/MainWindowStandalone.h"
#include "ninjam/client/Service.h"

#include <QDialog>
#include <QHostAddress>

#ifdef Q_OS_WIN
    #include "windows.h"
#endif

#ifdef Q_OS_MAC
    #include <AudioUnit/AudioUnit.h>
    #include "AU/AudioUnitPlugin.h"
#endif

#include <QDataStream>
#include <QFile>
#include <QDirIterator>
#include <QSettings>
#include <QtConcurrent/QtConcurrent>
#include "log/Logging.h"
#include "Configurator.h"

using ninjam::client::ServerInfo;

QString MainControllerStandalone::getJamtabaFlavor() const
{
    return "Standalone";
}

void MainControllerStandalone::setInputTrackToMono(int localChannelIndex,
                                                   int inputIndexInAudioDevice)
{
    auto inputTrack = getInputTrack(localChannelIndex);
    if (inputTrack)
    {
        if (!inputIndexIsValid(inputIndexInAudioDevice)) // use the first available channel?
            inputIndexInAudioDevice = 0;

        int availableInputs = audioDriver->getInputsCount();
        if (availableInputs > 0)
            inputTrack->setAudioInputSelection(inputIndexInAudioDevice, 1);    // mono
        else
            inputTrack->setToNoInput();

        if (window)
        {
            window->refreshTrackInputSelection(
                localChannelIndex);
        }

        if (isPlayingInNinjamRoom())
        {
            if (ninjamController) { // just in case
                auto trackGroupIndex = inputTrack->getChanneGroupIndex();
                bool voiceChannelActivated = isVoiceChatActivated(trackGroupIndex);
                ninjamController->scheduleEncoderChangeForChannel(trackGroupIndex, voiceChannelActivated);
            }
        }
    }
}

bool MainControllerStandalone::pluginDescriptorLessThan(const audio::PluginDescriptor &d1,
                                                        const audio::PluginDescriptor &d2)
{
    return d1.getName().localeAwareCompare(d2.getName()) < 0;
}

/**
 * @param category: VST, AU or NATIVE
 * @return A map indexed by plugin Manufacturer name.
 */

QMap<QString, QList<audio::PluginDescriptor> > MainControllerStandalone::getPluginsDescriptors(
    audio::PluginDescriptor::Category category)
{
    QMap<QString, QList<audio::PluginDescriptor> > descriptors;

    for (const auto &descriptor : pluginsDescriptors)
    {
        if (descriptor.getCategory() == category)
        {
            QString manufacturer = descriptor.getManufacturer();
            if (!descriptors.contains(manufacturer))
                descriptors.insert(manufacturer, QList<audio::PluginDescriptor>());

            descriptors[manufacturer].append(descriptor);
        }
    }

    for (const QString &manufacturer : descriptors.keys())
        qSort(descriptors[manufacturer].begin(),
              descriptors[manufacturer].end(), pluginDescriptorLessThan);

    return descriptors;
}

audio::Plugin *MainControllerStandalone::addPlugin(quint32 inputTrackIndex, quint32 pluginSlotIndex,
                                                   const audio::PluginDescriptor &descriptor)
{
    auto plugin = createPluginInstance(descriptor);
    if (plugin)
    {
        plugin->start();
        QMutexLocker locker(&mutex);
        getInputTrack(inputTrackIndex)->addProcessor(plugin, pluginSlotIndex);
    }
    return plugin;
}

void MainControllerStandalone::removePlugin(int inputTrackIndex, audio::Plugin *plugin)
{
    QMutexLocker locker(&mutex);
    QString pluginName = plugin->getName();
    try
    {
        auto trackNode = getInputTrack(inputTrackIndex);
        if (trackNode)
            trackNode->removeProcessor(plugin);
    }
    catch (...)
    {
        qCritical() << "Error removing plugin " << pluginName;
    }
}

void MainControllerStandalone::addPluginsScanPath(const QString &path)
{
    settings.addVstScanPath(path);
}

void MainControllerStandalone::removePluginsScanPath(const QString &path)
{
    settings.removeVstScanPath(path);
}

void MainControllerStandalone::clearPluginsList()
{
    pluginsDescriptors.clear();
}

void MainControllerStandalone::clearPluginsCache()
{
    settings.clearVstCache();

    #ifdef Q_OS_MAC
    settings.clearAudioUnitCache();
    #endif
}

// VST BlackList ...
void MainControllerStandalone::addBlackVstToSettings(const QString &path)
{
    settings.addVstToBlackList(path);
}

void MainControllerStandalone::removeBlackVstFromSettings(const QString &pluginPath)
{
    settings.removeVstFromBlackList(pluginPath);
}

bool MainControllerStandalone::inputIndexIsValid(int inputIndex)
{
    return inputIndex >= 0 && inputIndex <= audioDriver->getInputsCount();
}

void MainControllerStandalone::setInputTrackToMIDI(int localChannelIndex, int midiDevice,
                                                   int midiChannel, qint8 transpose,
                                                   quint8 lowerNote, quint8 higherNote)
{
    auto inputTrack = getInputTrack(localChannelIndex);
    if (inputTrack)
    {
        inputTrack->setMidiInputSelection(midiDevice, midiChannel);
        inputTrack->setTranspose(transpose);
        inputTrack->setMidiHigherNote(higherNote);
        inputTrack->setMidiLowerNote(lowerNote);
        if (window)
            window->refreshTrackInputSelection(localChannelIndex);
        if (isPlayingInNinjamRoom())
        {
            if (ninjamController) {
                auto trackGroupIndex = inputTrack->getChanneGroupIndex();
                bool voiceChannelActivated = isVoiceChatActivated(trackGroupIndex);
                ninjamController->scheduleEncoderChangeForChannel(trackGroupIndex, voiceChannelActivated);
            }
        }
    }
}

void MainControllerStandalone::setInputTrackToNoInput(int localChannelIndex)
{
    auto inputTrack = getInputTrack(localChannelIndex);
    if (inputTrack)
    {
        inputTrack->setToNoInput();
        if (window)
            window->refreshTrackInputSelection(localChannelIndex);
        if (isPlayingInNinjamRoom())      // send the finish interval message
        {
            if (audioIntervalsToUpload.contains(localChannelIndex))
            {
                ninjamService->sendIntervalPart(
                    audioIntervalsToUpload[localChannelIndex].getGUID(), QByteArray(), true);
                if (ninjamController) {
                    auto trackGroupIndex = inputTrack->getChanneGroupIndex();
                    bool voiceChannelActivated = isVoiceChatActivated(trackGroupIndex);
                    ninjamController->scheduleEncoderChangeForChannel(trackGroupIndex, voiceChannelActivated);
                }
            }
        }
    }
}

void MainControllerStandalone::setInputTrackToStereo(int localChannelIndex, int firstInputIndex)
{
    auto inputTrack = getInputTrack(localChannelIndex);
    if (inputTrack)
    {
        if (!inputIndexIsValid(firstInputIndex))
            firstInputIndex = 0;    // use the first channel
        int availableInputChannels = audioDriver->getInputsCount();
        if (availableInputChannels > 0)      // we have input channels?
        {
            if (availableInputChannels >= 2)     // can really use stereo?
                inputTrack->setAudioInputSelection(firstInputIndex, 2);    // stereo
            else
                inputTrack->setAudioInputSelection(firstInputIndex, 1);    // mono
        }
        else
        {
            inputTrack->setToNoInput();
        }

        if (window)
            window->refreshTrackInputSelection(localChannelIndex);
        if (isPlayingInNinjamRoom())
        {
            if (ninjamController) {
                auto trackGroupIndex = inputTrack->getChanneGroupIndex();
                bool voiceChannelActivated = isVoiceChatActivated(trackGroupIndex);
                ninjamController->scheduleEncoderChangeForChannel(trackGroupIndex, voiceChannelActivated);
            }

        }
    }
}

void MainControllerStandalone::updateBpm(int newBpm)
{
    MainController::updateBpm(newBpm);

    for (Host *host : hosts)
        host->setTempo(newBpm);
}

void MainControllerStandalone::connectInNinjamServer(const ServerInfo &server)
{
    MainController::connectInNinjamServer(server);

    for (auto host : hosts)
        host->setTempo(server.getBpm());
}

void MainControllerStandalone::setSampleRate(int newSampleRate)
{
    MainController::setSampleRate(newSampleRate);

    for (auto host : hosts)
        host->setSampleRate(newSampleRate);

    audioDriver->setSampleRate(newSampleRate);

    for (auto inputNode : inputTracks)
        inputNode->setProcessorsSampleRate(newSampleRate);
}

void MainControllerStandalone::setBufferSize(int newBufferSize)
{
    for (auto host : hosts)
        host->setBlockSize(newBufferSize);

    audioDriver->setBufferSize(newBufferSize);
    settings.setBufferSize(newBufferSize);
}

void MainControllerStandalone::on_audioDriverStarted()
{
    for (auto inputTrack : inputTracks)
        inputTrack->resumeProcessors();
}

void MainControllerStandalone::on_audioDriverStopped()
{
    for (auto inputTrack : inputTracks)
        inputTrack->suspendProcessors();    // suspend plugins
}

void MainControllerStandalone::handleNewNinjamInterval()
{
    MainController::handleNewNinjamInterval();

    for (auto host : hosts)
        host->setPlayingFlag(true);
}

void MainControllerStandalone::setupNinjamControllerSignals()
{
    MainController::setupNinjamControllerSignals();

    connect(ninjamController.data(), SIGNAL(startProcessing(int)), this,
            SLOT(on_ninjamStartProcessing(int)));
}

void MainControllerStandalone::on_ninjamStartProcessing(int intervalPosition)
{
    for (auto host : hosts)
        host->setPositionInSamples(intervalPosition); // update the vst host time line in every audio callback.
}

void MainControllerStandalone::addFoundedVstPlugin(const QString &name, const QString &path)
{
    bool containThePlugin = false;
    for (const auto descriptor : pluginsDescriptors)
    {
        if (descriptor.isVST() && descriptor.getPath() == path)
        {
            containThePlugin = true;
            break;
        }
    }

    if (!containThePlugin)
    {
        settings.addVstPlugin(path);
        auto category = audio::PluginDescriptor::VST_Plugin;
        QString manufacturer = "";
        pluginsDescriptors.append(audio::PluginDescriptor(name, category, manufacturer, path));
    }
}

#ifdef Q_OS_MAC
void MainControllerStandalone::addFoundedAudioUnitPlugin(const QString &name, const QString &path)
{
    bool containThePlugin = false;
    for (const auto descriptor : pluginsDescriptors)
    {
        if (descriptor.isAU() && descriptor.getPath() == path)
        {
            containThePlugin = true;
            break;
        }
    }
    if (!containThePlugin)
    {
        settings.addAudioUnitPlugin(path);
        pluginsDescriptors.append(au::createPluginDescriptor(name, path));
    }
}

#endif

void MainControllerStandalone::setMainWindow(MainWindow *mainWindow)
{
    MainController::setMainWindow(mainWindow);

    // store a casted pointer to convenience when calling MainWindowStandalone specific functions
    window = dynamic_cast<MainWindowStandalone *>(mainWindow);
}

midi::MidiDriver *MainControllerStandalone::createMidiDriver()
{
    // return new Midi::PortMidiDriver(settings.getMidiInputDevicesStatus());
    return new midi::RtMidiDriver(settings.getMidiInputDevicesStatus(), settings.getSyncOutputDevicesStatus());
    // return new Midi::NullMidiDriver();
}

controller::NinjamController *MainControllerStandalone::createNinjamController()
{
    return new NinjamController(this);
}

audio::AudioDriver *MainControllerStandalone::createAudioDriver(
    const persistence::Settings &settings)
{
    return new audio::PortAudioDriver(
        this,
        settings.getLastAudioInputDevice(),
        settings.getLastAudioOutputDevice(),
        settings.getFirstGlobalAudioInput(),
        settings.getLastGlobalAudioInput(),
        settings.getFirstGlobalAudioOutput(),
        settings.getLastGlobalAudioOutput(),
        settings.getLastSampleRate(),
        settings.getLastBufferSize()
        );
}

MainControllerStandalone::MainControllerStandalone(persistence::Settings settings,
                                                   QApplication *application) :
    MainController(settings),
    application(application),
    audioDriver(nullptr)
{
    application->setQuitOnLastWindowClosed(true);

    hosts.append(vst::VstHost::getInstance());
#ifdef Q_OS_MAC
    hosts.append(au::AudioUnitHost::getInstance());
#endif

    connect(vst::VstHost::getInstance(), &vst::VstHost::pluginRequestingWindowResize,
            this, &MainControllerStandalone::setVstPluginWindowSize);
}

void MainControllerStandalone::setVstPluginWindowSize(QString pluginName, int newWidht,
                                                      int newHeight)
{
    auto pluginEditorWindow = vst::VstPlugin::getPluginEditorWindow(pluginName);
    if (pluginEditorWindow)
    {
        pluginEditorWindow->setFixedSize(newWidht, newHeight);
        // pluginEditorWindow->updateGeometry();
    }
}

void MainControllerStandalone::start()
{
    // creating audio and midi driver before call start() in base class (MainController::start())

    if (!midiDriver)
    {
        qCInfo(jtCore) << "Creating midi driver...";
        midiDriver.reset(createMidiDriver());
    }

    if (!audioDriver)
    {
        qCInfo(jtCore) << "Creating audio driver...";
        audio::AudioDriver *driver = nullptr;
        try
        {
            driver = createAudioDriver(settings);
        }
        catch (const std::runtime_error &error)
        {
            qCritical() << "Audio initialization fail: " << QString::fromUtf8(
                error.what());
            QMessageBox::warning(window, "Audio Initialization Problem!", error.what());
        }
        if (!driver)
            driver = new audio::NullAudioDriver();

        audioDriver.reset(driver);

        QObject::connect(audioDriver.data(), SIGNAL(sampleRateChanged(int)), this,
                         SLOT(setSampleRate(int)));
        QObject::connect(audioDriver.data(), SIGNAL(stopped()), this,
                         SLOT(on_audioDriverStopped()));
        QObject::connect(audioDriver.data(), SIGNAL(started()), this,
                         SLOT(on_audioDriverStarted()));
    }

    // calling the base class
    MainController::start();

    if (audioDriver)
    {
        if (!audioDriver->canBeStarted())
            useNullAudioDriver();
        audioDriver->start();
    }

    if (midiDriver)
        midiDriver->start(settings.getMidiInputDevicesStatus(), settings.getSyncOutputDevicesStatus());

    qCInfo(jtCore) << "Creating plugin finder...";
    vstPluginFinder.reset(new audio::VSTPluginFinder());

#ifdef Q_OS_MAC

    auPluginFinder.reset(new audio::AudioUnitPluginFinder());

    connect(auPluginFinder.data(), &audio::AudioUnitPluginFinder::pluginScanFinished, this,
            &MainControllerStandalone::addFoundedAudioUnitPlugin);

#endif

    connect(vstPluginFinder.data(), &audio::VSTPluginFinder::pluginScanFinished, this,
            &MainControllerStandalone::addFoundedVstPlugin);

    if (audioDriver)
    {
        for (auto host : hosts)
        {
            host->setSampleRate(audioDriver->getSampleRate());
            host->setBlockSize(audioDriver->getBufferSize());
        }
    }
}

void MainControllerStandalone::cancelPluginFinders()
{
    if (vstPluginFinder)
        vstPluginFinder->cancel();

#ifdef Q_OS_MAC

    if (auPluginFinder)
        auPluginFinder->cancel();

#endif
}

void MainControllerStandalone::setCSS(const QString &css)
{
    application->setStyleSheet(css);
}

MainControllerStandalone::~MainControllerStandalone()
{
    qCDebug(jtCore) << "StandaloneMainController destructor!";
    // pluginsDescriptors.clear();
}

audio::Plugin *MainControllerStandalone::createPluginInstance(
    const audio::PluginDescriptor &descriptor)
{
    if (descriptor.isNative())
    {
        if (descriptor.getName() == "Delay")
            return new audio::JamtabaDelay(audioDriver->getSampleRate());
    }
    else if (descriptor.isVST())
    {
        auto host = vst::VstHost::getInstance();
        auto vstPlugin = new vst::VstPlugin(host, descriptor.getPath());
        if (vstPlugin->load(descriptor.getPath()))
            return vstPlugin;
        else
            delete vstPlugin; // avoid a memory leak
    }

#ifdef Q_OS_MAC

    else if (descriptor.isAU())
    {
        return au::audioUnitPluginfromPath(descriptor.getPath());
    }

#endif

    return nullptr;
}

QStringList MainControllerStandalone::getSteinbergRecommendedPaths()
{
    /*
    On a 64-bit OS

    64-bit plugins path = HKEY_LOCAL_MACHINE\SOFTWARE\VST
    32-bit plugins path = HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\VST
    */

    QStringList vstPaths;

#ifdef Q_OS_WIN

    #ifdef _WIN64 // 64 bits
    QSettings wowSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\VST\\",
                          QSettings::NativeFormat);
    vstPaths.append(wowSettings.value("VSTPluginsPath").toString());
    #else // 32 bits
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\VST\\", QSettings::NativeFormat);
    vstPaths.append(settings.value("VSTPluginsPath").toString());
    #endif
#endif

#ifdef Q_OS_MACX
    vstPaths.append("/Library/Audio/Plug-Ins/VST");
    vstPaths.append("~/Library/Audio/Plug-Ins/VST");
#endif

#ifdef Q_OS_LINUX
    // Steinberg VST 2.4 docs are saying nothing about default paths for VST plugins in Linux. But I see these paths
    // in a Linux plugin documentation...

    vstPaths.append("~/.vst/");
    vstPaths.append("/user/lib/vst");
#endif

    return vstPaths;
}

void MainControllerStandalone::addDefaultPluginsScanPath()
{
    QStringList vstPaths;

    // first try read the path store in registry by Jamtaba installer.
    // If this value is not founded use the Steinberg recommended paths.
    QSettings jamtabaRegistryEntry("HKEY_CURRENT_USER\\SOFTWARE\\Jamtaba", QSettings::NativeFormat);
    QString vst2InstallDir = jamtabaRegistryEntry.value("VST2InstallDir").toString();

    if (!vst2InstallDir.isEmpty())
        vstPaths.append(vst2InstallDir);
    else
        vstPaths.append(getSteinbergRecommendedPaths());

    for (const QString &vstPath : vstPaths)
    {
        if (!vstPath.isEmpty() && QDir(vstPath).exists())
            addPluginsScanPath(vstPath);
    }
}

/**
 * We need scan plugins when plugins cache is empty OR we have new plugins in scan folders
 * This code is executed whem Jamtaba is started.
*/

bool MainControllerStandalone::vstScanIsNeeded() const
{
    bool vstCacheIsEmpty = settings.getVstPluginsPaths().isEmpty();
    if (vstCacheIsEmpty)
        return true;

    // checking for new vst plugins in scan folders
    QStringList foldersToScan = settings.getVstScanFolders();

    QStringList skipList(settings.getBlackListedPlugins());
    skipList.append(settings.getVstPluginsPaths());

    bool newVstFounded = false;
    for (const QString &scanFolder : foldersToScan)
    {
        QDirIterator folderIterator(scanFolder, QDir::AllDirs | QDir::NoDotAndDotDot,
                                    QDirIterator::Subdirectories);
        while (folderIterator.hasNext())
        {
            folderIterator.next(); // point to next file inside current folder
            QString filePath = folderIterator.filePath();
            if (!skipList.contains(filePath) && Vst::PluginChecker::isValidPluginFile(filePath))
            {
                newVstFounded = true;
                break;
            }
        }
        if (newVstFounded)
            break;
    }

    return newVstFounded;
}

#ifdef Q_OS_MAC

void MainControllerStandalone::initializeAudioUnitPluginsList(const QStringList &paths)
{
    for (const QString &path : paths)
        qDebug() << "Inicializando " << path;
}

#endif

void MainControllerStandalone::initializeVstPluginsList(const QStringList &paths)
{
    auto category = audio::PluginDescriptor::VST_Plugin;
    for (const QString &path : paths)
    {
        QFile file(path);
        if (file.exists())
        {
            QString pluginName = audio::PluginDescriptor::getVstPluginNameFromPath(path);
            QString manufacturer = "";
            pluginsDescriptors.append(audio::PluginDescriptor(pluginName, category, manufacturer,
                                                              path));
        }
    }
}

void MainControllerStandalone::scanAllVstPlugins()
{
    saveLastUserSettings(settings.getInputsSettings()); // save the config file before start scanning
    clearPluginsCache();
    scanVstPlugins(false);
}

void MainControllerStandalone::scanOnlyNewVstPlugins()
{
    saveLastUserSettings(settings.getInputsSettings()); // save the config file before start scanning
    scanVstPlugins(true);
}

void MainControllerStandalone::scanVstPlugins(bool scanOnlyNewPlugins)
{
    if (vstPluginFinder)
    {
        if (!scanOnlyNewPlugins)
            pluginsDescriptors.clear();

        // The skipList contains the paths for black listed plugins by default.
        // If the parameter 'scanOnlyNewPlugins' is 'true' the cached plugins are added in the skipList too.
        QStringList skipList(settings.getBlackListedPlugins());
        if (scanOnlyNewPlugins)
            skipList.append(settings.getVstPluginsPaths());

        QStringList foldersToScan = settings.getVstScanFolders();
        vstPluginFinder->scan(foldersToScan, skipList);
    }
}

#ifdef Q_OS_MAC
void MainControllerStandalone::scanAudioUnitPlugins()
{
    if (auPluginFinder)
        auPluginFinder->scan();
}

#endif

void MainControllerStandalone::openExternalAudioControlPanel()
{
    if (audioDriver->hasControlPanel()) // just in case
        audioDriver->openControlPanel((void *)mainWindow->winId());
}

void MainControllerStandalone::stopNinjamController()
{
    MainController::stopNinjamController();

    for (Host *host : hosts)
        host->setPlayingFlag(false);
}

void MainControllerStandalone::quit()
{
    // destroy the extern !
    // if(JTBConfig)delete JTBConfig;
    qDebug() << "Thank you for Jamming with Jamtaba !";
    application->quit();
}

std::vector<midi::MidiMessage> MainControllerStandalone::pullMidiMessagesFromPlugins()
{
    // return midi messages created by vst and AU plugins, not by midi controllers.
    std::vector<midi::MidiMessage> receivedMidiMessages;
    for (auto host : hosts)
    {
        std::vector<midi::MidiMessage> pulledMessages = host->pullReceivedMidiMessages();
        receivedMidiMessages.insert(receivedMidiMessages.end(),
                                    pulledMessages.begin(), pulledMessages.end());
    }

    return receivedMidiMessages;
}

void MainControllerStandalone::startMidiClock() const {
    midiDriver->sendClockStart();
}

void MainControllerStandalone::stopMidiClock() const {
    midiDriver->sendClockStop();
}

void MainControllerStandalone::sendMidiClockPulse() const {
    midiDriver->sendClockPulse();
}

std::vector<midi::MidiMessage> MainControllerStandalone::pullMidiMessagesFromDevices()
{
    if (!midiDriver)
        return std::vector<midi::MidiMessage>();

    return midiDriver->getBuffer();
}

bool MainControllerStandalone::isUsingNullAudioDriver() const
{
    return qobject_cast<audio::NullAudioDriver *>(audioDriver.data()) != nullptr;
}

void MainControllerStandalone::stop()
{
    MainController::stop();

    if (audioDriver)
        this->audioDriver->release();

    if (midiDriver)
        this->midiDriver->release();

    qCDebug(jtCore) << "audio and midi drivers released";
}

void MainControllerStandalone::useNullAudioDriver()
{
    qCWarning(jtCore) << "Audio driver can't be used, using NullAudioDriver!";
    audioDriver.reset(new audio::NullAudioDriver());
}

void MainControllerStandalone::updateInputTracksRange()
{
    for (int trackIndex : inputTracks.keys())
    {
        auto inputTrack = getInputTrack(trackIndex);

        if (!inputTrack)
            continue;

        if (!inputTrack->isNoInput())
        {
            if (inputTrack->isAudio())   // audio track
            {
                auto inputTrackRange = inputTrack->getAudioInputRange();

                /** If global input range is reduced to 2 channels and user previous selected inputs 3+4 the input range need be corrected to avoid a beautiful crash :) */
                int globalInputs = audioDriver->getInputsCount();
                if (inputTrackRange.getFirstChannel() >= globalInputs)
                {
                    if (globalInputs >= inputTrackRange.getChannels())   // we have enough channels?
                    {
                        if (inputTrackRange.isMono())
                            setInputTrackToMono(trackIndex, 0);
                        else
                            setInputTrackToStereo(trackIndex, 0);
                    }
                    else
                    {
                        setInputTrackToNoInput(trackIndex);
                    }
                }
            }
            else     // midi track
            {
                int selectedDevice = inputTrack->getMidiDeviceIndex();
                bool deviceIsValid = selectedDevice >= 0
                                     && selectedDevice < midiDriver->getMaxInputDevices()
                                     && midiDriver->inputDeviceIsGloballyEnabled(selectedDevice);
                if (!deviceIsValid)
                {
                    // try another available midi input device
                    int firstAvailableDevice = midiDriver->getFirstGloballyEnableInputDevice();
                    if (firstAvailableDevice >= 0)
                        setInputTrackToMIDI(trackIndex, firstAvailableDevice, -1); // select all channels
                    else
                        setInputTrackToNoInput(trackIndex);
                }
            }
        }
    }
}

float MainControllerStandalone::getSampleRate() const
{
    if (audioDriver)
        return audioDriver->getSampleRate();

    return 44100;
}
