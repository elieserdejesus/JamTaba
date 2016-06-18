#include "NinjamController.h"
#include "MainController.h"
#include "audio/core/AudioDriver.h"
#include "audio/core/SamplesBuffer.h"
#include "ninjam/Service.h"
#include "ninjam/User.h"
#include "ninjam/UserChannel.h"
#include "ninjam/Server.h"
#include "audio/core/AudioNode.h"
#include "audio/core/SamplesBuffer.h"
#include "gui/NinjamRoomWindow.h"
#include "audio/NinjamTrackNode.h"
#include "persistence/Settings.h"
#include "audio/MetronomeTrackNode.h"
#include "audio/file/FileReaderFactory.h"
#include "audio/file/FileReader.h"
#include "MetronomeUtils.h"
#include "audio/Resampler.h"

#include <cmath>
#include <cassert>
#include <QMutexLocker>
#include <QDebug>
#include <QThread>
#include <QFileInfo>

#include "audio/SamplesBufferRecorder.h"
#include "Utils.h"
#include <QWaitCondition>
#include "log/Logging.h"

using namespace Controller;

//+++++++++++++  ENCODING THREAD  +++++++++++++
class NinjamController::EncodingThread : public QThread{//TODO: use better thread approach, avoid inheritance form QThread
public:
    EncodingThread(NinjamController* controller)
        :stopRequested(false), controller(controller){
        qCDebug(jtNinjamCore) << "Starting Encoding Thread";
        start();
    }

    ~EncodingThread(){
        stop();
    }

    void addSamplesToEncode(const Audio::SamplesBuffer& samplesToEncode, quint8 channelIndex, bool isFirstPart, bool isLastPart){
        //qCDebug(jtNinjamCore) << "Adding samples to encode";
        QMutexLocker locker(&mutex);
        chunksToEncode.append(new EncodingChunk(samplesToEncode, channelIndex, isFirstPart, isLastPart));
        //this method is called by Qt main thread (the producer thread).
        hasAvailableChunksToEncode.wakeAll();//wakeup the encoding thread (consumer thread)

    }

    void stop(){
        if(!stopRequested){
            //QMutexLocker locker(&mutex);
            stopRequested = true;
            hasAvailableChunksToEncode.wakeAll();
            qCDebug(jtNinjamCore) << "Stopping Encoding Thread";
        }
    }

protected:
    void run(){
        while(!stopRequested){
            mutex.lock();
            if(chunksToEncode.isEmpty()){
                hasAvailableChunksToEncode.wait(&mutex);
            }
            if(stopRequested){
                mutex.unlock();
                break;
            }
            EncodingChunk* chunk = chunksToEncode.first();
            chunksToEncode.removeFirst();
            mutex.unlock();
            if (chunk){
                QByteArray encodedBytes( controller->encode(chunk->buffer, chunk->channelIndex));
                if (chunk->lastPart){
                    encodedBytes.append( controller->encodeLastPartOfInterval(chunk->channelIndex));
                }

                if(!encodedBytes.isEmpty()){
                    emit controller->encodedAudioAvailableToSend(encodedBytes, chunk->channelIndex, chunk->firstPart, chunk->lastPart);
                }
                delete chunk;
            }

        }
        qCDebug(jtNinjamCore) << "Encoding thread stopped!";
    }

private:
    class EncodingChunk{
    public:
        EncodingChunk(const Audio::SamplesBuffer& buffer, quint8 channelIndex, bool firstPart, bool lastPart)
            :buffer(buffer), channelIndex(channelIndex), firstPart(firstPart), lastPart(lastPart ) {

        }

        Audio::SamplesBuffer buffer;
        quint8 channelIndex;
        bool firstPart;
        bool lastPart;
    };

    QList<EncodingChunk*> chunksToEncode;
    QMutex mutex;
    volatile bool stopRequested;
    NinjamController* controller;
    QWaitCondition hasAvailableChunksToEncode;

};

//+++++++++++++++++ Nested classes to handle schedulable events ++++++++++++++++

class NinjamController::SchedulableEvent{//an event scheduled to be processed in next interval
public:
    SchedulableEvent(NinjamController* controller)
        :controller(controller){}
    virtual void process() = 0;
    virtual ~SchedulableEvent(){}
protected:
    NinjamController* controller;
};
//++++++++++++++++++++++++++++++++++++++++++++
class NinjamController::BpiChangeEvent : public SchedulableEvent{
public:
    BpiChangeEvent(NinjamController* controller, quint16 newBpi)
        :SchedulableEvent(controller), newBpi(newBpi){

    }
    void process(){
        controller->currentBpi = newBpi;
        controller->samplesInInterval = controller->computeTotalSamplesInInterval();
        emit controller->currentBpiChanged(controller->currentBpi);
    }
private:
    quint16 newBpi;

};
//+++++++++++++++++++++++++++++++++++++++++
class NinjamController::BpmChangeEvent : public SchedulableEvent{
public:
    BpmChangeEvent(NinjamController* controller, quint16 newBpm)
        :SchedulableEvent(controller), newBpm(newBpm){}
    void process(){
        controller->setBpm(newBpm);
    }
private:
    quint16 newBpm;

};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class NinjamController::InputChannelChangedEvent : public SchedulableEvent{
    public:
        InputChannelChangedEvent(NinjamController* controller, int channelIndex)
            :SchedulableEvent(controller), channelIndex(channelIndex){}
        void process(){
            controller->recreateEncoderForChannel(channelIndex);
        }
    private:
        int channelIndex;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QList<QString> NinjamController::chatBlockedUsers; // initializing the static member

NinjamController::NinjamController(Controller::MainController* mainController)
    :mainController(mainController),
    metronomeTrackNode(createMetronomeTrackNode( mainController->getSampleRate())),
    intervalPosition(0),
    lastBeat(0),
    samplesInInterval(0),
    currentBpi(0),
    currentBpm(0),
    mutex(QMutex::Recursive),
    encodersMutex(QMutex::Recursive),
    encodingThread(nullptr),
    preparedForTransmit(false),
    waitingIntervals(0)//waiting for start transmit
{
    running = false;

}


Ninjam::User NinjamController::getUserByName(const QString &userName) const
{
    Ninjam::Server *server = mainController->getNinjamService()->getCurrentServer();
    QList<Ninjam::User> users = server->getUsers();
    foreach (const Ninjam::User &user, users) {
        if (user.getName() == userName)
            return user;
    }
    return Ninjam::User();
}

//++++++++++++++++++++++++++
void NinjamController::setBpm(int newBpm){
    currentBpm = newBpm;
    samplesInInterval = computeTotalSamplesInInterval();
    metronomeTrackNode->setSamplesPerBeat(getSamplesPerBeat());

    emit currentBpmChanged(currentBpm);
}

void NinjamController::removeEncoder(int groupChannelIndex){
    QMutexLocker locker(&mutex);
    if(encoders.contains(groupChannelIndex)){
        encoders.remove(groupChannelIndex);
    }
}

//+++++++++++++++++++++++++ THE MAIN LOGIC IS HERE  ++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate){

    QMutexLocker locker(&mutex);
    if(!running || samplesInInterval <= 0){
        return;//not initialized
    }

    int totalSamplesToProcess = out.getFrameLenght();
    int samplesProcessed = 0;

    int offset = 0;

    do{
        emit startProcessing(intervalPosition);//vst host time line is updated with this event

        int samplesToProcessInThisStep = (std::min)((int)(samplesInInterval - intervalPosition), totalSamplesToProcess - offset);

        assert(samplesToProcessInThisStep);

        static Audio::SamplesBuffer tempOutBuffer(out.getChannels(), samplesToProcessInThisStep);
        tempOutBuffer.setFrameLenght(samplesToProcessInThisStep);
        tempOutBuffer.zero();

        Audio::SamplesBuffer tempInBuffer(in.getChannels(), samplesToProcessInThisStep);
        tempInBuffer.set(in, offset, samplesToProcessInThisStep, 0);

        bool newInterval = intervalPosition == 0;
        if(newInterval){//starting new interval
            handleNewInterval();
        }

        metronomeTrackNode->setIntervalPosition(this->intervalPosition);
        int currentBeat = intervalPosition / getSamplesPerBeat();
        if(currentBeat != lastBeat){
            lastBeat = currentBeat;
            emit intervalBeatChanged(currentBeat);
        }

        //+++++++++++ MAIN AUDIO OUTPUT PROCESS +++++++++++++++
        bool isLastPart = intervalPosition + samplesToProcessInThisStep >= samplesInInterval;
        foreach (NinjamTrackNode* track, trackNodes) {
            track->setProcessingLastPartOfInterval(isLastPart);//TODO resampler still need a flag indicating the last part?
        }
        mainController->doAudioProcess(tempInBuffer, tempOutBuffer, sampleRate);
        out.add(tempOutBuffer, offset); //generate audio output
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++

        if(preparedForTransmit){
            //1) mix input subchannels, 2) encode and 3) send the encoded audio
            bool isFirstPart = intervalPosition == 0;
            int groupedChannels = mainController->getInputTrackGroupsCount();
            for (int groupIndex = 0; groupIndex < groupedChannels; ++groupIndex) {
                if(mainController->isTransmiting(groupIndex)){
                    int channels = mainController->getMaxChannelsForEncodingInTrackGroup(groupIndex);
                    if(channels > 0){
                        Audio::SamplesBuffer inputMixBuffer(channels, samplesToProcessInThisStep);

                        if(encoders.contains(groupIndex)){
                            inputMixBuffer.zero();
                            mainController->mixGroupedInputs(groupIndex, inputMixBuffer);

                            //encoding is running in another thread to avoid slow down the audio thread
                            encodingThread->addSamplesToEncode( inputMixBuffer, groupIndex, isFirstPart, isLastPart);
                        }
                    }
                }
            }
        }

        //++++++++++++++++++++++++++++++++++++++++
        samplesProcessed += samplesToProcessInThisStep;
        offset += samplesToProcessInThisStep;
        this->intervalPosition = (this->intervalPosition + samplesToProcessInThisStep) % samplesInInterval;
    }
    while( samplesProcessed < totalSamplesToProcess);
}

//++++++++++++++
Audio::MetronomeTrackNode* NinjamController::createMetronomeTrackNode(int sampleRate){
    Audio::SamplesBuffer firstBeatBuffer(2);
    Audio::SamplesBuffer secondaryBeatBuffer(2);
    if (!(mainController->isUsingCustomMetronomeSounds())){
        QString builtInMetronomeAlias = mainController->getSettings().getBuiltInMetronome();
        Audio::MetronomeUtils::createBuiltInSounds(builtInMetronomeAlias, firstBeatBuffer, secondaryBeatBuffer, sampleRate);
    }
    else{
        QString firstBeatAudioFile = mainController->getMetronomeFirstBeatFile();
        QString secondaryBeatAudioFile = mainController->getMetronomeSecondaryBeatFile();
        Audio::MetronomeUtils::createCustomSounds(firstBeatAudioFile, secondaryBeatAudioFile, firstBeatBuffer, secondaryBeatBuffer, sampleRate);
    }

    Audio::MetronomeUtils::removeSilenceInBufferStart(firstBeatBuffer);
    Audio::MetronomeUtils::removeSilenceInBufferStart(secondaryBeatBuffer);

    return new Audio::MetronomeTrackNode(firstBeatBuffer, secondaryBeatBuffer);
}

void NinjamController::recreateMetronome(int newSampleRate)
{
    //remove the old metronome
    float oldGain = metronomeTrackNode->getGain();
    float oldPan = metronomeTrackNode->getPan();
    bool oldMutedStatus = metronomeTrackNode->isMuted();
    bool oldSoloStatus = metronomeTrackNode->isSoloed();
    int oldBeatsPerAccent = metronomeTrackNode->getBeatsPerAccent();

    mainController->removeTrack(METRONOME_TRACK_ID);

    //recreate metronome using the new sample rate
    this->metronomeTrackNode = createMetronomeTrackNode(newSampleRate);
    this->metronomeTrackNode->setSamplesPerBeat(getSamplesPerBeat());
    this->metronomeTrackNode->setGain( oldGain );
    this->metronomeTrackNode->setPan( oldPan );
    this->metronomeTrackNode->setMute( oldMutedStatus );
    this->metronomeTrackNode->setSolo( oldSoloStatus );
    this->metronomeTrackNode->setBeatsPerAccent(oldBeatsPerAccent);
    mainController->addTrack(METRONOME_TRACK_ID, this->metronomeTrackNode);
}

//+++++++++++++++

void NinjamController::stop(bool emitDisconnectedingSignal){
    if(isRunning()){
        this->running = false;

        //store metronome settings
        Audio::AudioNode* metronomeTrack = mainController->getTrackNode(METRONOME_TRACK_ID);
        if(metronomeTrack){
            float metronomeGain = Utils::poweredGainToLinear(metronomeTrack->getGain());
            float metronomePan = metronomeTrack->getPan();
            bool metronomeIsMuted = metronomeTrack->isMuted();

            mainController->storeMetronomeSettings(metronomeGain, metronomePan, metronomeIsMuted);
            mainController->removeTrack(METRONOME_TRACK_ID);//remove metronome
        }

        //clear all tracks
        foreach(NinjamTrackNode* trackNode, trackNodes.values()){
            mainController->removeTrack(trackNode->getID());
        }
        trackNodes.clear();
    }

    if(encodingThread){
        encodingThread->stop();
        encodingThread->wait();//wait the encoding thread to finish
        delete encodingThread;
        encodingThread = nullptr;
    }

    foreach (VorbisEncoder* encoder, encoders.values()) {
        delete encoder;
    }
    encoders.clear();

    //delete possible non consumed events
    foreach (SchedulableEvent *e, scheduledEvents)
        delete e;
    scheduledEvents.clear();

    qCDebug(jtNinjamCore) << "NinjamController destructor - disconnecting...";

    Ninjam::Service* ninjamService = mainController->getNinjamService();// Ninjam::Service::getInstance();
    disconnect(ninjamService, SIGNAL(serverBpmChanged(quint16)), this, SLOT(on_ninjamServerBpmChanged(quint16)));
    disconnect(ninjamService, SIGNAL(serverBpiChanged(quint16,quint16)), this, SLOT(on_ninjamServerBpiChanged(quint16,quint16)));
    disconnect(ninjamService, SIGNAL(audioIntervalCompleted(const Ninjam::User &,quint8, const QByteArray &)), this, SLOT(on_ninjamAudiointervalCompleted(const Ninjam::User &,quint8, const QByteArray &)));

    disconnect(ninjamService, SIGNAL(userChannelCreated(const Ninjam::User &, const Ninjam::UserChannel &)), this, SLOT(on_ninjamUserChannelCreated(const Ninjam::User &, const Ninjam::UserChannel &)));
    disconnect(ninjamService, SIGNAL(userChannelRemoved(const Ninjam::User &, const Ninjam::UserChannel &)), this, SLOT(on_ninjamUserChannelRemoved(const Ninjam::User &, const Ninjam::UserChannel &)));
    disconnect(ninjamService, SIGNAL(userChannelUpdated(const Ninjam::User &, const Ninjam::UserChannel &)), this, SLOT(on_ninjamUserChannelUpdated(const Ninjam::User &, const Ninjam::UserChannel &)));
    disconnect(ninjamService, SIGNAL(audioIntervalDownloading(const Ninjam::User &,quint8,int)), this, SLOT(on_ninjamAudioIntervalDownloading(const Ninjam::User &,quint8,int)));

    disconnect(ninjamService, SIGNAL(chatMessageReceived(const Ninjam::User &, const QString &)), this, SIGNAL(chatMsgReceived(const Ninjam::User &, const QString &)));
    disconnect(ninjamService, SIGNAL(serverTopicMessageReceived(QString)), this, SIGNAL(topicMessageReceived(QString)));

    ninjamService->disconnectFromServer(emitDisconnectedingSignal);

}

NinjamController::~NinjamController(){
    //QMutexLocker locker(&mutex);
    qCDebug(jtNinjamCore) << "NinjamController destructor";
    if(isRunning()){
        stop(false);
    }

    //delete possible non consumed events
    foreach (SchedulableEvent *e, scheduledEvents) {
        delete e;
    }

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::start(const Ninjam::Server& server){
    qCDebug(jtNinjamCore) << "starting ninjam controller...";
    QMutexLocker locker(&mutex);

    //schedule an update in internal attributes
    scheduledEvents.append(new BpiChangeEvent(this, server.getBpi()));
    scheduledEvents.append(new BpmChangeEvent(this, server.getBpm()));
    preparedForTransmit = false; //the xmit start after the first interval is received
    emit preparingTransmission();

    //schedule the encoders creation (one encoder for each channel)
    int channels = mainController->getInputTrackGroupsCount();
    for (int channelIndex = 0; channelIndex < channels; ++channelIndex) {
        scheduledEvents.append(new InputChannelChangedEvent(this, channelIndex));
    }

    processScheduledChanges();

    if(!running){

        encodingThread = new NinjamController::EncodingThread(this);

        //add a sine wave generator as input to test audio transmission
        //mainController->addInputTrackNode(new Audio::LocalInputTestStreamer(440, mainController->getAudioDriverSampleRate()));


        mainController->addTrack(METRONOME_TRACK_ID, this->metronomeTrackNode);
        mainController->setTrackMute(METRONOME_TRACK_ID, mainController->getSettings().getMetronomeMuteStatus());
        mainController->setTrackGain(METRONOME_TRACK_ID,mainController->getSettings().getMetronomeGain());
        mainController->setTrackPan(METRONOME_TRACK_ID,  mainController->getSettings().getMetronomePan());

        this->intervalPosition  = lastBeat = 0;


        Ninjam::Service* ninjamService = mainController->getNinjamService();// Ninjam::Service::getInstance();
        connect(ninjamService, SIGNAL(serverBpmChanged(quint16)), this, SLOT(on_ninjamServerBpmChanged(quint16)));
        connect(ninjamService, SIGNAL(serverBpiChanged(quint16,quint16)), this, SLOT(on_ninjamServerBpiChanged(quint16,quint16)));
        connect(ninjamService, SIGNAL(audioIntervalCompleted(const Ninjam::User &,quint8, const QByteArray &)), this, SLOT(on_ninjamAudiointervalCompleted(const Ninjam::User &,quint8, const QByteArray &)));

        connect(ninjamService, SIGNAL(userChannelCreated(const Ninjam::User &, const Ninjam::UserChannel &)), this, SLOT(on_ninjamUserChannelCreated(const Ninjam::User &, const Ninjam::UserChannel &)));
        connect(ninjamService, SIGNAL(userChannelRemoved(const Ninjam::User &, const Ninjam::UserChannel &)), this, SLOT(on_ninjamUserChannelRemoved(const Ninjam::User &, const Ninjam::UserChannel &)));
        connect(ninjamService, SIGNAL(userChannelUpdated(const Ninjam::User &, const Ninjam::UserChannel &)), this, SLOT(on_ninjamUserChannelUpdated(const Ninjam::User &, const Ninjam::UserChannel &)));
        connect(ninjamService, SIGNAL(audioIntervalDownloading(const Ninjam::User &,quint8,int)), this, SLOT(on_ninjamAudioIntervalDownloading(const Ninjam::User &,quint8,int)));
        connect(ninjamService, SIGNAL(userExited(const Ninjam::User &)), this, SLOT(on_ninjamUserExited(const Ninjam::User &)));
        connect(ninjamService, SIGNAL(userEntered(const Ninjam::User &)), this, SLOT(on_ninjamUserEntered(const Ninjam::User &)));

        connect(ninjamService, SIGNAL(chatMessageReceived(Ninjam::User,QString)), this, SLOT(handleReceivedChatMessage(Ninjam::User,QString)));
        connect(ninjamService, SIGNAL(serverTopicMessageReceived(QString)), this, SIGNAL(topicMessageReceived(QString)));

        //add tracks for users connected in server
        QList<Ninjam::User> users = server.getUsers();
        foreach (const Ninjam::User &user, users) {
            foreach (const Ninjam::UserChannel &channel, user.getChannels()) {
                addTrack(user, channel);
            }
        }

        this->running = true;
    }
    qCDebug(jtNinjamCore) << "ninjam controller started!";
}

void NinjamController::blockUserInChat(const Ninjam::User &user)
{
    QString uniqueKey = getUniqueKeyForUser(user);
    if (!chatBlockedUsers.contains(uniqueKey)) {
        chatBlockedUsers.append(uniqueKey);
        emit userBlockedInChat(user.getName());
    }
}

void NinjamController::blockUserInChat(const QString &userNameToBlock)
{
    blockUserInChat(getUserByName(userNameToBlock));
}

void NinjamController::unblockUserInChat(const Ninjam::User &user)
{
    QString uniqueKey = getUniqueKeyForUser(user);
    if (chatBlockedUsers.removeOne(uniqueKey))
        emit userUnblockedInChat(user.getName());
}

void NinjamController::unblockUserInChat(const QString &userNameToBlock)
{
    unblockUserInChat(getUserByName(userNameToBlock));
}

bool NinjamController::userIsBlockedInChat(const Ninjam::User &user)
{
    QString uniqueKey = getUniqueKeyForUser(user);
    return chatBlockedUsers.contains(uniqueKey);
}

bool NinjamController::userIsBlockedInChat(const QString &userName) const
{
    return userIsBlockedInChat(getUserByName(userName));
}

void NinjamController::handleReceivedChatMessage(const Ninjam::User &user, const QString &message)
{
    if (!userIsBlockedInChat(user))
        emit chatMsgReceived(user, message);
}

void NinjamController::sendChatMessage(const QString &msg){
    mainController->getNinjamService()->sendChatMessageToServer(msg);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
long NinjamController::generateNewTrackID(){
    //QMutexLocker locker(&mutex);
    static long TRACK_IDS = 100;
    return TRACK_IDS++;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString NinjamController::getUniqueKeyForChannel(const Ninjam::UserChannel &channel)
{
    return channel.getUserFullName() + QString::number(channel.getIndex());
}

QString NinjamController::getUniqueKeyForUser(const Ninjam::User &user)
{
    return user.getFullName(); // full name is 'user_name@IP'
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool NinjamController::userIsBot(const QString userName) const
{
    if(mainController){
        return mainController->getBotNames().contains(userName);
    }
    return false;
}

void NinjamController::addTrack(const Ninjam::User &user, const Ninjam::UserChannel &channel){
    if(userIsBot(user.getName())){
        return;
    }
    NinjamTrackNode* trackNode = new NinjamTrackNode(generateNewTrackID());

    bool trackAdded = false;

    //checkThread("addTrack();");
    {
        QMutexLocker locker(&mutex);
        trackNodes.insert(getUniqueKeyForChannel(channel), trackNode);
    }//release the mutex before emit the signal
    trackAdded = mainController->addTrack(trackNode->getID(), trackNode);

    if(trackAdded){
        emit channelAdded(user,  channel, trackNode->getID());
    }
    else{
        QMutexLocker locker(&mutex);
        trackNodes.remove(getUniqueKeyForChannel(channel));
        delete trackNode;
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::removeTrack(const Ninjam::User &user, const Ninjam::UserChannel &channel){
    bool channelDeleted = false;
    long ID;
    {
        QMutexLocker locker(&mutex);
        //checkThread("removeTrack();");
        QString uniqueKey = getUniqueKeyForChannel(channel);

        if(trackNodes.contains(uniqueKey)){
            NinjamTrackNode* trackNode = trackNodes[uniqueKey];
            ID = trackNode->getID();
            trackNodes.remove(uniqueKey);
            mainController->removeTrack(ID);
            channelDeleted = true;
        }
    }
    if(channelDeleted){
        emit channelRemoved(user, channel, ID);
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::voteBpi(int bpi){
    mainController->getNinjamService()->voteToChangeBPI(bpi);
}

void NinjamController::voteBpm(int bpm){
    mainController->getNinjamService()->voteToChangeBPM(bpm);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::setMetronomeBeatsPerAccent(int beatsPerAccent){
    metronomeTrackNode->setBeatsPerAccent(beatsPerAccent);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//void NinjamController::deleteDeactivatedTracks(){
//    //QMutexLocker locker(&mutex);
//    checkThread("deleteDeactivatedTracks();");
//    QList<QString> keys = trackNodes.keys();
//    foreach (QString key, keys) {
//        NinjamTrackNode* trackNode = trackNodes[key];
//        if(!(trackNode->isActivated())){
//            trackNodes.remove(key);
//            mainController->removeTrack(trackNode->getID());
//            //delete trackNode; //BUG - sometimes Jamtaba crash when trackNode is deleted
//        }
//    }
//}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::handleNewInterval(){

    //check if the transmiting can start
    if(!preparedForTransmit){
        if(waitingIntervals >= TOTAL_PREPARED_INTERVALS){
            preparedForTransmit = true;
            waitingIntervals = 0;
            emit preparedToTransmit();
        }
        else{
            waitingIntervals++;
        }
    }

    if(hasScheduledChanges()){
        processScheduledChanges();
    }
    //QMutexLocker locker(&mutex);
    foreach (NinjamTrackNode* track, trackNodes.values()) {
        bool trackWasPlaying = track->isPlaying();
        bool trackIsPlaying = track->startNewInterval();
        if(trackWasPlaying != trackIsPlaying){
            emit channelXmitChanged(track->getID(), trackIsPlaying);
        }
    }
    qCDebug(jtNinjamCore) << "emitint startingNewInterval signal";
    emit startingNewInterval();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::processScheduledChanges(){
    foreach (SchedulableEvent* event, scheduledEvents) {
        event->process();
        delete event;
    }
    scheduledEvents.clear();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
long NinjamController::getSamplesPerBeat(){
    return computeTotalSamplesInInterval()/currentBpi;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
long NinjamController::computeTotalSamplesInInterval(){
    double intervalPeriod =  60000.0 / currentBpm * currentBpi;
    return (long)(mainController->getSampleRate() * intervalPeriod / 1000.0);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//ninjam slots
void NinjamController::on_ninjamUserEntered(const Ninjam::User &user){
    emit userEnter(user.getName());
}

void NinjamController::on_ninjamUserExited(const Ninjam::User &user){
     foreach (const Ninjam::UserChannel &channel, user.getChannels()) {
        removeTrack(user, channel);
     }
     emit userLeave(user.getName());
}

void NinjamController::on_ninjamUserChannelCreated(const Ninjam::User &user, const Ninjam::UserChannel &channel){
    addTrack(user, channel);
}

void NinjamController::on_ninjamUserChannelRemoved(const Ninjam::User &user, const Ninjam::UserChannel &channel){
    removeTrack(user, channel);
}

void NinjamController::on_ninjamUserChannelUpdated(const Ninjam::User &user, const Ninjam::UserChannel &channel){
    QString uniqueKey = getUniqueKeyForChannel(channel);
    QMutexLocker locker(&mutex);
    if(trackNodes.contains(uniqueKey)){
        NinjamTrackNode* trackNode = trackNodes[uniqueKey];
        emit channelNameChanged(user, channel, trackNode->getID());
    }

}

void NinjamController::on_ninjamServerBpiChanged(quint16 newBpi, quint16 oldBpi){
    Q_UNUSED(oldBpi);
    scheduledEvents.append(new BpiChangeEvent(this, newBpi));
}

void NinjamController::on_ninjamServerBpmChanged(quint16 newBpm){
    Q_UNUSED(newBpm)
    scheduledEvents.append(new BpmChangeEvent(this, newBpm));
}

void NinjamController::on_ninjamAudiointervalCompleted(const Ninjam::User &user, quint8 channelIndex, const QByteArray &encodedAudioData){

    if(mainController->isRecordingMultiTracksActivated()){
        Geo::Location geoLocation = mainController->getGeoLocation(user.getIp());
        QString userName = user.getName() + " from " + geoLocation.getCountryName();
        mainController->saveEncodedAudio(userName, channelIndex, encodedAudioData);
    }

    Ninjam::UserChannel channel = user.getChannel(channelIndex);
    QString channelKey = getUniqueKeyForChannel(channel);
    QMutexLocker locker(&mutex);
    if(trackNodes.contains(channelKey)){
        NinjamTrackNode* trackNode = trackNodes[channelKey];
        if(trackNode){
            trackNode->addVorbisEncodedInterval(encodedAudioData);
            emit channelAudioFullyDownloaded(trackNode->getID());
        }
    }
    else{
        qWarning() << "o canal " << channelIndex << " do usuário " << user.getName() << " não foi encontrado no mapa!";
    }
}

void NinjamController::reset(bool keepRecentIntervals){
    QMutexLocker locker(&mutex);
    foreach (NinjamTrackNode* trackNode, trackNodes.values()) {
        trackNode->discardIntervals(keepRecentIntervals);
    }
    intervalPosition = lastBeat = 0;
}

void NinjamController::scheduleEncoderChangeForChannel(int channelIndex){
    scheduledEvents.append(new InputChannelChangedEvent(this, channelIndex));
}

QByteArray NinjamController::encode(const Audio::SamplesBuffer &buffer, uint channelIndex){
    QMutexLocker locker(&encodersMutex);
    if(encoders.contains(channelIndex)){
        return encoders[channelIndex]->encode(buffer);
    }
    return QByteArray();
}

QByteArray NinjamController::encodeLastPartOfInterval(uint channelIndex){
    QMutexLocker locker(&encodersMutex);
    if(encoders.contains(channelIndex)){
        return encoders[channelIndex]->finishIntervalEncoding();
    }
    return QByteArray();
}

void NinjamController::recreateEncoderForChannel(int channelIndex){

    QMutexLocker locker(&encodersMutex);
    int maxChannelsForEncoding = mainController->getMaxChannelsForEncodingInTrackGroup(channelIndex);
    //qWarning() << "recreating encoding using " << maxChannelsForEncoding << " channels";
    if(maxChannelsForEncoding <= 0){//input tracks are setted as noInput?
        return;
    }
    bool currentEncoderIsInvalid = encoders.contains(channelIndex) &&
            (encoders[channelIndex]->getChannels() != maxChannelsForEncoding
                || encoders[channelIndex]->getSampleRate() != mainController->getSampleRate());

    if(!encoders.contains(channelIndex) || currentEncoderIsInvalid){//a new encoder is necessary?
        //qDebug() << "recreating encoder for channel index" << channelIndex;
        if(currentEncoderIsInvalid){
            delete encoders[channelIndex];
        }

        int sampleRate = mainController->getSampleRate();
        float encodingQuality = mainController->getEncodingQuality();
        encoders[channelIndex] = new VorbisEncoder(maxChannelsForEncoding, sampleRate, encodingQuality);
    }
}

void NinjamController::recreateEncoders(){
    if(isRunning()){
        QMutexLocker locker(&encodersMutex); //this method is called from main thread, and the encoders are used in audio thread every time
        for (int e = 0; e < encoders.size(); ++e) {
            delete encoders[e];
        }
        encoders.clear();//new encoders will be create on demand

        int trackGroupsCount = mainController->getInputTrackGroupsCount();
        for (int channelIndex = 0; channelIndex < trackGroupsCount; ++channelIndex) {
            recreateEncoderForChannel(channelIndex);
        }
    }
}

void NinjamController::setSampleRate(int newSampleRate){
    if(!isRunning()){
        return;
    }

    reset(false);//discard all downloaded intervals

    this->samplesInInterval = computeTotalSamplesInInterval();

    recreateMetronome(newSampleRate);

    recreateEncoders();
}

void NinjamController::on_ninjamAudioIntervalDownloading(const Ninjam::User &user, quint8 channelIndex, int downloadedBytes){
    Q_UNUSED(downloadedBytes);
    Ninjam::UserChannel channel = user.getChannel(channelIndex);
    QString channelKey = getUniqueKeyForChannel(channel);
    QMutexLocker locker(&mutex);
    if(trackNodes.contains(channelKey)){
        NinjamTrackNode* track = trackNodes[channelKey];
        if(track){
            if(!track->isPlaying()){//track is not playing yet and receive the first interval bytes
                emit channelXmitChanged(track->getID(), true);
            }
            emit channelAudioChunkDownloaded(track->getID());
        }
    }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

