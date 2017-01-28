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
                    ninjamController->scheduleEncoderChangeForChannel(inputTrack->getChanneGrouplIndex());

            }
        }
    }


    bool MainControllerStandalone::pluginDescriptorLessThan(const Audio::PluginDescriptor &d1,
                                                  const Audio::PluginDescriptor &d2)
    {
        return d1.getName().localeAwareCompare(d2.getName()) < 0;
    }


    /**
     * @param category: VST, AU or NATIVE
     * @return A map indexed by plugin Manufacturer name.
     */
    QMap<QString, QList<Audio::PluginDescriptor>> MainControllerStandalone::getPluginsDescriptors(Audio::PluginDescriptor::Category category)
    {
        QMap<QString, QList<Audio::PluginDescriptor>> descriptors;

        for (const Audio::PluginDescriptor &descriptor : pluginsDescriptors) {
            if (descriptor.getCategory() == category) {
                QString manufacturer = descriptor.getManufacturer();
                if (!descriptors.contains(manufacturer))
                    descriptors.insert(manufacturer, QList<Audio::PluginDescriptor>());

                 descriptors[manufacturer].append(descriptor);
            }
        }

        for(const QString &manufacturer : descriptors.keys()) {
            qSort(descriptors[manufacturer].begin(), descriptors[manufacturer].end(), pluginDescriptorLessThan);
        }

        return descriptors;
    }

    Audio::Plugin *MainControllerStandalone::addPlugin(quint32 inputTrackIndex, quint32 pluginSlotIndex,
                                             const Audio::PluginDescriptor &descriptor)
    {
        Audio::Plugin *plugin = createPluginInstance(descriptor);
        if (plugin) {
            plugin->start();
            QMutexLocker locker(&mutex);
            getInputTrack(inputTrackIndex)->addProcessor(plugin, pluginSlotIndex);
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
                    ninjamController->scheduleEncoderChangeForChannel(inputTrack->getChanneGrouplIndex());

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
                            inputTrack->getChanneGrouplIndex());
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
                    ninjamController->scheduleEncoderChangeForChannel(inputTrack->getChanneGrouplIndex());

            }
        }
    }

    void MainControllerStandalone::updateBpm(int newBpm)
    {
        MainController::updateBpm(newBpm);

        for(Host *host : hosts)
            host->setTempo(newBpm);
    }

    void MainControllerStandalone::connectedNinjamServer(const Ninjam::Server &server)
    {
        MainController::connectedNinjamServer(server);

        for(Host *host : hosts)
            host->setTempo(server.getBpm());
    }

    void MainControllerStandalone::setSampleRate(int newSampleRate)
    {
        MainController::setSampleRate(newSampleRate);

        for(Host *host : hosts)
            host->setSampleRate(newSampleRate);

        audioDriver->setSampleRate(newSampleRate);
        foreach (Audio::LocalInputNode *inputNode, inputTracks)
            inputNode->setProcessorsSampleRate(newSampleRate);
    }

    void MainControllerStandalone::setBufferSize(int newBufferSize)
    {

        for(Host *host : hosts)
            host->setBlockSize(newBufferSize);

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

        for(Host *host : hosts)
            host->setPlayingFlag(true);
    }

    void MainControllerStandalone::setupNinjamControllerSignals(){
        MainController::setupNinjamControllerSignals();

        connect(ninjamController.data(), SIGNAL(startProcessing(int)), this, SLOT(on_ninjamStartProcessing(int)));
    }

    void MainControllerStandalone::on_ninjamStartProcessing(int intervalPosition)
    {
        for(Host *host : hosts)
            host->setPositionInSamples(intervalPosition);// update the vst host time line in every audio callback.
    }

    void MainControllerStandalone::addFoundedVstPlugin(const QString &name, const QString &path)
    {
        bool containThePlugin = false;
        foreach (const Audio::PluginDescriptor descriptor, pluginsDescriptors) {
            if (descriptor.isVST() && descriptor.getPath() == path) {
                containThePlugin = true;
                break;
            }
        }
        if (!containThePlugin) {
            settings.addVstPlugin(path);
            Audio::PluginDescriptor::Category category = Audio::PluginDescriptor::VST_Plugin;
            QString manufacturer = "";
            pluginsDescriptors.append(Audio::PluginDescriptor(name, category, manufacturer, path));
        }
    }

#ifdef Q_OS_MAC
    void MainControllerStandalone::addFoundedAudioUnitPlugin(const QString &name, const QString &path)
    {
        bool containThePlugin = false;
        foreach (const Audio::PluginDescriptor descriptor, pluginsDescriptors) {
            if (descriptor.isAU() && descriptor.getPath() == path) {
                containThePlugin = true;
                break;
            }
        }
        if (!containThePlugin) {
            settings.addAudioUnitPlugin(path);
            pluginsDescriptors.append(AU::createPluginDescriptor(name, path));
        }
    }
#endif

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
        application(application)
    {
        application->setQuitOnLastWindowClosed(true);

        hosts.append(Vst::VstHost::getInstance());
    #ifdef Q_OS_MAC
        hosts.append(AU::AudioUnitHost::getInstance());
    #endif

        connect(Vst::VstHost::getInstance(), &Vst::VstHost::pluginRequestingWindowResize,
                this, &MainControllerStandalone::setVstPluginWindowSize);
    }

    void MainControllerStandalone::setVstPluginWindowSize(QString pluginName, int newWidht, int newHeight)
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
                qCritical() << "Audio initialization fail: " << QString::fromUtf8(
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
        vstPluginFinder.reset(new audio::VSTPluginFinder());
    #ifdef Q_OS_MAC
        auPluginFinder.reset(new audio::AudioUnitPluginFinder());

        connect(auPluginFinder.data(), &audio::AudioUnitPluginFinder::pluginScanFinished, this, &MainControllerStandalone::addFoundedAudioUnitPlugin);
    #endif

        connect(vstPluginFinder.data(), &audio::VSTPluginFinder::pluginScanFinished, this,
                                                    &MainControllerStandalone::addFoundedVstPlugin);

        if (audioDriver) {
            for(Host *host : hosts) {
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

    Audio::Plugin *MainControllerStandalone::createPluginInstance(
        const Audio::PluginDescriptor &descriptor)
    {
        if (descriptor.isNative()) {
            if (descriptor.getName() == "Delay")
                return new Audio::JamtabaDelay(audioDriver->getSampleRate());
        }
        else if (descriptor.isVST()) {
            Vst::VstHost *host = Vst::VstHost::getInstance();
            Vst::VstPlugin *vstPlugin = new Vst::VstPlugin(host, descriptor.getPath());
            if (vstPlugin->load(descriptor.getPath()))
                return vstPlugin;
        }
    #ifdef Q_OS_MAC
        else if(descriptor.isAU()) {
            int initialSampleRate = audioDriver->getSampleRate();
            int blockSize = audioDriver->getBufferSize();
            return AU::audioUnitPluginfromPath(descriptor.getPath(), initialSampleRate, blockSize);
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

    /**
     * We need scan plugins when plugins cache is empty OR we have new plugins in scan folders
     * This code is executed whem Jamtaba is started.
    */
    bool MainControllerStandalone::vstScanIsNeeded() const
    {
        bool vstCacheIsEmpty = settings.getVstPluginsPaths().isEmpty();
    #ifdef Q_OS_MAC
        bool audioUnitCacheIsEmpty = settings.getAudioUnitsPaths().isEmpty();
        if (vstCacheIsEmpty || audioUnitCacheIsEmpty)
            return true;
    #else
        if (vstCacheIsEmpty)
            return true;
    #endif

        // checking for new vst plugins in scan folders
        QStringList foldersToScan = settings.getVstScanFolders();

        QStringList skipList(settings.getBlackListedPlugins());
        skipList.append(settings.getVstPluginsPaths());

        bool newVstFounded = false;
        foreach (const QString &scanFolder, foldersToScan) {
            QDirIterator folderIterator(scanFolder, QDir::AllDirs | QDir::NoDotAndDotDot,  QDirIterator::Subdirectories);
            while (folderIterator.hasNext()) {
                folderIterator.next();// point to next file inside current folder
                QString filePath = folderIterator.filePath();
                if (!skipList.contains(filePath) && Vst::PluginChecker::isValidPluginFile(filePath)) {
                     newVstFounded = true;
                     break;
                }
            }
            if (newVstFounded)
                break;
        }



    #ifdef Q_OS_WIN
        return newVstFounded;
    #endif

    #ifdef Q_OS_MAC
        //check for new AU plugins
        //QList<Audio::PluginDescriptor> aus = AU::scanAudioUnitPlugins();
        //QStringList cachedPlugins = settings.getAudioUnitsPaths();
        bool newAudioUnitFounded = false;
        //for (const auto &au : aus) {
        //    if (!cachedPlugins.contains(au.getPath())) {
        //        newAudioUnitFounded = true;
        //        break;
        //    }
        //}
        return newVstFounded || newAudioUnitFounded;
    #endif

        return false;
    }

    #ifdef Q_OS_MAC

    void MainControllerStandalone::initializeAudioUnitPluginsList(const QStringList &paths)
    {
        for(const QString &path : paths) {
            qDebug() << "Inicializando " << path;
        }
    }

    #endif

    void MainControllerStandalone::initializeVstPluginsList(const QStringList &paths)
    {
        Audio::PluginDescriptor::Category category = Audio::PluginDescriptor::VST_Plugin;
        for (const QString &path : paths) {
            QFile file(path);
            if (file.exists()) {
                QString pluginName = Audio::PluginDescriptor::getVstPluginNameFromPath(path);
                QString manufacturer = "";
                pluginsDescriptors.append(Audio::PluginDescriptor(pluginName, category, manufacturer, path));
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
        if (vstPluginFinder) {
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

    #ifdef Q_OS_MAC
        if (auPluginFinder)
            auPluginFinder->scan();
    #endif

    }

    void MainControllerStandalone::openExternalAudioControlPanel()
    {
        if (audioDriver->hasControlPanel())// just in case
            audioDriver->openControlPanel((void *)mainWindow->winId());
    }


    void MainControllerStandalone::stopNinjamController()
    {
        MainController::stopNinjamController();

        for(Host *host : hosts)
            host->setPlayingFlag(false);
    }

    void MainControllerStandalone::quit()
    {
        // destroy the extern !
        // if(JTBConfig)delete JTBConfig;
        qDebug() << "Thank you for Jamming with Jamtaba !";
        application->quit();
    }

    Midi::MidiMessageBuffer MainControllerStandalone::pullMidiMessagesFromPlugins()
    {
        // return midi messages created by vst and AU plugins, not by midi controllers.
        QList<Midi::MidiMessage> receivedMidiMessages;
        for(Host *host : hosts)
            receivedMidiMessages.append(host->pullReceivedMidiMessages());

        Midi::MidiMessageBuffer midiBuffer(receivedMidiMessages.count());
        foreach (const Midi::MidiMessage &vstMidiMessage, receivedMidiMessages) {
            midiBuffer.addMessage(vstMidiMessage);
        }
        return midiBuffer;
    }

    Midi::MidiMessageBuffer MainControllerStandalone::pullMidiMessagesFromDevices()
    {
        if (!midiDriver)
            return Midi::MidiMessageBuffer(0);

        return Midi::MidiMessageBuffer(midiDriver->getBuffer());
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
