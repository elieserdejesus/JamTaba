#include "NinjamController.h"
#include "MainController.h"
#include "audio/core/AudioDriver.h"
#include "audio/core/SamplesBuffer.h"
#include "ninjam/Service.h"
#include "ninjam/User.h"
#include "ninjam/Server.h"
#include "audio/core/AudioNode.h"
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

using namespace Controller;

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
    //++++++++++++++++++++++++++++++
    void process(){
        controller->currentBpi = newBpi;
        controller->samplesInInterval = controller->computeTotalSamplesInInterval();
        emit controller->currentBpiChanged(controller->currentBpi);
    }
    //++++++++++++++++++++++++++++++
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
NinjamController::NinjamController(Controller::MainController* mainController)
    :mainController(mainController),
    metronomeTrackNode(createMetronomeTrackNode( mainController->getAudioDriverSampleRate())),
    intervalPosition(0),
    samplesInInterval(0),
    //newBpi(-1), newBpm(-1),
    currentBpi(0),
    currentBpm(0),
    mutex(QMutex::Recursive)
    //recorder("record.wav", mainController->getAudioDriver()->getSampleRate())
{
    running = false;
    QObject::connect( mainController->getAudioDriver(), SIGNAL(sampleRateChanged(int)), this, SLOT(on_audioDriverSampleRateChanged(int)));
    QObject::connect( mainController->getAudioDriver(), SIGNAL(stopped()), this, SLOT(on_audioDriverStopped()));
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
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::start(const Ninjam::Server& server){
    //schedule an update in internal attributes
    scheduledEvents.append(new BpiChangeEvent(this, server.getBpi()));
    scheduledEvents.append(new BpmChangeEvent(this, server.getBpm()));

    //schedule the encoders creation (one encoder for each channel)
    int channels = mainController->getInputTrackGroupsCount();
    for (int channelIndex = 0; channelIndex < channels; ++channelIndex) {
        scheduledEvents.append(new InputChannelChangedEvent(this, channelIndex));
    }
    //processScheduledChanges();

    if(!running){

        //Audio::OscillatorAudioNode* osc = new Audio::OscillatorAudioNode(440 * 2, mainController->getAudioDriver()->getSampleRate());
        //mainController->addTrack(-5, osc);
        //mainController->setTrackLevel(-5, 0.6);

        mainController->addTrack(METRONOME_TRACK_ID, this->metronomeTrackNode);
        mainController->setTrackLevel(METRONOME_TRACK_ID, mainController->getSettings().getMetronomeGain());
        mainController->setTrackPan(METRONOME_TRACK_ID, mainController->getSettings().getMetronomePan());

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

        //add server users
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
    {
        QMutexLocker locker(&mutex);
        trackNodes.insert(getUniqueKey(channel), trackNode);
        trackAdded = mainController->addTrack(trackNode->getID(), trackNode);
    }//release the mutex before emit the signal
    if(trackAdded){
        emit channelAdded(user,  channel, trackNode->getID());
    }
    else{
        trackNodes.remove(getUniqueKey(channel));
        delete trackNode;
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::removeTrack(Ninjam::User user, Ninjam::UserChannel channel){
    //qDebug() << "Removendo track " << channel.getName() << " do user " << user.getName();
    QMutexLocker locker(&mutex);
    QString uniqueKey = getUniqueKey(channel);
    if(trackNodes.contains(uniqueKey)){
        NinjamTrackNode* trackNode = trackNodes[uniqueKey];
        long ID = trackNode->getID();
        trackNode->deactivate();//tracks are deactivated and deleted in next step
        //mainController->removeTrack(ID);
        emit channelRemoved(user, channel, ID);
    }
    else{
        qDebug() << " não encontrou o channel em trackNodes";

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
    QMutexLocker locker(&mutex);
    if(running){
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


        //store metronome settings
        Audio::AudioNode* metronomeTrack = mainController->getTrackNode(METRONOME_TRACK_ID);
        if(metronomeTrack){
                                //  std::pow( metronomeTrack->getGain(), 1.0/4);//4th root - save the metronome gain in linear
            float correctedGain = Utils::poweredGainToLinear(metronomeTrack->getGain());
            mainController->storeMetronomeSettings(correctedGain, metronomeTrack->getPan());
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
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::deleteDeactivatedTracks(){
    QMutexLocker locker(&mutex);
    QList<QString> keys = trackNodes.keys();
    foreach (QString key, keys) {
        NinjamTrackNode* trackNode = trackNodes[key];
        if(!(trackNode->isActivated())){
            trackNodes.remove(key);
            mainController->removeTrack(trackNode->getID());
            //delete trackNode; //BUG - sometimes Jamtaba crash when trackNode is deleted
        }
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::handleNewInterval(){
    if(hasScheduledChanges()){
        processScheduledChanges();
    }
    QMutexLocker locker(&mutex);
    foreach (NinjamTrackNode* track, trackNodes.values()) {
        bool trackWasPlaying = track->isPlaying();
        bool trackIsPlaying = track->startNewInterval();
        if(trackWasPlaying != trackIsPlaying){
            emit channelXmitChanged(track->getID(), trackIsPlaying);
        }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamController::process(Audio::SamplesBuffer &in, Audio::SamplesBuffer &out){
    deleteDeactivatedTracks();

    //QMutexLocker locker(&mutex);
    //qDebug()<< "ninjamController process";

    int totalSamplesToProcess = out.getFrameLenght();
    int samplesProcessed = 0;
    static int lastBeat = 0;
    int offset = 0;
    do{
        int samplesToProcessInThisStep = std::min((int)(samplesInInterval - intervalPosition), totalSamplesToProcess - offset);
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
        //+++++++++++ MAIN PROCESS +++++++++++++++
        mainController->doAudioProcess(tempInBuffer, tempOutBuffer);
        out.add(tempOutBuffer, offset); //generate audio output


        // 1) group subchannels, 2) mix subchannels, 3) encode and 4) send the encoded audio

        // 1) group the subchannels using a map
        QMap<int, QList<Audio::LocalInputAudioNode*>> groups;
        int inputTracks = mainController->getInputTracksCount();
        for (int i = 0; i < inputTracks; ++i) {
            Audio::LocalInputAudioNode* inputTrack = mainController->getInputTrack(i);
            if(inputTrack && !inputTrack->isNoInput()){
                int channelIndex = inputTrack->getGroupChannelIndex();
                if(!groups.contains(channelIndex)){
                    groups.insert(channelIndex, QList<Audio::LocalInputAudioNode*>());
                }
                groups[channelIndex].append(inputTrack);
            }
        }

        // 2 - mix the groups subchannels
        static Audio::SamplesBuffer inputMixBuffer(2, 4096);
        inputMixBuffer.setFrameLenght(samplesToProcessInThisStep);
        foreach (int channelIndex, groups.keys()) {
            if(encoders.contains(channelIndex)){
                QList<Audio::LocalInputAudioNode*> inputTrackList = groups[channelIndex];

                inputMixBuffer.zero();
                //1 - mix the grouped subchannels
                foreach (Audio::LocalInputAudioNode* track, inputTrackList) {
                    inputMixBuffer.add(track->getLastBuffer());
                }

                // 3 - encoding
                assert(encoders[channelIndex]);
                QByteArray encodedBytes = encoders[channelIndex]->encode(inputMixBuffer);
                bool isLastPart = intervalPosition + samplesToProcessInThisStep >= samplesInInterval;
                bool isFirstPart = intervalPosition == 0;
                if(isLastPart){//get the last encoded bytes
                    encodedBytes.append( encoders[channelIndex]->finishIntervalEncoding() );
                }

                // 4 - send encoded bytes to main thread (only the main thread can do socket writing)
                emit encodedAudioAvailableToSend(encodedBytes, channelIndex, isFirstPart, isLastPart);
            }
            else{
                qDebug() << "encoder " << channelIndex << " not founded!";
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
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//VorbisEncoder* NinjamController::getEncoder(quint8 channelIndex){
//    QMutexLocker locker(&mutex);
//    if(!encoders.contains(channelIndex)){
//        //all subchannels in the channel are mono, stereo, or a mix?
//        int inputsCount = mainController->getInputTracksCount();
//        bool encoderWillBeStereo = false;
//        for (int i = 0; i < inputsCount; ++i) {
//            Audio::LocalInputAudioNode* inputTrack = mainController->getInputTrack(i);
//            assert(inputTrack);
//            if(!inputTrack->isNoInput() && !inputTrack->isMono()){
//                encoderWillBeStereo = true;// stereo inputTracks or midiTracks will use stereo encoders
//            }
//        }
//        encoders.insert(channelIndex, new VorbisEncoder(encoderWillBeStereo ? 2 : 1, mainController->getAudioDriverSampleRate()));
//    }
//    return encoders[channelIndex];
//}

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
    QMutexLocker locker(&mutex);
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
    QMutexLocker locker(&mutex);
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
    foreach (NinjamTrackNode* trackNode, this->trackNodes.values()) {
        trackNode->discardIntervals();
    }
    intervalPosition = 0;
}

void NinjamController::scheduleEncoderChangeForChannel(int channelIndex){
    scheduledEvents.append(new InputChannelChangedEvent(this, channelIndex));
}

void NinjamController::recreateEncoderForChannel(int channelIndex){
    qDebug() << "recreating encoder for channel index" << channelIndex;
    QMutexLocker locker(&mutex);
    int inputsCount = mainController->getInputTracksCount();
    int maxChannelsFounded = 1;//at least a mono channel
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

    bool currentEncoderIsInvalid = encoders.contains(channelIndex) && encoders[channelIndex]->getChannels() != maxChannelsFounded;

    if(!encoders.contains(channelIndex) || currentEncoderIsInvalid){//a new encoder is necessary?
        if(currentEncoderIsInvalid){
            delete encoders[channelIndex];
        }
        encoders[channelIndex] = new VorbisEncoder(maxChannelsFounded, mainController->getAudioDriverSampleRate());
    }
}

void NinjamController::recreateEncoders(){
//    QMutexLocker locker(&mutex); //this method is called from main thread, and the encoder are off course used in audio thread every time
//    for (int e = 0; e < encoders.size(); ++e) {
//        delete encoders[e];
//    }
//    encoders.clear();//new encoders will be create on demand
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
    QMutexLocker locker(&mutex);
    if(trackNodes.contains(channelKey)){
        NinjamTrackNode* track = dynamic_cast<NinjamTrackNode*>( trackNodes[channelKey]);
        if(!track->isPlaying()){//track is not playing yet and receive the first interval bytes
            emit channelXmitChanged(track->getID(), true);
        }
    }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

