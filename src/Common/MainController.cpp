#include "MainController.h"
#include "recorder/ReaperProjectGenerator.h"
#include "gui/MainWindow.h"
#include "NinjamController.h"
#include "geo/WebIpToLocationResolver.h"
#include "Utils.h"
#include "loginserver/natmap.h"
#include "log/Logging.h"
#include "audio/core/AudioNode.h"
#include "audio/core/LocalInputNode.h"

using namespace Persistence;
using namespace Midi;
using namespace Ninjam;
using namespace Controller;

// ++++++++++++++++++++++++++++++++++++++++++++++
MainController::MainController(const Settings &settings) :
    audioMixer(44100),
    currentStreamingRoomID(-1000),
    mutex(QMutex::Recursive),
    started(false),
    ipToLocationResolver(new Geo::WebIpToLocationResolver()),
    loginService(new Login::LoginService(this)),
    settings(settings),
    mainWindow(nullptr),
    jamRecorder(new Recorder::ReaperProjectGenerator()),
    masterGain(1),
    lastInputTrackID(0)
{
    connect(ipToLocationResolver.data(), SIGNAL(ipResolved(const QString &)), this, SIGNAL(ipResolved(const QString &)));
}

void MainController::setSampleRate(int newSampleRate)
{
    audioMixer.setSampleRate(newSampleRate);
    if (settings.isSaveMultiTrackActivated())
        jamRecorder.setSampleRate(newSampleRate);
    if (isPlayingInNinjamRoom())
        ninjamController->setSampleRate(newSampleRate);
    settings.setSampleRate(newSampleRate);
}

void MainController::finishUploads()
{
    foreach (int channelIndex, intervalsToUpload.keys())
        ninjamService.sendAudioIntervalPart(intervalsToUpload[channelIndex]->getGUID(),
                                            QByteArray(), true);
}

void MainController::quitFromNinjamServer(const QString &error)
{
    qCWarning(jtCore) << error;
    stopNinjamController();
    if (mainWindow)
        mainWindow->exitFromRoom(false, error);
}

void MainController::disconnectFromNinjamServer(const Server &server)
{
    Q_UNUSED(server);
    stopNinjamController();
    if (mainWindow)
        mainWindow->exitFromRoom(true);
    if (settings.isSaveMultiTrackActivated())
        jamRecorder.stopRecording();
}

void MainController::setupNinjamControllerSignals(){
    Q_ASSERT(ninjamController.data());
    connect(ninjamController.data(), SIGNAL(encodedAudioAvailableToSend(const QByteArray &, quint8, bool, bool)), this, SLOT(enqueueAudioDataToUpload(const QByteArray &, quint8, bool, bool)));
    connect(ninjamController.data(), SIGNAL(startingNewInterval()), this, SLOT(on_newNinjamInterval()));
    connect(ninjamController.data(), SIGNAL(currentBpiChanged(int)), this, SLOT(updateBpi(int)));
    connect(ninjamController.data(), SIGNAL(currentBpmChanged(int)), this, SLOT(updateBpm(int)));
}

void MainController::connectedNinjamServer(const Ninjam::Server &server)
{
    qCDebug(jtCore) << "connected in ninjam server";
    stopNinjamController();
    Controller::NinjamController *newNinjamController = createNinjamController();// new
    ninjamController.reset(newNinjamController);

    setupNinjamControllerSignals();


    if (mainWindow) {
        mainWindow->enterInRoom(Login::RoomInfo(server.getHostName(), server.getPort(),
                                                Login::RoomTYPE::NINJAM, server.getMaxUsers(),
                                                server.getMaxChannels()));
    } else {
        qCCritical(jtCore) << "mainWindow is null!";
    }
    qCDebug(jtCore) << "starting ninjamController...";

    newNinjamController->start(server);

    if (settings.isSaveMultiTrackActivated())
        jamRecorder.startRecording(getUserName(), QDir(settings.getRecordingPath()),
                                   server.getBpm(), server.getBpi(), getSampleRate());
}

QMap<int, bool> MainController::getXmitChannelsFlags() const
{
    QMap<int, bool> xmitFlags;
    foreach (Audio::LocalInputGroup *inputGroup, trackGroups)
        xmitFlags.insert(inputGroup->getIndex(), inputGroup->isTransmiting());
    return xmitFlags;
}

void MainController::on_newNinjamInterval()
{
    // TODO move the jamRecorder to NinjamController?
    qCDebug(jtCore) << "MainController: on_newNinjamInterval";
    if (settings.isSaveMultiTrackActivated())
        jamRecorder.newInterval();
}

void MainController::updateBpi(int newBpi)
{
    if (settings.isSaveMultiTrackActivated())
        jamRecorder.setBpi(newBpi);
}

void MainController::updateBpm(int newBpm)
{
    if (settings.isSaveMultiTrackActivated())
        jamRecorder.setBpm(newBpm);
}

void MainController::enqueueAudioDataToUpload(const QByteArray &encodedAudio, quint8 channelIndex,
                                              bool isFirstPart, bool isLastPart)
{
    /** The audio thread is firing this event. This thread (main/gui thread) write the encoded bytes in socket.
        We can't write in the socket from audio thread.*/
    if (isFirstPart) {
        if (intervalsToUpload.contains(channelIndex))
            delete intervalsToUpload[channelIndex];
        intervalsToUpload.insert(channelIndex, new UploadIntervalData());
        ninjamService.sendAudioIntervalBegin(intervalsToUpload[channelIndex]->getGUID(), channelIndex);
    }
    if (intervalsToUpload[channelIndex]) {// just in case...
        UploadIntervalData *upload = intervalsToUpload[channelIndex];
        upload->appendData(encodedAudio);
        bool canSend = upload->getTotalBytes() >= 4096 || isLastPart;
        if (canSend) {
            ninjamService.sendAudioIntervalPart(upload->getGUID(),
                                                upload->getStoredBytes(), isLastPart);
            upload->clear();
        }
    }

    if (settings.isSaveMultiTrackActivated() && isPlayingInNinjamRoom())
        jamRecorder.appendLocalUserAudio(encodedAudio, channelIndex, isFirstPart, isLastPart);
}

// ++++++++++++++++++++
int MainController::getMaxChannelsForEncodingInTrackGroup(uint trackGroupIndex) const
{
    if (trackGroups.contains(trackGroupIndex)) {
        Audio::LocalInputGroup *group = trackGroups[trackGroupIndex];
        if (group)
            return group->getMaxInputChannelsForEncoding();
    }
    return 0;
}

// ++++++++++++++++++++
void MainController::setUserName(const QString &newUserName)
{
    if (!newUserName.isEmpty()) {
        settings.setUserName(newUserName);
    } else {
        qCritical() << "empty userNAme";
    }
}

QString MainController::getUserName() const
{
    return settings.getUserName();
}

// ++++++++++++++++++++

QStringList MainController::getBotNames() const
{
    return Ninjam::Service::getBotNamesList();
}

Geo::Location MainController::getGeoLocation(const QString &ip)
{
    return ipToLocationResolver->resolve(ip, getTranslationLanguage());
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainController::mixGroupedInputs(int groupIndex, Audio::SamplesBuffer &out)
{
    if (groupIndex >= 0 && groupIndex < trackGroups.size())
        trackGroups[groupIndex]->mixGroupedInputs(out);
}

// ++++++++++++++++++++++++
// this is called when a new ninjam interval is received and the 'record multi track' option is enabled
void MainController::saveEncodedAudio(const QString &userName, quint8 channelIndex,
                                      const QByteArray &encodedAudio)
{
    if (settings.isSaveMultiTrackActivated())// just in case
        jamRecorder.addRemoteUserAudio(userName, encodedAudio, channelIndex);
}

// +++++++++++++++++++++++++++++++++++++++++++++++

void MainController::removeInputTrackNode(int inputTrackIndex)
{
    QMutexLocker locker(&mutex);
    if (inputTracks.contains(inputTrackIndex)) {
        // remove from group
        Audio::LocalInputNode *inputTrack = inputTracks[inputTrackIndex];
        int trackGroupIndex = inputTrack->getGroupChannelIndex();
        if (trackGroups.contains(trackGroupIndex)) {
            trackGroups[trackGroupIndex]->removeInput(inputTrack);
            if (trackGroups[trackGroupIndex]->isEmpty())
                trackGroups.remove(trackGroupIndex);
        }

        inputTracks.remove(inputTrackIndex);
        removeTrack(inputTrackIndex);
    }
}

int MainController::addInputTrackNode(Audio::LocalInputNode *inputTrackNode)
{
    int inputTrackID = lastInputTrackID++;//input tracks are not created concurrently, no worries about thread safe in this track ID generation, I hope :)
    inputTracks.insert(inputTrackID, inputTrackNode);
    addTrack(inputTrackID, inputTrackNode);

    int trackGroupIndex = inputTrackNode->getGroupChannelIndex();
    if (!trackGroups.contains(trackGroupIndex))
        trackGroups.insert(trackGroupIndex,
                           new Audio::LocalInputGroup(trackGroupIndex, inputTrackNode));
    else
        trackGroups[trackGroupIndex]->addInput(inputTrackNode);

    return inputTrackID;
}

Audio::LocalInputNode *MainController::getInputTrack(int localInputIndex)
{
    if (inputTracks.contains(localInputIndex))
        return inputTracks[localInputIndex];
    qCritical() << "wrong input track index " << localInputIndex;
    return nullptr;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++
Audio::AudioNode *MainController::getTrackNode(long ID)
{
    if (tracksNodes.contains(ID))
        return tracksNodes[ID];
    return nullptr;
}

bool MainController::addTrack(long trackID, Audio::AudioNode *trackNode)
{
    QMutexLocker locker(&mutex);

    tracksNodes.insert(trackID, trackNode);
    audioMixer.addNode(trackNode);
    return true;
}

// +++++++++++++++  SETTINGS +++++++++++
void MainController::storeRecordingMultiTracksStatus(bool savingMultiTracks)
{
    if (settings.isSaveMultiTrackActivated() && !savingMultiTracks)// user is disabling recording multi tracks?
        jamRecorder.stopRecording();
    settings.setSaveMultiTrack(savingMultiTracks);
}

void MainController::storeRecordingPath(const QString &newPath)
{
    settings.setRecordingPath(newPath);
    if (settings.isSaveMultiTrackActivated())
        jamRecorder.setRecordPath(newPath);
}

// ---------------------------------
void MainController::storePrivateServerSettings(const QString &server, int serverPort, const QString &password)
{
    settings.setPrivateServerData(server, serverPort, password);
}

void MainController::storeMetronomeSettings(float metronomeGain, float metronomePan, bool metronomeMuted)
{

    settings.setMetronomeSettings(metronomeGain, metronomePan, metronomeMuted);
}

void MainController::setBuiltInMetronome(const QString &metronomeAlias)
{
    settings.setBuiltInMetronome(metronomeAlias);
    recreateMetronome();
}

void MainController::setCustomMetronome(const QString &primaryBeatFile, const QString &secondaryBeatFile)
{
    settings.setCustomMetronome(primaryBeatFile, secondaryBeatFile);
    recreateMetronome();
}

void MainController::recreateMetronome()
{
    if (isPlayingInNinjamRoom()) {
        ninjamController->recreateMetronome(getSampleRate());
    }
}

void MainController::storeIntervalProgressShape(int shape)
{
    settings.setIntervalProgressShape(shape);
}

void MainController::storeWindowSettings(bool maximized, bool usingFullViewMode, QPointF location)
{
    settings.setWindowSettings(maximized, usingFullViewMode, location);
}

void MainController::storeIOSettings(int firstIn, int lastIn, int firstOut, int lastOut,
                                     int audioDevice, const QList<bool> &midiInputsStatus)
{
    settings.setAudioSettings(firstIn, lastIn, firstOut, lastOut, audioDevice);
    settings.setMidiSettings(midiInputsStatus);
}

// +++++++++++++++++

void MainController::removeTrack(long trackID)
{
    QMutexLocker locker(&mutex);
    /** remove Track is called from ninjam service thread, and cause a crash if the process callback (audio Thread) is iterating over tracksNodes to render audio */

    Audio::AudioNode *trackNode = tracksNodes[trackID];
    if (trackNode) {
        trackNode->suspendProcessors();
        audioMixer.removeNode(trackNode);
        tracksNodes.remove(trackID);
        delete trackNode;
    }
}

void MainController::doAudioProcess(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                                    int sampleRate)
{
    audioMixer.process(in, out, sampleRate, pullMidiBuffer());

    out.applyGain(masterGain, 1.0f);// using 1 as boost factor/multiplier (no boost)
    masterPeak.update(out.computePeak());
}

void MainController::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                             int sampleRate)
{
    QMutexLocker locker(&mutex);
    if (!started)
        return;

    if (!isPlayingInNinjamRoom()) {
        doAudioProcess(in, out, sampleRate);
    } else {
        if (ninjamController)
            ninjamController->process(in, out, sampleRate);
    }
}

Audio::AudioPeak MainController::getTrackPeak(int trackID)
{
    QMutexLocker locker(&mutex);
    Audio::AudioNode *trackNode = tracksNodes[trackID];
    if (trackNode && !trackNode->isMuted())
        return trackNode->getLastPeak();
    if (!trackNode)
        qWarning(jtGUI) << "trackNode not found! ID:" << trackID;
    return Audio::AudioPeak();
}

Audio::AudioPeak MainController::getRoomStreamPeak()
{
    return roomStreamer->getLastPeak();
}

// ++++++++++++++ XMIT +++++++++
void MainController::setTransmitingStatus(int channelID, bool transmiting)
{
    if (trackGroups.contains(channelID)) {
        if (trackGroups[channelID]->isTransmiting() != transmiting)
            trackGroups[channelID]->setTransmitingStatus(transmiting);
    }
}

bool MainController::isTransmiting(int channelID) const
{
    if (trackGroups.contains(channelID))
        return trackGroups[channelID]->isTransmiting();
    return false;
}

// ++++++++++ TRACKS ++++++++++++
void MainController::setTrackPan(int trackID, float pan, bool blockSignals)
{
    Audio::AudioNode *node = tracksNodes[trackID];
    if (node) {
        node->blockSignals(blockSignals);
        node->setPan(pan);
        node->blockSignals(false);
    }
}

void MainController::setTrackBoost(int trackID, float boostInDecibels)
{
    Audio::AudioNode *node = tracksNodes[trackID];
    if (node)
        node->setBoost(Utils::dbToLinear(boostInDecibels));
}

void MainController::resetTrack(int trackID)
{
    Audio::AudioNode *node = tracksNodes[trackID];
    if (node)
        node->reset();
}

void MainController::setTrackGain(int trackID, float gain, bool blockSignals)
{
    Audio::AudioNode *node = tracksNodes[trackID];
    if (node) {
        node->blockSignals(blockSignals);
        node->setGain(Utils::linearGainToPower(gain));
        node->blockSignals(false);
    }
}

void MainController::setMasterGain(float newGain)
{
    this->masterGain = Utils::linearGainToPower(newGain);
}

void MainController::setTrackMute(int trackID, bool muteStatus, bool blockSignals)
{
    Audio::AudioNode *node = tracksNodes[trackID];
    if (node) {
        node->blockSignals(blockSignals);
        node->setMute(muteStatus);
        node->blockSignals(false);// unblock signals by default
    }
}

void MainController::setTrackSolo(int trackID, bool soloStatus, bool blockSignals)
{
    Audio::AudioNode *node = tracksNodes[trackID];
    if (node) {
        node->blockSignals(blockSignals);
        node->setSolo(soloStatus);
        node->blockSignals(false);
    }
}

bool MainController::trackIsMuted(int trackID) const
{
    Audio::AudioNode *node = tracksNodes[trackID];
    if (node)
        return node->isMuted();
    return false;
}

bool MainController::trackIsSoloed(int trackID) const
{
    Audio::AudioNode *node = tracksNodes[trackID];
    if (node)
        return node->isSoloed();
    return false;
}

// +++++++++++++++++++++++++++++++++

MainController::~MainController()
{
    qCDebug(jtCore()) << "MainController destrutor!";
    if (mainWindow)
        mainWindow->detachMainController();

    stop();
    qCDebug(jtCore()) << "main controller stopped!";

    qCDebug(jtCore()) << "cleaning tracksNodes...";
    tracksNodes.clear();
    foreach (Audio::LocalInputNode *input, inputTracks)
        delete input;
    inputTracks.clear();

    foreach (Audio::LocalInputGroup *group, trackGroups)
        delete group;
    trackGroups.clear();
    qCDebug(jtCore()) << "cleaning tracksNodes done!";

    qCDebug(jtCore) << "MainController destructor finished!";
}

void MainController::saveLastUserSettings(const Persistence::LocalInputTrackSettings &inputsSettings)
{
    if (inputsSettings.isValid()){// avoid save empty settings
        settings.storeMasterGain(Utils::poweredGainToLinear(getMasterGain()));
        settings.save(inputsSettings);
    }
}

// -------------------------      PRESETS   ----------------------------

QStringList MainController::getPresetList()
{
    return Configurator::getInstance()->getPresetFilesNames(false);
}

void MainController::savePreset(const LocalInputTrackSettings &inputsSettings, const QString &name)
{
    settings.writePresetToFile(Persistence::Preset(name, inputsSettings));
}

void MainController::deletePreset(const QString &name)
{
    return settings.deletePreset(name);
}

Persistence::Preset MainController::loadPreset(const QString &name)
{
    return settings.readPresetFromFile(name);//allow multi subchannels by default
}

// +++++++++++++++++++++++++++++++++++++++

void MainController::setFullScreenView(bool fullScreen)
{
    settings.setFullScreenView(fullScreen);
}

void MainController::setAllTracksActivation(bool activated)
{
    foreach (Audio::AudioNode *track, tracksNodes) {
        if (activated)
            track->activate();
        else
            track->deactivate();
    }
}

void MainController::playRoomStream(const Login::RoomInfo &roomInfo)
{
    if (roomInfo.hasStream()) {
        roomStreamer->setStreamPath(roomInfo.getStreamUrl());
        currentStreamingRoomID = roomInfo.getID();

        // mute all tracks and unmute the room Streamer
        setAllTracksActivation(false);
        roomStreamer->activate();
    }
}

void MainController::stopRoomStream()
{
    roomStreamer->stopCurrentStream();
    currentStreamingRoomID = -1000;

    setAllTracksActivation(true);
    // roomStreamer->setMuteStatus(true);
}

bool MainController::isPlayingRoomStream() const
{
    return roomStreamer->isStreaming();
}

void MainController::enterInRoom(const Login::RoomInfo &room, const QStringList &channelsNames, const QString &password)
{
    qCDebug(jtCore) << "EnterInRoom slot";
    if (isPlayingRoomStream())
        stopRoomStream();

    if (room.getType() == Login::RoomTYPE::NINJAM)
        tryConnectInNinjamServer(room, channelsNames, password);
}

void MainController::sendNewChannelsNames(const QStringList &channelsNames)
{
    if (isPlayingInNinjamRoom())
        ninjamService.sendNewChannelsListToServer(channelsNames);
}

void MainController::sendRemovedChannelMessage(int removedChannelIndex)
{
    if (isPlayingInNinjamRoom())
        ninjamService.sendRemovedChannelIndex(removedChannelIndex);
}

void MainController::tryConnectInNinjamServer(const Login::RoomInfo &ninjamRoom, const QStringList &channelsNames,
                                              const QString &password)
{
    qCDebug(jtCore) << "connecting...";
    if (userNameWasChoosed()) {// just in case :)
        QString serverIp = ninjamRoom.getName();
        int serverPort = ninjamRoom.getPort();
        QString userName = getUserName();
        QString pass = (password.isNull() || password.isEmpty()) ? "" : password;

        this->ninjamService.startServerConnection(serverIp, serverPort, userName, channelsNames,
                                                  pass);
    } else {
        qCritical() << "user name not choosed yet!";
    }
}

void MainController::start()
{
    if (!started) {
        qCInfo(jtCore) << "Creating roomStreamer ...";
        roomStreamer.reset(new Audio::NinjamRoomStreamerNode()); // new Audio::AudioFileStreamerNode(":/teste.mp3");
        this->audioMixer.addNode(roomStreamer.data());

        QObject::connect(&ninjamService, SIGNAL(connectedInServer(const Ninjam::Server &)), this,
                         SLOT(connectedNinjamServer(const Ninjam::Server &)));
        QObject::connect(&ninjamService, SIGNAL(disconnectedFromServer(const Ninjam::Server &)), this,
                         SLOT(disconnectFromNinjamServer(const Ninjam::Server &)));
        QObject::connect(&ninjamService, SIGNAL(error(QString)), this,
                         SLOT(quitFromNinjamServer(QString)));

        NatMap map;// not used yet,will be used in future to real time rooms

        // connect with login server and receive a list of public rooms to play
        QString userEnvironment = getUserEnvironmentString();
        QString version = QApplication::applicationVersion();// applicationVersion();
        QString userName = settings.getUserName();
        if (userName.isEmpty())
            userName = "No name!";

        qCInfo(jtCore) << "Connecting in Jamtaba server...";
        loginService.connectInServer(userName, 0, "", map, version, userEnvironment,
                                     getSampleRate());

        qInfo() << "Starting " + userEnvironment;
        started = true;
    }
}

QString MainController::getUserEnvironmentString() const
{
    QString systemName = QSysInfo::prettyProductName();
    QString userMachineArch = QSysInfo::currentCpuArchitecture();
    QString jamtabaArch = QSysInfo::buildCpuArchitecture();
    QString version = QApplication::applicationVersion();
    QString flavor = getJamtabaFlavor();
    return "Jamtaba " + version + " " + flavor + " (" + jamtabaArch + ") running on " + systemName
           + " (" + userMachineArch + ")";
}

void MainController::stop()
{
    if (started) {
        qCDebug(jtCore) << "Stopping MainController...";
        {
            if (ninjamController)
                ninjamController->stop(false);// block disconnected signal
            started = false;
        }

        qCDebug(jtCore) << "disconnecting from login server...";
        loginService.disconnectFromServer();
    }
}

// +++++++++++

void MainController::configureStyleSheet(const QString &cssFile)
{
    QFile styleFile(":/style/" + cssFile);
    if (!styleFile.open(QFile::ReadOnly)) {
        qCritical("não carregou estilo!");
    } else {
        // Apply the loaded stylesheet
        setCSS(styleFile.readAll());
    }
}

Login::LoginService *MainController::getLoginService() const
{
    return const_cast<Login::LoginService *>(&loginService);
}

bool MainController::isPlayingInNinjamRoom() const
{
    if (ninjamController)
        return ninjamController->isRunning();
    return false;
}

// ++++++++++++= NINJAM ++++++++++++++++

void MainController::stopNinjamController()
{
    QMutexLocker locker(&mutex);
    if (ninjamController && ninjamController->isRunning())
        ninjamController->stop(true);

    foreach (UploadIntervalData *uploadInterval, intervalsToUpload)
        delete uploadInterval;
    intervalsToUpload.clear();
}

void MainController::setTranslationLanguage(const QString &languageCode)
{
    settings.setTranslation(languageCode);
}

QString MainController::getSuggestedUserName()
{
    //try get user name in home path. If is empty try the environment variables.

    QString userName = QDir::home().dirName();
    if (!userName.isEmpty())
        return userName;

    userName = qgetenv("USER"); // for MAc or Linux
    if (userName.isEmpty())
        userName = qgetenv("USERNAME"); // for windows

    if (!userName.isEmpty())
        return userName;

    return ""; //returning empty name as suggestion
}
