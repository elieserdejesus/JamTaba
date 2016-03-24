#include "MainControllerStandalone.h"

#include "midi/RtMidiDriver.h"
#include "audio/PortAudioDriver.h"
#include "audio/core/LocalInputNode.h"
#include "vst/VstPlugin.h"
#include "vst/VstHost.h"
#include "vst/PluginFinder.h"
#include "audio/core/PluginDescriptor.h"
#include "NinjamController.h"
#include "vst/VstPluginChecker.h"
#include "gui/MainWindowStandalone.h"

#include <QDialog>
#include <QHostAddress>

#if _WIN32
    #include "windows.h"
#endif

#include <QDataStream>
#include <QFile>
#include <QDirIterator>
#include <QSettings>
#include <QtConcurrent/QtConcurrent>
#include "log/Logging.h"
#include "Configurator.h"

using namespace Controller;

QString MainControllerStandalone::getJamtabaFlavor() const
{
    return "Standalone";
}

// ++++++++++++++++++++++++++++++++++

void MainControllerStandalone::setInputTrackToMono(int localChannelIndex,
                                                   int inputIndexInAudioDevice)
{
    Audio::LocalInputNode *inputTrack = getInputTrack(localChannelIndex);
    if (inputTrack) {
        if (!inputIndexIsValid(inputIndexInAudioDevice))  // use the first available channel?
            inputIndexInAudioDevice = 0;

        int availableInputs = audioDriver->getInputsCount();
        if (availableInputs > 0) {
            inputTrack->setAudioInputSelection(inputIndexInAudioDevice, 1);// mono
        }
        else{
            inputTrack->setToNoInput();
        }

        if (window) {
            window->refreshTrackInputSelection(
                localChannelIndex);
        }
        if (isPlayingInNinjamRoom()) {
            if (ninjamController)// just in case
                ninjamController->scheduleEncoderChangeForChannel(inputTrack->getGroupChannelIndex());

        }
    }
}


bool MainControllerStandalone::pluginDescriptorLessThan(const Audio::PluginDescriptor &d1,
                                              const Audio::PluginDescriptor &d2)
{
    return d1.getName().localeAwareCompare(d2.getName()) < 0;
}


QList<Audio::PluginDescriptor> MainControllerStandalone::getPluginsDescriptors()
{
    qSort(pluginsDescriptors.begin(), pluginsDescriptors.end(), pluginDescriptorLessThan);
    return pluginsDescriptors;
}

Audio::Plugin *MainControllerStandalone::addPlugin(int inputTrackIndex,
                                         const Audio::PluginDescriptor &descriptor)
{
    Audio::Plugin *plugin = createPluginInstance(descriptor);
    if (plugin) {
        plugin->start();
        QMutexLocker locker(&mutex);
        getInputTrack(inputTrackIndex)->addProcessor(plugin);
    }
    return plugin;
}

void MainControllerStandalone::removePlugin(int inputTrackIndex, Audio::Plugin *plugin)
{
    QMutexLocker locker(&mutex);
    QString pluginName = plugin->getName();
    try{
        Audio::AudioNode *trackNode = getInputTrack(inputTrackIndex);
        if (trackNode)
            trackNode->removeProcessor(plugin);
    }
    catch (...) {
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

void MainControllerStandalone::clearPluginsCache()
{
    settings.clearVstCache();
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
    Audio::LocalInputNode *inputTrack = getInputTrack(localChannelIndex);
    if (inputTrack) {
        inputTrack->setMidiInputSelection(midiDevice, midiChannel);
        inputTrack->setTranspose(transpose);
        inputTrack->setMidiHigherNote(higherNote);
        inputTrack->setMidiLowerNote(lowerNote);
        if (window)
            window->refreshTrackInputSelection(localChannelIndex);
        if (isPlayingInNinjamRoom()) {
            if (ninjamController)
                ninjamController->scheduleEncoderChangeForChannel(inputTrack->getGroupChannelIndex());

        }
    }
}

void MainControllerStandalone::setInputTrackToNoInput(int localChannelIndex)
{
    Audio::LocalInputNode *inputTrack = getInputTrack(localChannelIndex);
    if (inputTrack) {
        inputTrack->setToNoInput();
        if (window)
            window->refreshTrackInputSelection(localChannelIndex);
        if (isPlayingInNinjamRoom()) {// send the finish interval message
            if (intervalsToUpload.contains(localChannelIndex)) {
                ninjamService.sendAudioIntervalPart(
                    intervalsToUpload[localChannelIndex]->getGUID(), QByteArray(), true);
                if (ninjamController)
                    ninjamController->scheduleEncoderChangeForChannel(
                        inputTrack->getGroupChannelIndex());
            }
        }
    }
}

void MainControllerStandalone::setInputTrackToStereo(int localChannelIndex, int firstInputIndex)
{
    Audio::LocalInputNode *inputTrack = getInputTrack(localChannelIndex);
    if (inputTrack) {
        if (!inputIndexIsValid(firstInputIndex))
            firstInputIndex = 0;//use the first channel
        int availableInputChannels = audioDriver->getInputsCount();
        if (availableInputChannels > 0) {//we have input channels?
            if (availableInputChannels >= 2) //can really use stereo?
                inputTrack->setAudioInputSelection(firstInputIndex, 2);// stereo
            else
                inputTrack->setAudioInputSelection(firstInputIndex, 1);//mono
        }
        else{
            inputTrack->setToNoInput();
        }

        if (window)
            window->refreshTrackInputSelection(localChannelIndex);
        if (isPlayingInNinjamRoom()) {
            if (ninjamController)
                ninjamController->scheduleEncoderChangeForChannel(inputTrack->getGroupChannelIndex());

        }
    }
}

void MainControllerStandalone::updateBpm(int newBpm)
{
    MainController::updateBpm(newBpm);
    vstHost->setTempo(newBpm);
}

void MainControllerStandalone::connectedNinjamServer(const Ninjam::Server &server)
{
    MainController::connectedNinjamServer(server);
    vstHost->setTempo(server.getBpm());
}

void MainControllerStandalone::setSampleRate(int newSampleRate)
{
    MainController::setSampleRate(newSampleRate);
    vstHost->setSampleRate(newSampleRate);
    audioDriver->setSampleRate(newSampleRate);
    foreach (Audio::LocalInputNode *inputNode, inputTracks)
        inputNode->setProcessorsSampleRate(newSampleRate);
}

void MainControllerStandalone::setBufferSize(int newBufferSize)
{
    vstHost->setBlockSize(newBufferSize);
    audioDriver->setBufferSize(newBufferSize);
    settings.setBufferSize(newBufferSize);
}

void MainControllerStandalone::on_audioDriverStarted()
{
    foreach (Audio::LocalInputNode *inputTrack, inputTracks)
        inputTrack->resumeProcessors();
}

void MainControllerStandalone::on_audioDriverStopped()
{
    foreach (Audio::LocalInputNode *inputTrack, inputTracks)
        inputTrack->suspendProcessors();// suspend plugins
}

void MainControllerStandalone::on_newNinjamInterval()
{
    MainController::on_newNinjamInterval();
    vstHost->setPlayingFlag(true);
}

void MainControllerStandalone::setupNinjamControllerSignals(){
    MainController::setupNinjamControllerSignals();

    connect(ninjamController.data(), SIGNAL(startProcessing(int)), this, SLOT(on_ninjamStartProcessing(int)));
}

void MainControllerStandalone::on_ninjamStartProcessing(int intervalPosition)
{
    vstHost->update(intervalPosition);// update the vst host time line in every audio callback.
}

void MainControllerStandalone::on_VSTPluginFounded(QString name, QString group, QString path)
{
    pluginsDescriptors.append(Audio::PluginDescriptor(name, group, path));
    settings.addVstPlugin(path);
}

// ++++++++++++++++++++++++++++++++++++++++++

Vst::PluginFinder *MainControllerStandalone::createPluginFinder()
{
    return new Vst::PluginFinder();
}

void MainControllerStandalone::setMainWindow(MainWindow *mainWindow)
{
    MainController::setMainWindow(mainWindow);

    // store a casted pointer to convenience when calling MainWindowStandalone specific functions
    window = dynamic_cast<MainWindowStandalone *>(mainWindow);
}

Midi::MidiDriver *MainControllerStandalone::createMidiDriver()
{
    // return new Midi::PortMidiDriver(settings.getMidiInputDevicesStatus());
    return new Midi::RtMidiDriver(settings.getMidiInputDevicesStatus());
    // return new Midi::NullMidiDriver();
}

Controller::NinjamController *MainControllerStandalone::createNinjamController()
{
    return new NinjamController(this);
}

Audio::AudioDriver *MainControllerStandalone::createAudioDriver(
    const Persistence::Settings &settings)
{
    return new Audio::PortAudioDriver(
        this,
        settings.getLastAudioDevice(),
        settings.getFirstGlobalAudioInput(),
        settings.getLastGlobalAudioInput(),
        settings.getFirstGlobalAudioOutput(),
        settings.getLastGlobalAudioOutput(),
        settings.getLastSampleRate(),
        settings.getLastBufferSize()
        );
}

// ++++++++++++++++++++++++++++++++++++++++++
MainControllerStandalone::MainControllerStandalone(Persistence::Settings settings,
                                                   QApplication *application) :
    MainController(settings),
    vstHost(Vst::Host::getInstance()),
    application(application)
{
    application->setQuitOnLastWindowClosed(true);

    QObject::connect(Vst::Host::getInstance(),
                     SIGNAL(pluginRequestingWindowResize(QString, int, int)),
                     this, SLOT(on_vstPluginRequestedWindowResize(QString, int, int)));
}

void MainControllerStandalone::on_vstPluginRequestedWindowResize(QString pluginName, int newWidht,
                                                                 int newHeight)
{
    QDialog *pluginEditorWindow = Vst::VstPlugin::getPluginEditorWindow(pluginName);
    if (pluginEditorWindow) {
        pluginEditorWindow->setFixedSize(newWidht, newHeight);
        // pluginEditorWindow->updateGeometry();
    }
}

void MainControllerStandalone::start()
{
    // creating audio and midi driver before call start() in base class (MainController::start())

    if (!midiDriver) {
        qCInfo(jtCore) << "Creating midi driver...";
        midiDriver.reset(createMidiDriver());
    }
    if (!audioDriver) {
        qCInfo(jtCore) << "Creating audio driver...";
        Audio::AudioDriver *driver = nullptr;
        try{
            driver = createAudioDriver(settings);
        }
        catch (const std::runtime_error &error) {
            qCCritical(jtCore) << "Audio initialization fail: " << QString::fromUtf8(
                error.what());
            QMessageBox::warning(window, "Audio Initialization Problem!", error.what());
        }
        if (!driver)
            driver = new Audio::NullAudioDriver();

        audioDriver.reset(driver);


        QObject::connect(audioDriver.data(), SIGNAL(sampleRateChanged(int)), this, SLOT(setSampleRate(int)));
        QObject::connect(audioDriver.data(), SIGNAL(stopped()), this, SLOT(on_audioDriverStopped()));
        QObject::connect(audioDriver.data(), SIGNAL(started()), this, SLOT(on_audioDriverStarted()));
    }

    //calling the base class
    MainController::start();

    if (audioDriver) {
        if (!audioDriver->canBeStarted())
            useNullAudioDriver();
        audioDriver->start();
    }
    if (midiDriver)
        midiDriver->start(settings.getMidiInputDevicesStatus());


    qCInfo(jtCore) << "Creating plugin finder...";
    pluginFinder.reset(createPluginFinder());

    QObject::connect(pluginFinder.data(), SIGNAL(pluginScanFinished(QString, QString,
                                                                    QString)), this,
                     SLOT(on_VSTPluginFounded(QString, QString, QString)));

    if (audioDriver) {
        vstHost->setSampleRate(audioDriver->getSampleRate());
        vstHost->setBlockSize(audioDriver->getBufferSize());
    }
}

void MainControllerStandalone::cancelPluginFinder()
{
    if (pluginFinder)
        pluginFinder->cancel();
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

Audio::Plugin *MainControllerStandalone::createPluginInstance(
    const Audio::PluginDescriptor &descriptor)
{
    if (descriptor.isNative()) {
        if (descriptor.getName() == "Delay")
            return new Audio::JamtabaDelay(audioDriver->getSampleRate());
    } else if (descriptor.isVST()) {
        Vst::VstPlugin *vstPlugin = new Vst::VstPlugin(this->vstHost);
        if (vstPlugin->load(descriptor.getPath()))
            return vstPlugin;
    }
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

    #ifdef _WIN64// 64 bits
    QSettings wowSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\VST\\",
                          QSettings::NativeFormat);
    vstPaths.append(wowSettings.value("VSTPluginsPath").toString());
    #else// 32 bits
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\VST\\", QSettings::NativeFormat);
    vstPaths.append(settings.value("VSTPluginsPath").toString());
    #endif
#endif

#ifdef Q_OS_MACX
    vstPaths.append("/Library/Audio/Plug-Ins/VST");
    vstPaths.append("~/Library/Audio/Plug-Ins/VST");
#endif

#ifdef Q_OS_LINUX
    //Steinberg VST 2.4 docs are saying nothing about default paths for VST plugins in Linux. But I see these paths
    //in a Linux plugin documentation...
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

    foreach (const QString &vstPath, vstPaths) {
        if (!vstPath.isEmpty() && QDir(vstPath).exists())
            addPluginsScanPath(vstPath);
    }
}

// we need scan plugins when plugins cache is empty OR we have new plugins in scan folders
// This code is executed whem Jamtaba is started.
bool MainControllerStandalone::pluginsScanIsNeeded() const
{
    if (settings.getVstPluginsPaths().isEmpty())// cache is empty?

        return true;

    // checking for new plugins in scan folders
    QStringList foldersToScan = settings.getVstScanFolders();

    QStringList skipList(settings.getBlackListedPlugins());
    skipList.append(settings.getVstPluginsPaths());

    foreach (const QString &scanFolder, foldersToScan) {
        QDirIterator folderIterator(scanFolder, QDirIterator::Subdirectories);
        while (folderIterator.hasNext())
        {
            folderIterator.next();// point to next file inside current folder
            QString filePath = folderIterator.filePath();
            if (!skipList.contains(filePath) && Vst::PluginChecker::isValidPluginFile(filePath))
                return true; // a new vst plugin was founded
        }
    }
    return false;
}

void MainControllerStandalone::initializePluginsList(const QStringList &paths)
{
    pluginsDescriptors.clear();
    foreach (const QString &path, paths) {
        QFile file(path);
        if (file.exists()) {
            QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(path);
            pluginsDescriptors.append(Audio::PluginDescriptor(pluginName, "VST", path));
        }
    }
}

void MainControllerStandalone::scanAllPlugins()
{
    saveLastUserSettings(settings.getInputsSettings());// save the config file before start scanning
    clearPluginsCache();
    scanPlugins(false);
}

void MainControllerStandalone::scanOnlyNewPlugins()
{
    saveLastUserSettings(settings.getInputsSettings());// save the config file before start scanning
    scanPlugins(true);
}

void MainControllerStandalone::scanPlugins(bool scanOnlyNewPlugins)
{
    if (pluginFinder) {
        if (!scanOnlyNewPlugins)
            pluginsDescriptors.clear();

        pluginFinder->setFoldersToScan(settings.getVstScanFolders());

        // The skipList contains the paths for black listed plugins by default.
        // If the parameter 'scanOnlyNewPlugins' is 'true' the cached plugins are added in the skipList too.
        QStringList skipList(settings.getBlackListedPlugins());
        if (scanOnlyNewPlugins)
            skipList.append(settings.getVstPluginsPaths());
        pluginFinder->scan(skipList);
    }
}

void MainControllerStandalone::openExternalAudioControlPanel()
{
    if (audioDriver->hasControlPanel())// just in case
        audioDriver->openControlPanel((void *)mainWindow->winId());
}


void MainControllerStandalone::stopNinjamController()
{
    MainController::stopNinjamController();
    vstHost->setPlayingFlag(false);
}

void MainControllerStandalone::quit()
{
    // destroy the extern !
    // if(JTBConfig)delete JTBConfig;
    qDebug() << "Thank you for Jamming with Jamtaba !";
    application->quit();
}

Midi::MidiMessageBuffer MainControllerStandalone::pullMidiBuffer()
{
    Midi::MidiMessageBuffer midiBuffer(midiDriver ? midiDriver->getBuffer() : Midi::MidiMessageBuffer(0));
// int messages = midiBuffer.getMessagesCount();
// for(int m=0; m < messages; m++){
// Midi::MidiMessage msg = midiBuffer.getMessage(m);
// if(msg.isControl()){
// int inputTrackIndex = 0;//just for test for while, we need get this index from the mapping pair
// char cc = msg.getData1();
// char ccValue = msg.getData2();
// qCDebug(jtMidi) << "Control Change received: " << QString::number(cc) << " -> " << QString::number(ccValue);
// getInputTrack(inputTrackIndex)->setGain(ccValue/127.0);
// }
// }
    return midiBuffer;
}

bool MainControllerStandalone::isUsingNullAudioDriver() const
{
    return qobject_cast<Audio::NullAudioDriver *>(audioDriver.data()) != nullptr;
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
    audioDriver.reset(new Audio::NullAudioDriver());
}

void MainControllerStandalone::updateInputTracksRange()
{
    foreach(int trackIndex, inputTracks.keys()) {
        Audio::LocalInputNode *inputTrack = getInputTrack(trackIndex);
        if(!inputTrack)
            continue;

        if (!inputTrack->isNoInput()) {
            if (inputTrack->isAudio()) {// audio track
                Audio::ChannelRange inputTrackRange = inputTrack->getAudioInputRange();

                /** If global input range is reduced to 2 channels and user previous selected inputs 3+4 the input range need be corrected to avoid a beautiful crash :) */
                int globalInputs = audioDriver->getInputsCount();
                if (inputTrackRange.getFirstChannel() >= globalInputs) {
                    if (globalInputs >= inputTrackRange.getChannels()) {//we have enough channels?
                        if (inputTrackRange.isMono()) {
                            setInputTrackToMono(trackIndex, 0);
                        }
                        else{
                            setInputTrackToStereo(trackIndex, 0);
                        }
                    }
                    else{
                        setInputTrackToNoInput(trackIndex);
                    }
                }

            } else {// midi track
                int selectedDevice = inputTrack->getMidiDeviceIndex();
                bool deviceIsValid = selectedDevice >= 0
                                     && selectedDevice < midiDriver->getMaxInputDevices()
                                     && midiDriver->deviceIsGloballyEnabled(selectedDevice);
                if (!deviceIsValid) {
                    // try another available midi input device
                    int firstAvailableDevice = midiDriver->getFirstGloballyEnableInputDevice();
                    if (firstAvailableDevice >= 0)
                        setInputTrackToMIDI(trackIndex, firstAvailableDevice, -1);// select all channels
                    else
                        setInputTrackToNoInput(trackIndex);
                }
            }
        }
    }
}
