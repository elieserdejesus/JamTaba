#include "MainController.h"
#include "recorder/JamRecorder.h"
#include "recorder/ReaperProjectGenerator.h"
#include "recorder/ClipSortLogGenerator.h"
#include "gui/MainWindow.h"
#include "NinjamController.h"
#include "geo/WebIpToLocationResolver.h"
#include "Utils.h"
#include "loginserver/natmap.h"
#include "log/Logging.h"
#include "audio/core/AudioNode.h"
#include "audio/core/LocalInputNode.h"
#include "ThemeLoader.h"

#include <QBuffer>
#include <QByteArray>
#include <QDateTime>

using namespace Persistence;
using namespace Midi;
using namespace Ninjam;
using namespace Controller;

const quint8 MainController::CAMERA_FPS = 10;

// ++++++++++++++++++++++++++++++++++++++++++++++

MainController::MainController(const Settings &settings) :
    audioMixer(44100),
    currentStreamingRoomID(-1000),
    mutex(QMutex::Recursive),
    started(false),
    ipToLocationResolver(nullptr),
    loginService(new Login::LoginService(this)),
    settings(settings),
    mainWindow(nullptr),
    masterGain(1),
    lastInputTrackID(0),
    usersDataCache(Configurator::getInstance()->getCacheDir()),
    lastFrameTimeStamp(0),
    videoEncoder(nullptr),
    videoIntervalToUpload(nullptr)
{

    QDir cacheDir = Configurator::getInstance()->getCacheDir();
    ipToLocationResolver.reset(new Geo::WebIpToLocationResolver(cacheDir));

    connect(ipToLocationResolver.data(), &Geo::IpToLocationResolver::ipResolved, this, &MainController::ipResolved);

    // Register known JamRecorders here:
    jamRecorders.append(new Recorder::JamRecorder(new Recorder::ReaperProjectGenerator()));
    jamRecorders.append(new Recorder::JamRecorder(new Recorder::ClipSortLogGenerator()));

    videoEncoder = new FFMpegMuxer();
    connect(videoEncoder, &FFMpegMuxer::dataEncoded, this, &MainController::uploadEncodedVideoData);
}

void MainController::setChannelReceiveStatus(const QString &userFullName, quint8 channelIndex, bool receiveChannel)
{
    if (isPlayingInNinjamRoom()) {
        ninjamService.setChannelReceiveStatus(userFullName, channelIndex, receiveChannel);
    }
}

void MainController::setVideoProperties(const QSize &resolution)
{
    videoEncoder->setVideoResolution(resolution);
    videoEncoder->setVideoFrameRate(CAMERA_FPS);
}

QSize MainController::getVideoResolution() const
{
    return videoEncoder->getVideoResolution();
}

void MainController::blockUserInChat(const QString &userNameToBlock)
{
    if (isPlayingInNinjamRoom()) {
        ninjamController->blockUserInChat(userNameToBlock);
    }
}

void MainController::unblockUserInChat(const QString &userNameToUnblock)
{
    if (isPlayingInNinjamRoom()) {
        ninjamController->unblockUserInChat(userNameToUnblock);
    }
}

void MainController::setSampleRate(int newSampleRate)
{
    for (Audio::AudioNode *node : tracksNodes.values()) {
        int rmsWindowSize = Audio::SamplesBuffer::computeRmsWindowSize(newSampleRate);
        node->setRmsWindowSize(rmsWindowSize);
    }

    audioMixer.setSampleRate(newSampleRate);

    if (settings.isSaveMultiTrackActivated()) {
        for (Recorder::JamRecorder *jamRecorder : jamRecorders) {
            jamRecorder->setSampleRate(newSampleRate);
        }
    }

    if (isPlayingInNinjamRoom()) {
        ninjamController->setSampleRate(newSampleRate);

        for (Audio::LocalInputNode *inputTrack : inputTracks.values()) {
            inputTrack->getLooper()->stop(); // looper is stopped when sample rate is changed because the recorded material will sound funny :)
        }
    }

    settings.setSampleRate(newSampleRate);
}

void MainController::setEncodingQuality(float newEncodingQuality)
{
    settings.setEncodingQuality(newEncodingQuality);

    if (isPlayingInNinjamRoom())
        ninjamController->recreateEncoders();
}

void MainController::finishUploads()
{
    for (int channelIndex : audioIntervalsToUpload.keys()) {
        ninjamService.sendIntervalPart(audioIntervalsToUpload[channelIndex]->getGUID(), QByteArray(), true);
    }

    if (videoIntervalToUpload)
        ninjamService.sendIntervalPart(videoIntervalToUpload->getGUID(), QByteArray(), true);
}

void MainController::quitFromNinjamServer(const QString &error)
{
    qCWarning(jtCore) << error;

    stopNinjamController();

    if (mainWindow) {
        mainWindow->exitFromRoom(false, error);
    }
}

void MainController::disconnectFromNinjamServer(const Server &server)
{
    Q_UNUSED(server);

    stopNinjamController();

    if (mainWindow)
        mainWindow->exitFromRoom(true);

    if (settings.isSaveMultiTrackActivated()) {
        for (Recorder::JamRecorder *jamRecorder : jamRecorders) {
            jamRecorder->stopRecording();
        }
    }
}

void MainController::setupNinjamControllerSignals()
{
    auto controller = ninjamController.data();

    Q_ASSERT(controller);

    connect(controller, &NinjamController::encodedAudioAvailableToSend, this, &MainController::enqueueDataToUpload);
    connect(controller, &NinjamController::startingNewInterval, this, &MainController::handleNewNinjamInterval);
    connect(controller, &NinjamController::currentBpiChanged, this, &MainController::updateBpi);
    connect(controller, &NinjamController::currentBpmChanged, this, &MainController::updateBpm);
    connect(controller, &NinjamController::startProcessing, this, &MainController::requestCameraFrame);
}

void MainController::connectInNinjamServer(const Ninjam::Server &server)
{
    qCDebug(jtCore) << "connected in ninjam server";

    stopNinjamController();

    auto newNinjamController = createNinjamController();
    ninjamController.reset(newNinjamController);

    setupNinjamControllerSignals();


    if (mainWindow) {
        mainWindow->enterInRoom(Login::RoomInfo(server.getHostName(), server.getPort(), Login::RoomTYPE::NINJAM, server.getMaxUsers(), server.getMaxChannels()));
    }
    else {
        qCritical() << "mainWindow is null!";
    }

    qCDebug(jtCore) << "starting ninjamController...";

    newNinjamController->start(server);

    if (settings.isSaveMultiTrackActivated()) {

        QString userName = getUserName();
        QDir recordBasePath = QDir(settings.getRecordingPath());
        quint16 bpm = server.getBpm();
        quint16 bpi = server.getBpi();
        float sampleRate = getSampleRate();

        for (Recorder::JamRecorder *jamRecorder : getActiveRecorders()) {
            jamRecorder->startRecording(userName, recordBasePath, bpm, bpi, sampleRate);
        }
    }
}

QMap<int, bool> MainController::getXmitChannelsFlags() const
{
    QMap<int, bool> xmitFlags;

    for (Audio::LocalInputGroup *inputGroup : trackGroups) {
        xmitFlags.insert(inputGroup->getIndex(), inputGroup->isTransmiting());
    }

    return xmitFlags;
}

void MainController::handleNewNinjamInterval()
{
    // TODO move the jamRecorder to NinjamController?
    if (settings.isSaveMultiTrackActivated()) {
        for (Recorder::JamRecorder *jamRecorder : jamRecorders) {
            jamRecorder->newInterval();
        }
    }

    videoEncoder->startNewInterval();
}

void MainController::processCapturedFrame(int frameID, const QImage &frame)
{
    Q_UNUSED(frameID);
    if (videoEncoder) {
        videoEncoder->encodeImage(frame); // video encoder will emit a signal when video frame is encoded
    }
    else {
        qDebug() << "video encoder is null";
    }
}

void MainController::requestCameraFrame(int intervalPosition)
{
    if (isPlayingInNinjamRoom() && mainWindow->cameraIsActivated()) {
        bool isFirstPart = intervalPosition == 0;
        if (isFirstPart || canGrabNewFrameFromCamera()) {
            static int frameID = 0;
            processCapturedFrame(frameID++, mainWindow->pickCameraFrame());
            lastFrameTimeStamp = QDateTime::currentMSecsSinceEpoch();
        }
    }
}

void MainController::uploadEncodedVideoData(const QByteArray &encodedVideoData, bool firstPart)
{
    if (ninjamController) {

        const quint8 videoChannelIndex = 0; // always sending video in first channel

        enqueueDataToUpload(encodedVideoData, videoChannelIndex, firstPart);
    }
}

uint MainController::getFramesPerInterval() const
{
    auto intervalTimeInSeconds = ninjamController->getSamplesPerInterval()/getSampleRate();

    return intervalTimeInSeconds * CAMERA_FPS;
}

void MainController::updateBpi(int newBpi)
{
    qCDebug(jtCore) << "MainController::updateBpi " << newBpi;
    if (settings.isSaveMultiTrackActivated()) {
        for (Recorder::JamRecorder *jamRecorder : jamRecorders) {
            jamRecorder->setBpi(newBpi);
        }
    }
}

void MainController::updateBpm(int newBpm)
{
    qCDebug(jtCore) << "MainController::updateBpm " << newBpm;
    if (settings.isSaveMultiTrackActivated()) {
        for (Recorder::JamRecorder *jamRecorder : jamRecorders) {
            jamRecorder->setBpm(newBpm);
        }
    }

    if (isPlayingInNinjamRoom()) {
        for (Audio::LocalInputNode *inputTrack: inputTracks.values()) {
            inputTrack->getLooper()->stop(); // looper is stopped when BPM is changed because the recorded material will be out of sync
        }
    }
    qCDebug(jtCore) << "MainController::updateBpm ...done";
}

void MainController::enqueueAudioDataToUpload(const QByteArray &encodedData, quint8 channelIndex, bool isFirstPart)
{

    Q_ASSERT(encodedData.left(4) ==  "OggS");

    if (isFirstPart) {

        if (audioIntervalsToUpload.contains(channelIndex)) {

            // flush the end of previous interval
            auto audioInterval = audioIntervalsToUpload[channelIndex];
            ninjamService.sendIntervalPart(audioInterval->getGUID(), audioInterval->getData(), true); // is the last part of interval

            delete audioInterval;
        }


        auto newInterval = new UploadIntervalData(); // generate a new GUID
        audioIntervalsToUpload.insert(channelIndex, newInterval);

        ninjamService.sendIntervalBegin(newInterval->getGUID(), channelIndex, true); // starting a new audio interval
    }

    auto interval = audioIntervalsToUpload[channelIndex];
    if (!interval) {
        return;
    }

    interval->appendData(encodedData);

    bool canSend = interval->getTotalBytes() >= 4096;
    if (canSend) {
        ninjamService.sendIntervalPart(interval->getGUID(), interval->getData(), false); // is not the last part of interval
        interval->clear();
    }

    if (settings.isSaveMultiTrackActivated() && isPlayingInNinjamRoom()) {
        for (auto jamRecorder : getActiveRecorders()) {
            jamRecorder->appendLocalUserAudio(encodedData, channelIndex, isFirstPart);
        }
    }

}

void MainController::enqueueVideoDataToUpload(const QByteArray &encodedData, quint8 channelIndex, bool isFirstPart)
{
    if (isFirstPart) {

        Q_ASSERT(encodedData.left(4) ==  "RIFF");

        if (videoIntervalToUpload) {

            // flush the end of previous interval
            ninjamService.sendIntervalPart(videoIntervalToUpload->getGUID(), videoIntervalToUpload->getData(), true); // is the last part of interval

            delete videoIntervalToUpload;
        }

        videoIntervalToUpload = new UploadIntervalData(); // generate a new GUID

        ninjamService.sendIntervalBegin(videoIntervalToUpload->getGUID(), channelIndex, false); // starting a new audio interval
    }

    if (!videoIntervalToUpload) {
        return;
    }

    videoIntervalToUpload->appendData(encodedData);

    bool canSend = videoIntervalToUpload->getTotalBytes() >= 4096;
    if (canSend) {
        ninjamService.sendIntervalPart(videoIntervalToUpload->getGUID(), videoIntervalToUpload->getData(), false); // is not the last part of interval
        videoIntervalToUpload->clear();
    }

    if (settings.isSaveMultiTrackActivated() && isPlayingInNinjamRoom()) {
        for (auto jamRecorder : getActiveRecorders()) {
            jamRecorder->appendLocalUserVideo(encodedData, isFirstPart);
        }
    }
}

void MainController::enqueueDataToUpload(const QByteArray &encodedData, quint8 channelIndex, bool isFirstPart)
{
    /** The audio thread is firing this event. This thread (main/gui thread) write the encoded bytes in socket.
        We can't write in the socket from audio thread.*/

    bool isAudioData = encodedData.left(4) == "OggS"; // all ogg chunks are prefixed with 'OggS' string

    if (isAudioData)
        enqueueAudioDataToUpload(encodedData, channelIndex, isFirstPart);
    else
        enqueueVideoDataToUpload(encodedData, channelIndex, isFirstPart);
}

int MainController::getMaxAudioChannelsForEncoding(uint trackGroupIndex) const
{
    if (trackGroups.contains(trackGroupIndex)) {
        Audio::LocalInputGroup *group = trackGroups[trackGroupIndex];
        if (group)
            return group->getMaxInputChannelsForEncoding();
    }

    return 0;
}

void MainController::setUserName(const QString &newUserName)
{
    settings.storeUserName(newUserName);
}

QString MainController::getUserName() const
{
    return settings.getUserName();
}

QStringList MainController::getBotNames()
{
    return Ninjam::Service::getBotNamesList();
}

Geo::Location MainController::getGeoLocation(const QString &ip)
{
    return ipToLocationResolver->resolve(ip, getTranslationLanguage());
}

void MainController::mixGroupedInputs(int groupIndex, Audio::SamplesBuffer &out)
{
    if (groupIndex >= 0 && groupIndex < trackGroups.size())
        trackGroups[groupIndex]->mixGroupedInputs(out);
}

// this is called when a new ninjam interval is received and the 'record multi track' option is enabled
void MainController::saveEncodedAudio(const QString &userName, quint8 channelIndex, const QByteArray &encodedAudio)
{
    if (settings.isSaveMultiTrackActivated()) { // just in case
        for (Recorder::JamRecorder *jamRecorder : getActiveRecorders()) {
            jamRecorder->addRemoteUserAudio(userName, encodedAudio, channelIndex);
        }
    }
}

void MainController::removeAllInputTracks()
{
    for (int trackID : inputTracks.keys()) {
        removeInputTrackNode(trackID);
    }
}

void MainController::removeInputTrackNode(int inputTrackIndex)
{
    QMutexLocker locker(&mutex);

    if (inputTracks.contains(inputTrackIndex)) {
        // remove from group
        Audio::LocalInputNode *inputTrack = inputTracks[inputTrackIndex];
        int trackGroupIndex = inputTrack->getChanneGrouplIndex();
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
    int inputTrackID = lastInputTrackID++; // input tracks are not created concurrently, no worries about thread safe in this track ID generation, I hope :)
    inputTracks.insert(inputTrackID, inputTrackNode);
    addTrack(inputTrackID, inputTrackNode);

    int trackGroupIndex = inputTrackNode->getChanneGrouplIndex();
    if (!trackGroups.contains(trackGroupIndex))
        trackGroups.insert(trackGroupIndex, new Audio::LocalInputGroup(trackGroupIndex, inputTrackNode));
    else
        trackGroups[trackGroupIndex]->addInputNode(inputTrackNode);

    return inputTrackID;
}

Audio::LocalInputNode *MainController::getInputTrack(int localInputIndex)
{
    if (inputTracks.contains(localInputIndex))
        return inputTracks[localInputIndex];

    qCritical() << "wrong input track index " << localInputIndex;

    return nullptr;
}

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

void MainController::storeMultiTrackRecordingStatus(bool savingMultiTracks)
{
    if (settings.isSaveMultiTrackActivated() && !savingMultiTracks) { // user is disabling recording multi tracks?
        for (Recorder::JamRecorder *jamRecorder : jamRecorders) {
            jamRecorder->stopRecording();
        }
    }

    settings.setSaveMultiTrack(savingMultiTracks);
}

QMap<QString, QString> MainController::getJamRecoders() const
{
    QMap<QString, QString> jamRecoderMap;
    for (Recorder::JamRecorder * jamRecorder : jamRecorders) {
        jamRecoderMap.insert(jamRecorder->getWriterId(), jamRecorder->getWriterName());
    }

    return jamRecoderMap;
}

void MainController::storeJamRecorderStatus(const QString &writerId, bool status)
{
    if (settings.isSaveMultiTrackActivated()) { // recording is active and changing the jamRecorder status
        for (Recorder::JamRecorder *jamRecorder : jamRecorders) {
            if (jamRecorder->getWriterId() == writerId && !status) {
                jamRecorder->stopRecording();
            }
        }
    }

    settings.setJamRecorderActivated(writerId, status);
}

void MainController::storeMultiTrackRecordingPath(const QString &newPath)
{
    settings.setMultiTrackRecordingPath(newPath);
    if (settings.isSaveMultiTrackActivated()) {
        for (Recorder::JamRecorder *jamRecorder : jamRecorders) {
            jamRecorder->setRecordPath(newPath);
        }
    }
}

void MainController::storePrivateServerSettings(const QString &server, int serverPort, const QString &password)
{
    settings.addPrivateServer(server, serverPort, password);
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

void MainController::setCustomMetronome(const QString &primaryBeatFile, const QString &offBeatFile, const QString &accentBeatFile)
{
    settings.setCustomMetronome(primaryBeatFile, offBeatFile, accentBeatFile);
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

void MainController::storeWindowSettings(bool maximized, const QPointF &location, const QSize &size)
{
    settings.setWindowSettings(maximized, location, size);
}

void MainController::storeIOSettings(int firstIn, int lastIn, int firstOut, int lastOut, int audioDevice, const QList<bool> &midiInputsStatus)
{
    settings.setAudioSettings(firstIn, lastIn, firstOut, lastOut, audioDevice);
    settings.setMidiSettings(midiInputsStatus);
}

void MainController::removeTrack(long trackID)
{
    QMutexLocker locker(&mutex);
    /** remove Track is called from ninjam service thread, and cause a crash if the process callback (audio Thread) is iterating over tracksNodes to render audio */

    auto trackNode = tracksNodes[trackID];
    if (trackNode) {
        trackNode->suspendProcessors();
        audioMixer.removeNode(trackNode);
        tracksNodes.remove(trackID);
        delete trackNode;
    }
}

void MainController::setAllLoopersStatus(bool activated)
{
    for (LocalInputNode *inputTrack : inputTracks.values()) {
        inputTrack->getLooper()->setActivated(activated);
    }
}

void MainController::doAudioProcess(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate)
{
    std::vector<Midi::MidiMessage> incommingMidi = pullMidiMessagesFromDevices();
    audioMixer.process(in, out, sampleRate, incommingMidi);

    out.applyGain(masterGain, 1.0f); // using 1 as boost factor/multiplier (no boost)
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
    }
    else {

        if (ninjamController)
            ninjamController->process(in, out, sampleRate);
    }
}

void MainController::syncWithNinjamIntervalStart(uint intervalLenght)
{
    for (Audio::LocalInputNode *inputTrack : inputTracks.values()) {
        inputTrack->startNewLoopCycle(intervalLenght);
    }
}

Audio::AudioPeak MainController::getTrackPeak(int trackID)
{
    QMutexLocker locker(&mutex);

    auto trackNode = tracksNodes[trackID];

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

void MainController::setTransmitingStatus(int channelID, bool transmiting)
{
    if (trackGroups.contains(channelID)) {
        auto trackGroup = trackGroups[channelID];
        if (trackGroup->isTransmiting() != transmiting) {
            trackGroup->setTransmitingStatus(transmiting);
        }
    }
}

bool MainController::isTransmiting(int channelID) const
{
    if (trackGroups.contains(channelID))
        return trackGroups[channelID]->isTransmiting();

    return false;
}

void MainController::setTrackPan(int trackID, float pan, bool blockSignals)
{
    auto node = tracksNodes[trackID];
    if (node) {
        node->blockSignals(blockSignals);
        node->setPan(pan);
        node->blockSignals(false);
    }
}

void MainController::setTrackBoost(int trackID, float boostInDecibels)
{
    auto node = tracksNodes[trackID];
    if (node)
        node->setBoost(Utils::dbToLinear(boostInDecibels));
}

void MainController::resetTrack(int trackID)
{
    auto node = tracksNodes[trackID];
    if (node)
        node->reset();
}

void MainController::setTrackGain(int trackID, float gain, bool blockSignals)
{
    auto node = tracksNodes[trackID];
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
    auto node = tracksNodes[trackID];
    if (node) {
        node->blockSignals(blockSignals);
        node->setMute(muteStatus);
        node->blockSignals(false); // unblock signals by default
    }
}

void MainController::setTrackSolo(int trackID, bool soloStatus, bool blockSignals)
{
    auto node = tracksNodes[trackID];
    if (node) {
        node->blockSignals(blockSignals);
        node->setSolo(soloStatus);
        node->blockSignals(false);
    }
}

bool MainController::trackIsMuted(int trackID) const
{
    auto node = tracksNodes[trackID];

    if (node)
        return node->isMuted();

    return false;
}

bool MainController::trackIsSoloed(int trackID) const
{
    auto node = tracksNodes[trackID];

    if (node)
        return node->isSoloed();

    return false;
}

void MainController::setTrackStereoInversion(int trackID, bool stereoInverted)
{
    auto inputTrackNode = inputTracks[trackID];

    if (inputTrackNode)
        inputTrackNode->setStereoInversion(stereoInverted);
}

bool MainController::trackStereoIsInverted(int trackID) const
{
    auto inputTrackNode = inputTracks[trackID];

    if (inputTrackNode)
        return inputTrackNode->isStereoInverted();

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

    for (auto input : inputTracks) {
        delete input;
    }

    inputTracks.clear();

    for (auto group : trackGroups) {
        delete group;
    }

    trackGroups.clear();

    qCDebug(jtCore()) << "cleaning tracksNodes done!";

    qCDebug(jtCore()) << "cleaning jamRecorders...";

    for(auto jamRecorder : jamRecorders) {
        delete jamRecorder;
    }

    if (videoIntervalToUpload)
        delete videoIntervalToUpload;

    for (auto audioIntervalToUpload : audioIntervalsToUpload)
        if (audioIntervalToUpload)
            delete audioIntervalToUpload;

    audioIntervalsToUpload.clear();

    qCDebug(jtCore()) << "cleaning jamRecorders done!";

    qCDebug(jtCore) << "MainController destructor finished!";
}

void MainController::saveLastUserSettings(const Persistence::LocalInputTrackSettings &inputsSettings)
{
    if (inputsSettings.isValid()) { // avoid save empty settings
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
    return settings.readPresetFromFile(name); // allow multi subchannels by default
}

// +++++++++++++++++++++++++++++++++++++++

void MainController::setFullScreenView(bool fullScreen)
{
    settings.setFullScreenView(fullScreen);
}

void MainController::setAllTracksActivation(bool activated)
{
    for (auto track : tracksNodes) {
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
    return roomStreamer && roomStreamer->isStreaming();
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

    if (userNameWasChoosed()) { // just in case :)
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

        connect(&ninjamService, &Service::connectedInServer, this, &MainController::connectInNinjamServer);

        connect(&ninjamService, &Service::disconnectedFromServer, this, &MainController::disconnectFromNinjamServer);

        connect(&ninjamService, &Service::error, this, &MainController::quitFromNinjamServer);

        qInfo() << "Starting " + getUserEnvironmentString();

        started = true;
    }
}

void MainController::connectInJamtabaServer()
{
    // connect with login server and receive (after some seconds) a list of public rooms to play

    NatMap map; // not used yet, will be used in future to real time rooms
    QString userEnvironment = getUserEnvironmentString();
    QString version = QApplication::applicationVersion();
    QString userName = settings.getUserName();
    if (userName.isEmpty())
        userName = "No name!";

    qCInfo(jtCore) << "Connecting in Jamtaba server...";

    loginService.connectInServer(userName, 0, "", map, version, userEnvironment, getSampleRate());
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

        if (ninjamController)
            ninjamController->stop(false); // block disconnected signal

        started = false;

        qCDebug(jtCore) << "disconnecting from login server...";
        loginService.disconnectFromServer();
    }
}

// +++++++++++

bool MainController::setTheme(const QString &themeName)
{
    QString themeDir(Configurator::getInstance()->getThemesDir().absolutePath());
    QString themeCSS = Theme::Loader::loadCSS(themeDir, themeName);

    if (!themeCSS.isEmpty()) {
        setCSS(themeCSS);
        settings.setTheme(themeName);
        emit themeChanged();
        return true;
    }

    return false;
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

    if (ninjamController && ninjamController->isRunning()) {
        ninjamController->stop(true);
    }

    for (auto uploadInterval : audioIntervalsToUpload) {
        delete uploadInterval;
    }
    audioIntervalsToUpload.clear();
}

void MainController::setTranslationLanguage(const QString &languageCode)
{
    settings.setTranslation(languageCode);
}

QString MainController::getSuggestedUserName()
{
    // try get user name in home path. If is empty try the environment variables.

    QString userName = QDir::home().dirName();
    if (!userName.isEmpty())
        return userName;

    userName = qgetenv("USER"); // for MAc or Linux
    if (userName.isEmpty())
        userName = qgetenv("USERNAME"); // for windows

    if (!userName.isEmpty())
        return userName;

    return ""; // returning empty name as suggestion
}


void MainController::storeMeteringSettings(bool showingMaxPeaks, quint8 meterOption)
{
    settings.storeMeterOption(meterOption);
    settings.storeMeterShowingMaxPeaks(showingMaxPeaks);
}

Audio::LocalInputNode *MainController::getInputTrackInGroup(quint8 groupIndex, quint8 trackIndex) const
{
    auto trackGroup = trackGroups[groupIndex];
    if (!trackGroup)
        return nullptr;

    return trackGroup->getInputNode(trackIndex);
}

bool MainController::canGrabNewFrameFromCamera() const
{
    static const quint64 timeBetweenFrames = 1000/CAMERA_FPS;

    const quint64 now = QDateTime::currentMSecsSinceEpoch();

    return (now - lastFrameTimeStamp) >= timeBetweenFrames;
}

QList<Recorder::JamRecorder *> MainController::getActiveRecorders() const
{
    QList<Recorder::JamRecorder *> activeRecorders;

    for (auto jamRecorder : jamRecorders) {
        if (settings.isJamRecorderActivated(jamRecorder->getWriterId())) {
            activeRecorders.append(jamRecorder);
        }
    }

    return activeRecorders;
}

