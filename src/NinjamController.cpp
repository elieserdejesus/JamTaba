#include "NinjamController.h"
#include "MainController.h"
#include "audio/core/AudioDriver.h"
#include "audio/core/SamplesBuffer.h"
#include "ninjam/Service.h"
#include "ninjam/User.h"
#include "ninjam/Server.h"
#include "audio/core/AudioNode.h"
#include "audio/core/SamplesBuffer.h"
#include "../NinjamRoomWindow.h"
#include "../audio/NinjamTrackNode.h"
#include "../persistence/Settings.h"
#include "../audio/MetronomeTrackNode.h"
#include "../audio/vst/vsthost.h"

#include <cmath>
#include <cassert>
#include <QMutexLocker>
#include <QDebug>
#include <QThread>

#include "audio/samplesbufferrecorder.h"
#include "Utils.h"
#include <QWaitCondition>
#include "../log/logging.h"


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
    BpiChangeEvent(NinjamController* controller, int newBpi)
        :SchedulableEvent(controller), newBpi(newBpi){

    }
    void process(){
        controller->currentBpi = newBpi;
        controller->samplesInInterval = controller->computeTotalSamplesInInterval();
        emit controller->currentBpiChanged(controller->currentBpi);
    }
private:
    int newBpi;

};
//+++++++++++++++++++++++++++++++++++++++++
class NinjamController::BpmChangeEvent : public SchedulableEvent{
public:
    BpmChangeEvent(NinjamController* controller, int newBpm)
        :SchedulableEvent(controller), newBpm(newBpm){}
    void process(){
        controller->setBpm(newBpm);
    }
private:
    int newBpm;

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
class NinjamController::XmitChangedEvent : public SchedulableEvent{
public:
    XmitChangedEvent(NinjamController* controller, int channelID, bool transmiting)
        : SchedulableEvent(controller), transmiting(transmiting), channelID(channelID) {

    }
    void process(){
        controller->setTransmitStatus(channelID, transmiting);
    }

private:
    bool transmiting;
    int channelID;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NinjamController::NinjamController(Controller::MainController* mainController)
    :mainController(mainController),
    metronomeTrackNode(createMetronomeTrackNode( mainController->getSampleRate())),
    intervalPosition(0),
    samplesInInterval(0),
    currentBpi(0),
    currentBpm(0),
    mutex(QMutex::Recursive),
    encodersMutex(QMutex::Recursive),
    encodingThread(nullptr)

    //recorder("record.wav", mainController->getAudioDriver()->getSampleRate())
{
    running = false;

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
    static int lastBeat = 0;
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

        //++++++++++++++++++++++++++++++++++++++++
        samplesProcessed += samplesToProcessInThisStep;
        offset += samplesToProcessInThisStep;
        this->intervalPosition = (this->intervalPosition + samplesToProcessInThisStep) % samplesInInterval;

        //recorder.addSamples(tempBuffer);
    }
    while( samplesProcessed < totalSamplesToProcess);

    //mainController->getVstHost()->setTransportChangedFlag(false);

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Audio::MetronomeTrackNode* NinjamController::createMetronomeTrackNode(int sampleRate){
    return new Audio::MetronomeTrackNode(":/click.wav", sampleRate);
}

//+++++++++++++++

void NinjamController::stop(bool emitDisconnectedingSignal){
    if(isRunning()){
        this->running = false;

        //store metronome settings
        Audio::AudioNode* metronomeTrack = mainController->getTrackNode(METRONOME_TRACK_ID);
        if(metronomeTrack){
            //  std::pow( metronomeTrack->getGain(), 1.0/4);//4th root - save the metronome gain in linear
            float correctedGain = Utils::poweredGainToLinear(metronomeTrack->getGain());
            mainController->storeMetronomeSettings(correctedGain, metronomeTrack->getPan(), metronomeTrack->isMuted());
            mainController->removeTrack(METRONOME_TRACK_ID);//remove metronome
        }
        //clear all tracks
        foreach(NinjamTrackNode* trackNode, trackNodes.values()){
            mainController->removeTrack(trackNode->getID());
            //trackNode->deactivate();
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

    qCDebug(jtNinjamCore) << "NinjamController destructor - disconnecting...";

    Ninjam::Service* ninjamService = mainController->getNinjamService();// Ninjam::Service::getInstance();
    QObject::disconnect(ninjamService, SIGNAL(serverBpmChanged(short)), this, SLOT(on_ninjamServerBpmChanged(short)));
    QObject::disconnect(ninjamService, SIGNAL(serverBpiChanged(short,short)), this, SLOT(on_ninjamServerBpiChanged(short,short)));
    QObject::disconnect(ninjamService, SIGNAL(audioIntervalCompleted(Ninjam::User,int,QByteArray)), this, SLOT(on_ninjamAudiointervalCompleted(Ninjam::User,int,QByteArray)));
    //QObject::disconnect(ninjamService, SIGNAL(disconnectedFromServer(Ninjam::Server)), this, SLOT(on_ninjamDisconnectedFromServer(Ninjam::Server)));

    QObject::disconnect(ninjamService, SIGNAL(userChannelCreated(Ninjam::User, Ninjam::UserChannel)), this, SLOT(on_ninjamUserChannelCreated(Ninjam::User, Ninjam::UserChannel)));
    QObject::disconnect(ninjamService, SIGNAL(userChannelRemoved(Ninjam::User, Ninjam::UserChannel)), this, SLOT(on_ninjamUserChannelRemoved(Ninjam::User, Ninjam::UserChannel)));
    QObject::disconnect(ninjamService, SIGNAL(userChannelUpdated(Ninjam::User, Ninjam::UserChannel)), this, SLOT(on_ninjamUserChannelUpdated(Ninjam::User, Ninjam::UserChannel)));
    QObject::disconnect(ninjamService, SIGNAL(audioIntervalDownloading(Ninjam::User,int,int)), this, SLOT(on_ninjamAudiointervalDownloading(Ninjam::User,int,int)));

    QObject::disconnect(ninjamService, SIGNAL(chatMessageReceived(Ninjam::User,QString)), this, SIGNAL(chatMsgReceived(Ninjam::User,QString)));

    ninjamService->disconnectFromServer(emitDisconnectedingSignal);

}

NinjamController::~NinjamController(){
    //QMutexLocker locker(&mutex);
    qCDebug(jtNinjamCore) << "NinjamController destructor";
    if(isRunning()){
        stop(false);
    }

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::start(const Ninjam::Server& server, QMap<int, bool> channelsXmitFlags){
    qCDebug(jtNinjamCore) << "starting ninjam controller...";
    QMutexLocker locker(&mutex);
    //schedule an update in internal attributes
    scheduledEvents.append(new BpiChangeEvent(this, server.getBpi()));
    scheduledEvents.append(new BpmChangeEvent(this, server.getBpm()));
    foreach (int channelID, channelsXmitFlags.keys()) {
        bool channelIsTransmiting = channelsXmitFlags[channelID];
        scheduledEvents.append(new XmitChangedEvent(this, channelID, channelIsTransmiting));
    }


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
        mainController->setTrackLevel(METRONOME_TRACK_ID,mainController->getSettings().getMetronomeGain());
        mainController->setTrackPan(METRONOME_TRACK_ID,  mainController->getSettings().getMetronomePan());

        this->intervalPosition  = 0;


        Ninjam::Service* ninjamService = mainController->getNinjamService();// Ninjam::Service::getInstance();
        QObject::connect(ninjamService, SIGNAL(serverBpmChanged(short)), this, SLOT(on_ninjamServerBpmChanged(short)));
        QObject::connect(ninjamService, SIGNAL(serverBpiChanged(short,short)), this, SLOT(on_ninjamServerBpiChanged(short,short)));
        QObject::connect(ninjamService, SIGNAL(audioIntervalCompleted(Ninjam::User,int,QByteArray)), this, SLOT(on_ninjamAudiointervalCompleted(Ninjam::User,int,QByteArray)));
        //QObject::connect(ninjamService, SIGNAL(disconnectedFromServer(Ninjam::Server)), this, SLOT(on_ninjamDisconnectedFromServer(Ninjam::Server)));

        QObject::connect(ninjamService, SIGNAL(userChannelCreated(Ninjam::User, Ninjam::UserChannel)), this, SLOT(on_ninjamUserChannelCreated(Ninjam::User, Ninjam::UserChannel)));
        QObject::connect(ninjamService, SIGNAL(userChannelRemoved(Ninjam::User, Ninjam::UserChannel)), this, SLOT(on_ninjamUserChannelRemoved(Ninjam::User, Ninjam::UserChannel)));
        QObject::connect(ninjamService, SIGNAL(userChannelUpdated(Ninjam::User, Ninjam::UserChannel)), this, SLOT(on_ninjamUserChannelUpdated(Ninjam::User, Ninjam::UserChannel)));
        QObject::connect(ninjamService, SIGNAL(audioIntervalDownloading(Ninjam::User,int,int)), this, SLOT(on_ninjamAudiointervalDownloading(Ninjam::User,int,int)));
        QObject::connect(ninjamService, SIGNAL(userLeaveTheJam(Ninjam::User)), this, SLOT(on_ninjamUserLeave(Ninjam::User)));
        QObject::connect(ninjamService, SIGNAL(userEnterInTheJam(Ninjam::User)), this, SLOT(on_ninjamUserEnter(Ninjam::User)));

        QObject::connect(ninjamService, SIGNAL(chatMessageReceived(Ninjam::User,QString)), this, SIGNAL(chatMsgReceived(Ninjam::User,QString)));

        //add tracks for users connected in server
        QList<Ninjam::User*> users = server.getUsers();
        foreach (Ninjam::User* user, users) {
            foreach (Ninjam::UserChannel* channel, user->getChannels()) {
                addTrack(*user, *channel);
            }
        }

        this->running = true;
    }
    qCDebug(jtNinjamCore) << "ninjam controller started!";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::sendChatMessage(QString msg){
    mainController->getNinjamService()->sendChatMessageToServer(msg);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
long NinjamController::generateNewTrackID(){
    //QMutexLocker locker(&mutex);
    static long TRACK_IDS = 100;
    return TRACK_IDS++;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString NinjamController::getUniqueKey(Ninjam::UserChannel channel){
    return channel.getUserFullName() + QString::number(channel.getIndex());
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::addTrack(Ninjam::User user, Ninjam::UserChannel channel){
    if(user.isBot()){
        return;
    }
    NinjamTrackNode* trackNode = new NinjamTrackNode(generateNewTrackID());

    bool trackAdded = false;

    //checkThread("addTrack();");
    {
        QMutexLocker locker(&mutex);
        trackNodes.insert(getUniqueKey(channel), trackNode);
    }//release the mutex before emit the signal
    trackAdded = mainController->addTrack(trackNode->getID(), trackNode);

    if(trackAdded){
        emit channelAdded(user,  channel, trackNode->getID());
    }
    else{
        QMutexLocker locker(&mutex);
        trackNodes.remove(getUniqueKey(channel));
        delete trackNode;
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::removeTrack(Ninjam::User user, Ninjam::UserChannel channel){
    bool channelDeleted = false;
    long ID;
    {
        QMutexLocker locker(&mutex);
        //checkThread("removeTrack();");
        QString uniqueKey = getUniqueKey(channel);

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
//ninjam events

//void NinjamController::on_ninjamDisconnectedFromServer(Ninjam::Server server){
//    Q_UNUSED(server);
//    //emit disconnected(normalDisconnection);

//}

void NinjamController::on_ninjamUserEnter(Ninjam::User user){
    emit userEnter(user.getName());
}

void NinjamController::on_ninjamUserLeave(Ninjam::User user){
     foreach (Ninjam::UserChannel* channel, user.getChannels()) {
        removeTrack(user, *channel);
     }
     emit userLeave(user.getName());
}

void NinjamController::on_ninjamUserChannelCreated(Ninjam::User user, Ninjam::UserChannel channel){
    addTrack(user, channel);
}

void NinjamController::on_ninjamUserChannelRemoved(Ninjam::User user, Ninjam::UserChannel channel){
    removeTrack(user, channel);
}

void NinjamController::on_ninjamUserChannelUpdated(Ninjam::User user, Ninjam::UserChannel channel){
    QString uniqueKey = getUniqueKey(channel);
    QMutexLocker locker(&mutex);
    //checkThread("on_ninjamUserChannelUpdated();");
    if(trackNodes.contains(uniqueKey)){
        NinjamTrackNode* trackNode = trackNodes[uniqueKey];
        emit channelNameChanged(user, channel, trackNode->getID());
    }

}

void NinjamController::on_ninjamServerBpiChanged(short newBpi, short oldBpi){
    Q_UNUSED(oldBpi);
    //this->samplesInInterval = computeTotalSamplesInInterval();
    //this->newBpi = newBpi;
    scheduledEvents.append(new BpiChangeEvent(this, newBpi));
}

void NinjamController::on_ninjamServerBpmChanged(short newBpm){
    //this->metronomeTrackNode->setSamplesPerBeat(getSamplesPerBeat());
    //this->newBpm = newBpm;
    scheduledEvents.append(new BpmChangeEvent(this, newBpm));
}

void NinjamController::on_ninjamAudiointervalCompleted(Ninjam::User user, int channelIndex, QByteArray encodedAudioData){

    if(mainController->isRecordingMultiTracksActivated()){
        Geo::Location geoLocation = mainController->getGeoLocation(user.getIp());
        QString userName = user.getName() + " from " + geoLocation.getCountryName();
        mainController->saveEncodedAudio(userName, channelIndex, encodedAudioData);
    }

    //qDebug() << "audio available  Thread ID: " << QThread::currentThreadId();
    Ninjam::UserChannel channel = user.getChannel(channelIndex);
    QString channelKey = getUniqueKey(channel);
    QMutexLocker locker(&mutex);
    //checkThread("on_ninjamAudiointervalCompleted();");
    if(trackNodes.contains(channelKey)){
        NinjamTrackNode* trackNode = trackNodes[channelKey];
        trackNode->addVorbisEncodedInterval(encodedAudioData);
    }
    else{
        qWarning() << "o canal " << channelIndex << " do usuário " << user.getName() << " não foi encontrado no mapa!";
    }
}

void NinjamController::reset(){
    QMutexLocker locker(&mutex);
    //checkThread("on_audioDriverStopped();");
    foreach (NinjamTrackNode* trackNode, this->trackNodes.values()) {
        trackNode->discardIntervals();
    }
    intervalPosition = 0;
}


void NinjamController::setTransmitStatus(int channelID, bool transmiting){
    scheduledEvents.append(new XmitChangedEvent(this, channelID, transmiting));
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
        encoders[channelIndex] = new VorbisEncoder(maxChannelsForEncoding, mainController->getSampleRate());
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

    reset();//discard downloaded intervals

    this->samplesInInterval = computeTotalSamplesInInterval();

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
    this->metronomeTrackNode->setMuteStatus( oldMutedStatus );
    this->metronomeTrackNode->setSoloStatus( oldSoloStatus );
    this->metronomeTrackNode->setBeatsPerAccent(oldBeatsPerAccent);
    mainController->addTrack(METRONOME_TRACK_ID, this->metronomeTrackNode);

    recreateEncoders();
}



void NinjamController::on_ninjamAudiointervalDownloading(Ninjam::User user, int channelIndex, int downloadedBytes){
    Q_UNUSED(downloadedBytes);
    Ninjam::UserChannel channel = user.getChannel(channelIndex);
    QString channelKey = getUniqueKey(channel);
    QMutexLocker locker(&mutex);
    //checkThread("on_ninjamAudiointervalDownloading();");
    if(trackNodes.contains(channelKey)){
        NinjamTrackNode* track = dynamic_cast<NinjamTrackNode*>( trackNodes[channelKey]);
        if(!track->isPlaying()){//track is not playing yet and receive the first interval bytes
            emit channelXmitChanged(track->getID(), true);
        }
    }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

