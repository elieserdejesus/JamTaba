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
#include <QMutexLocker>
#include "../audio/MetronomeTrackNode.h"
#include <cmath>
#include <cassert>

#include <QDebug>
#include <QThread>

#include "audio/samplesbufferrecorder.h"
#include "Utils.h"
#include <QWaitCondition>

using namespace Controller;

//+++++++++++++  ENCODING THREAD +++++++++++++
class NinjamController::EncodingThread : public QThread{
public:
    EncodingThread(NinjamController* controller)
        :stopRequested(false), controller(controller){
        start();
    }

    ~EncodingThread(){
        stop();
    }

    void addSamplesToEncode(const Audio::SamplesBuffer& samplesToEncode, quint8 channelIndex, bool isFirstPart, bool isLastPart){
        QMutexLocker locker(&mutex);
        chunksToEncode.append(new EncodingChunk(samplesToEncode, channelIndex, isFirstPart, isLastPart));
        hasAvailableChunksToEncode.wakeAll();
    }

    void stop(){
        stopRequested = true;
    }

protected:
    void run(){
        while(!stopRequested){

            mutex.lock();
            while(chunksToEncode.isEmpty()){
                //qWarning() << "waiting...";
                hasAvailableChunksToEncode.wait(&mutex);
            }

            EncodingChunk* chunk = chunksToEncode.first();
            chunksToEncode.removeFirst();
            mutex.unlock();
            if(controller->encoders.contains(chunk->channelIndex)){
                QByteArray encodedBytes( controller->encoders[chunk->channelIndex]->encode(chunk->buffer));
                if(chunk->lastPart){
                    encodedBytes.append(controller->encoders[chunk->channelIndex]->finishIntervalEncoding());
                }

                //qWarning() << "emmitint";
                emit controller->encodedAudioAvailableToSend(encodedBytes, chunk->channelIndex, chunk->firstPart, chunk->lastPart );
            }

            delete chunk;

        }
        qWarning() << "Encoding thread stopped!";
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
    bool stopRequested;
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
        controller->currentBpm = newBpm;
        controller->samplesInInterval = controller->computeTotalSamplesInInterval();
        controller->metronomeTrackNode->setSamplesPerBeat(controller->getSamplesPerBeat());
        emit controller->currentBpmChanged(controller->currentBpm);
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
    XmitChangedEvent(NinjamController* controller, bool transmiting)
        : SchedulableEvent(controller), transmiting(transmiting) {

    }
    void process(){
        controller->transmiting = this->transmiting;
    }

private:
    bool transmiting;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NinjamController::NinjamController(Controller::MainController* mainController)
    :mainController(mainController),
    metronomeTrackNode(createMetronomeTrackNode( mainController->getAudioDriverSampleRate())),
    intervalPosition(0),
    samplesInInterval(0),
    //newBpi(-1), newBpm(-1),
    currentBpi(0),
    currentBpm(0),
    transmiting(false),
    mutex(QMutex::Recursive)

    //recorder("record.wav", mainController->getAudioDriver()->getSampleRate())
{
    running = false;
    QObject::connect( mainController->getAudioDriver(), SIGNAL(sampleRateChanged(int)), this, SLOT(on_audioDriverSampleRateChanged(int)));
    QObject::connect( mainController->getAudioDriver(), SIGNAL(stopped()), this, SLOT(on_audioDriverStopped()));

    threadHandle = nullptr;


}
//+++++++++++++++++++++++++ THE MAIN LOGIC IS HERE  ++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate){

    QMutexLocker locker(&mutex);

    if(samplesInInterval <= 0){
        return;//not initialized
    }

    if(!threadHandle){
        threadHandle = QThread::currentThreadId();
    }

    //deleteDeactivatedTracks();

    int totalSamplesToProcess = out.getFrameLenght();
    int samplesProcessed = 0;
    static int lastBeat = 0;
    int offset = 0;
    do{
        int samplesToProcessInThisStep = std::min((int)(samplesInInterval - intervalPosition), totalSamplesToProcess - offset);

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
            track->setProcessingLastPartOfInterval(isLastPart);//resampler need a flat indicating the last part
        }
        mainController->doAudioProcess(tempInBuffer, tempOutBuffer, sampleRate);
        out.add(tempOutBuffer, offset); //generate audio output
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++

        if( transmiting){
            bool isFirstPart = intervalPosition == 0;
            //1) mix input subchannels, 2) encode and 3) send the encoded audio

            static Audio::SamplesBuffer inputMixBuffer(2, 4096); //TODO esse canal sempre estereo nãi vai dar problema com pitas mono?
            inputMixBuffer.setFrameLenght(samplesToProcessInThisStep);

            int groupedChannels = mainController->getInputTrackGroupsCount();
            for (int channelIndex = 0; channelIndex < groupedChannels; ++channelIndex) {
                if(encoders.contains(channelIndex)){
                    inputMixBuffer.zero();
                    mainController->mixGroupedInputs(channelIndex, inputMixBuffer);

                    //encoding is running in another thread to avoid slow down the audio thread
                    encodingThread->addSamplesToEncode(inputMixBuffer, channelIndex, isFirstPart, isLastPart);
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


}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Audio::MetronomeTrackNode* NinjamController::createMetronomeTrackNode(int sampleRate){
    return new Audio::MetronomeTrackNode(":/click.wav", sampleRate);
}

//+++++++++++++++
NinjamController::~NinjamController(){
    delete metronomeTrackNode;
    QObject::disconnect( mainController->getAudioDriver(), SIGNAL(sampleRateChanged(int)), this, SLOT(on_audioDriverSampleRateChanged(int)));
    QObject::disconnect( mainController->getAudioDriver(), SIGNAL(stopped()), this, SLOT(on_audioDriverStopped()));
    if(encodingThread){
        delete encodingThread;
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::start(const Ninjam::Server& server, bool transmiting){
    QMutexLocker locker(&mutex);
    //schedule an update in internal attributes
    scheduledEvents.append(new BpiChangeEvent(this, server.getBpi()));
    scheduledEvents.append(new BpmChangeEvent(this, server.getBpm()));
    scheduledEvents.append(new XmitChangedEvent(this, transmiting));

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
        this->running = true;

        Ninjam::Service* ninjamService = Ninjam::Service::getInstance();
        QObject::connect(ninjamService, SIGNAL(serverBpmChanged(short)), this, SLOT(on_ninjamServerBpmChanged(short)));
        QObject::connect(ninjamService, SIGNAL(serverBpiChanged(short,short)), this, SLOT(on_ninjamServerBpiChanged(short,short)));
        QObject::connect(ninjamService, SIGNAL(audioIntervalCompleted(Ninjam::User,int,QByteArray)), this, SLOT(on_ninjamAudiointervalCompleted(Ninjam::User,int,QByteArray)));
        QObject::connect(ninjamService, SIGNAL(disconnectedFromServer(Ninjam::Server)), this, SLOT(on_ninjamDisconnectedFromServer(Ninjam::Server)));

        QObject::connect(ninjamService, SIGNAL(userChannelCreated(Ninjam::User, Ninjam::UserChannel)), this, SLOT(on_ninjamUserChannelCreated(Ninjam::User, Ninjam::UserChannel)));
        QObject::connect(ninjamService, SIGNAL(userChannelRemoved(Ninjam::User, Ninjam::UserChannel)), this, SLOT(on_ninjamUserChannelRemoved(Ninjam::User, Ninjam::UserChannel)));
        QObject::connect(ninjamService, SIGNAL(userChannelUpdated(Ninjam::User, Ninjam::UserChannel)), this, SLOT(on_ninjamUserChannelUpdated(Ninjam::User, Ninjam::UserChannel)));
        QObject::connect(ninjamService, SIGNAL(audioIntervalDownloading(Ninjam::User,int,int)), this, SLOT(on_ninjamAudiointervalDownloading(Ninjam::User,int,int)));

        QObject::connect(ninjamService, SIGNAL(chatMessageReceived(Ninjam::User,QString)), this, SIGNAL(chatMsgReceived(Ninjam::User,QString)));

        //add tracks for users connected in server
        QList<Ninjam::User*> users = server.getUsers();
        foreach (Ninjam::User* user, users) {
            foreach (Ninjam::UserChannel* channel, user->getChannels()) {
                addTrack(*user, *channel);
            }
        }
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::sendChatMessage(QString msg){
    Ninjam::Service::getInstance()->sendChatMessageToServer(msg);
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
            delete trackNode; //BUG - sometimes Jamtaba crash when trackNode is deleted
            channelDeleted = true;
        }
    }
    if(channelDeleted){
        emit channelRemoved(user, channel, ID);
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::voteBpi(int bpi){
    Ninjam::Service::getInstance()->voteToChangeBPI(bpi);
}

void NinjamController::voteBpm(int bpm){
    Ninjam::Service::getInstance()->voteToChangeBPM(bpm);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::setMetronomeBeatsPerAccent(int beatsPerAccent){
    metronomeTrackNode->setBeatsPerAccent(beatsPerAccent);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::stop(){

    //checkThread("stop();");
    if(running){

        delete encodingThread;
        encodingThread = nullptr;

        this->running = false;

        Ninjam::Service* ninjamService = Ninjam::Service::getInstance();
        QObject::disconnect(ninjamService, SIGNAL(serverBpmChanged(short)), this, SLOT(on_ninjamServerBpmChanged(short)));
        QObject::disconnect(ninjamService, SIGNAL(serverBpiChanged(short,short)), this, SLOT(on_ninjamServerBpiChanged(short,short)));
        QObject::disconnect(ninjamService, SIGNAL(audioIntervalCompleted(Ninjam::User,int,QByteArray)), this, SLOT(on_ninjamAudiointervalCompleted(Ninjam::User,int,QByteArray)));
        QObject::disconnect(ninjamService, SIGNAL(disconnectedFromServer(Ninjam::Server)), this, SLOT(on_ninjamDisconnectedFromServer(Ninjam::Server)));

        QObject::disconnect(ninjamService, SIGNAL(userChannelCreated(Ninjam::User, Ninjam::UserChannel)), this, SLOT(on_ninjamUserChannelCreated(Ninjam::User, Ninjam::UserChannel)));
        QObject::disconnect(ninjamService, SIGNAL(userChannelRemoved(Ninjam::User, Ninjam::UserChannel)), this, SLOT(on_ninjamUserChannelRemoved(Ninjam::User, Ninjam::UserChannel)));
        QObject::disconnect(ninjamService, SIGNAL(userChannelUpdated(Ninjam::User, Ninjam::UserChannel)), this, SLOT(on_ninjamUserChannelUpdated(Ninjam::User, Ninjam::UserChannel)));
        QObject::disconnect(ninjamService, SIGNAL(audioIntervalDownloading(Ninjam::User,int,int)), this, SLOT(on_ninjamAudiointervalDownloading(Ninjam::User,int,int)));

        QObject::disconnect(ninjamService, SIGNAL(chatMessageReceived(Ninjam::User,QString)), this, SIGNAL(chatMsgReceived(Ninjam::User,QString)));

        ninjamService->disconnectFromServer();


        {
            QMutexLocker locker(&mutex);

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
                trackNode->deactivate();
            }
            trackNodes.clear();
        }
    }
}
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
    checkThread("handleNewInterval();");
    foreach (NinjamTrackNode* track, trackNodes.values()) {
        bool trackWasPlaying = track->isPlaying();
        bool trackIsPlaying = track->startNewInterval();
        if(trackWasPlaying != trackIsPlaying){
            emit channelXmitChanged(track->getID(), trackIsPlaying);
        }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::checkThread(QString methodName){
    if(threadHandle && QThread::currentThreadId() != threadHandle){
        qCritical() << "diferente thread detected! " << methodName;
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
    int sampleRate = mainController->getAudioDriver()->getSampleRate();
    return (long)(sampleRate * intervalPeriod / 1000.0);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//ninjam events

void NinjamController::on_ninjamDisconnectedFromServer(Ninjam::Server server){
    Q_UNUSED(server);
    //emit disconnected(normalDisconnection);

}

void NinjamController::on_ninjamUserLeave(Ninjam::User user){
     foreach (Ninjam::UserChannel* channel, user.getChannels()) {
        removeTrack(user, *channel);
     }
}

void NinjamController::on_ninjamUserChannelCreated(Ninjam::User user, Ninjam::UserChannel channel){
    addTrack(user, channel);
}

void NinjamController::on_ninjamUserChannelRemoved(Ninjam::User user, Ninjam::UserChannel channel){
    removeTrack(user, channel);
}

void NinjamController::on_ninjamUserChannelUpdated(Ninjam::User user, Ninjam::UserChannel channel){
    QString uniqueKey = getUniqueKey(channel);
    //QMutexLocker locker(&mutex);
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
    //qDebug() << "audio available  Thread ID: " << QThread::currentThreadId();
    Ninjam::UserChannel channel = user.getChannel(channelIndex);
    QString channelKey = getUniqueKey(channel);
    //QMutexLocker locker(&mutex);
    //checkThread("on_ninjamAudiointervalCompleted();");
    if(trackNodes.contains(channelKey)){
        NinjamTrackNode* trackNode = trackNodes[channelKey];
        trackNode->addVorbisEncodedInterval(encodedAudioData);
    }
    else{
        qWarning() << "o canal " << channelIndex << " do usuário " << user.getName() << " não foi encontrado no mapa!";
    }
}

void NinjamController::on_audioDriverStopped(){
    QMutexLocker locker(&mutex);
    //checkThread("on_audioDriverStopped();");
    foreach (NinjamTrackNode* trackNode, this->trackNodes.values()) {
        trackNode->discardIntervals();
    }
    intervalPosition = 0;
    threadHandle = nullptr;
}

void NinjamController::setTransmitStatus(bool transmiting){
    scheduledEvents.append(new XmitChangedEvent(this, transmiting));
}

void NinjamController::scheduleEncoderChangeForChannel(int channelIndex){
    scheduledEvents.append(new InputChannelChangedEvent(this, channelIndex));
}

void NinjamController::recreateEncoderForChannel(int channelIndex){

    //QMutexLocker locker(&mutex);
    checkThread("recreateEncoderForChannel() ");
    int inputsCount = mainController->getInputTracksCount();
    int maxChannelsFounded = 0;//at least a mono channel
    for (int i = 0; i < inputsCount; ++i) {
        Audio::LocalInputAudioNode* inputTrack = mainController->getInputTrack(i);
        if(inputTrack && inputTrack->getGroupChannelIndex() == channelIndex){
            if( !inputTrack->isNoInput()  ){//stereo or midi tracks are always stereo
                int trackChannels = inputTrack->isMidi() ? 2 : inputTrack->getAudioInputRange().getChannels();
                if(trackChannels > maxChannelsFounded){
                    maxChannelsFounded = inputTrack->getAudioInputRange().getChannels();
                }
            }
        }
    }
    if(maxChannelsFounded <= 0){//input tracks are setted as noInput
        return;
    }
    bool currentEncoderIsInvalid = encoders.contains(channelIndex) && encoders[channelIndex]->getChannels() != maxChannelsFounded;

    if(!encoders.contains(channelIndex) || currentEncoderIsInvalid){//a new encoder is necessary?
        //qDebug() << "recreating encoder for channel index" << channelIndex;
        if(currentEncoderIsInvalid){
            delete encoders[channelIndex];
        }
        encoders[channelIndex] = new VorbisEncoder(maxChannelsFounded, mainController->getAudioDriverSampleRate());
    }
}

void NinjamController::recreateEncoders(){
    if(isRunning()){
        QMutexLocker locker(&mutex); //this method is called from main thread, and the encoder are off course used in audio thread every time
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

void NinjamController::on_audioDriverSampleRateChanged(int newSampleRate){
    if(!isRunning()){
        return;
    }
    Q_UNUSED(newSampleRate);
    this->samplesInInterval = computeTotalSamplesInInterval();

    //remove the old metronome
    mainController->removeTrack(METRONOME_TRACK_ID);
    Audio::MetronomeTrackNode* oldMetronomeTrack = this->metronomeTrackNode;

    //recreate metronome using the new sample rate
    this->metronomeTrackNode = createMetronomeTrackNode(newSampleRate);
    this->metronomeTrackNode->setSamplesPerBeat(getSamplesPerBeat());
    this->metronomeTrackNode->setGain(oldMetronomeTrack->getGain());
    this->metronomeTrackNode->setPan(oldMetronomeTrack->getPan());
    mainController->addTrack(METRONOME_TRACK_ID, this->metronomeTrackNode);

    delete oldMetronomeTrack;

    recreateEncoders();
}

void NinjamController::on_ninjamAudiointervalDownloading(Ninjam::User user, int channelIndex, int downloadedBytes){
    Q_UNUSED(downloadedBytes);
    Ninjam::UserChannel channel = user.getChannel(channelIndex);
    QString channelKey = getUniqueKey(channel);
    //QMutexLocker locker(&mutex);
    //checkThread("on_ninjamAudiointervalDownloading();");
    if(trackNodes.contains(channelKey)){
        NinjamTrackNode* track = dynamic_cast<NinjamTrackNode*>( trackNodes[channelKey]);
        if(!track->isPlaying()){//track is not playing yet and receive the first interval bytes
            emit channelXmitChanged(track->getID(), true);
        }
    }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

