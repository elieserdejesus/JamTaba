#include "MainController.h"
#include "recorder/ReaperProjectGenerator.h"
#include "audio/core/AudioDriver.h"
#include "midi/MidiDriver.h"
#include "audio/core/AudioMixer.h"
#include "audio/core/AudioNode.h"
#include "audio/RoomStreamerNode.h"
#include "persistence/Settings.h"
#include "gui/MainWindow.h"
#include "NinjamController.h"
#include "geo/WebIpToLocationResolver.h"
#include "audio/NinjamTrackNode.h"
#include "Utils.h"
#include "loginserver/LoginService.h"
#include "loginserver/natmap.h"
#include "ninjam/Service.h"
#include "ninjam/Server.h"
#include "audio/core/PluginDescriptor.h"

#include <QDateTime>
#include <QStandardPaths>
#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QApplication>
#include <QSysInfo>
#include <QUuid>
#include <QSettings>
#include <QDir>

#include "log/logging.h"
#include "configurator.h"

//QString Controller::MainController::LOG_CONFIG_FILE = "logging.ini";

//extern Configurator *JTBConfig;
using namespace Persistence;
using namespace Midi;
using namespace Ninjam;

//++++++++++++++++++++++++++++++

using namespace Controller;

//++++++++++++++++++++++++++++
//Nested class to group input tracks
class MainController::InputTrackGroup{
public:
    InputTrackGroup(int groupIndex, Audio::LocalInputAudioNode* firstInput)
        :groupIndex(groupIndex), transmiting(true)
    {
        addInput(firstInput);
    }

    ~InputTrackGroup(){
        groupedInputs.clear();
    }

    inline bool isEmpty() const{return groupedInputs.empty();}

    void addInput(Audio::LocalInputAudioNode* input){
        groupedInputs.append(input);
    }
    inline int getIndex() const{return groupIndex;}

    void mixGroupedInputs(Audio::SamplesBuffer& out){
        foreach (Audio::LocalInputAudioNode* inputTrack, groupedInputs) {
            if(!inputTrack->isMuted()){
                out.add(inputTrack->getLastBuffer());
            }
        }
    }

    void removeInput(Audio::LocalInputAudioNode* input){
       if(!groupedInputs.removeOne(input)){
           qCritical() << "the input track was not removed!";
       }
    }

    int getMaxInputChannelsForEncoding() const{
        if(groupedInputs.size() > 1){
            return 2;//stereo encoding
        }
        if(!groupedInputs.isEmpty()){
            if(groupedInputs.first()->isMidi()){
                return 2;//just one midi track, use stereo encoding
            }
            if(groupedInputs.first()->isAudio()){
                return groupedInputs.first()->getAudioInputRange().getChannels();
            }
            if(groupedInputs.first()->isNoInput()){
                return 2;//allow channels using noInput but processing some vst looper in stereo
            }
        }
        return 0;//no channels to encoding
    }

    inline bool isTransmiting() const{return transmiting;}
    void setTransmitingStatus(bool transmiting){this->transmiting = transmiting;}

private:
    int groupIndex;
    QList<Audio::LocalInputAudioNode*> groupedInputs;
    bool transmiting;
};

//+++++++++++++++++++++++++++++
//nested class to store interval upload data
class MainController::UploadIntervalData{
public:
    UploadIntervalData()
        :GUID(newGUID()){

    }

    inline QByteArray getGUID() const{return GUID;}

    void appendData(QByteArray encodedData){
        dataToUpload.append(encodedData);
    }

    inline int getTotalBytes() const{return dataToUpload.size();}

    inline QByteArray getStoredBytes() const{return dataToUpload;}

    inline void clear(){ dataToUpload.clear();}
private:
     static QByteArray newGUID(){
        QUuid uuid = QUuid::createUuid();
        return uuid.toRfc4122();
    }
    const QByteArray GUID;
    QByteArray dataToUpload;
};

//++++++++++++++++++++++++++++++++++++++++++++++
void MainController::setSampleRate(int newSampleRate){
    audioMixer.setSampleRate(newSampleRate);
    if(settings.isSaveMultiTrackActivated()){
        jamRecorder.setSampleRate(newSampleRate);
    }
    if(isPlayingInNinjamRoom()){
        ninjamController->setSampleRate(newSampleRate);
    }
}

void MainController::on_audioDriverSampleRateChanged(int newSampleRate){
    setSampleRate(newSampleRate);
}

void MainController::on_audioDriverStarted(){

}

void MainController::on_audioDriverStopped(){
    if(isPlayingInNinjamRoom()){
        //send the last interval part when audio driver is stopped
        finishUploads();
        ninjamController->reset();//discard downloaded intervals and reset interval position
    }
}

void MainController::finishUploads(){
    foreach (int channelIndex, intervalsToUpload.keys()) {
        ninjamService.sendAudioIntervalPart(intervalsToUpload[channelIndex]->getGUID(), QByteArray(), true);
    }
}

void MainController::on_errorInNinjamServer(QString error){
    qCWarning(jtCore) << error;
    stopNinjamController();
    //emit exitedFromRoom(false);//not a normal disconnection
    if(mainWindow){
        mainWindow->exitFromRoom(false);
    }
}

void MainController::on_disconnectedFromNinjamServer(const Server &server){
    Q_UNUSED(server);
    stopNinjamController();
    //emit exitedFromRoom(true);//normal disconnection
    if(mainWindow){
        mainWindow->exitFromRoom(true);
    }
    if(settings.isSaveMultiTrackActivated()){
        jamRecorder.stopRecording();
    }
}


void MainController::on_connectedInNinjamServer(Ninjam::Server server){
    qCDebug(jtCore) << "connected in ninjam server";
    stopNinjamController();
    Controller::NinjamController* newNinjamController = createNinjamController(this);// new Controller::NinjamController(this);
    this->ninjamController.reset( newNinjamController );
    QObject::connect(newNinjamController, SIGNAL(encodedAudioAvailableToSend(QByteArray,quint8,bool,bool)),
                     this, SLOT(on_ninjamEncodedAudioAvailableToSend(QByteArray,quint8,bool,  bool)));

    QObject::connect(newNinjamController, SIGNAL(startingNewInterval()), this, SLOT(on_newNinjamInterval()));
    QObject::connect(newNinjamController, SIGNAL(currentBpiChanged(int)), this, SLOT(on_ninjamBpiChanged(int)));
    QObject::connect(newNinjamController, SIGNAL(currentBpmChanged(int)), this, SLOT(on_ninjamBpmChanged(int)));
    QObject::connect(newNinjamController, SIGNAL(startProcessing(int)), this, SLOT(on_ninjamStartProcessing(int)));

    if(mainWindow){
        mainWindow->enterInRoom(Login::RoomInfo(server.getHostName(), server.getPort(), Login::RoomTYPE::NINJAM, server.getMaxUsers(), server.getMaxChannels()));
    }
    else{
        qCCritical(jtCore) << "mainWindow is null!";
    }
    qCDebug(jtCore) << "starting ninjamController...";


    newNinjamController->start(server, getXmitChannelsFlags());


    if(settings.isSaveMultiTrackActivated()){
        jamRecorder.startRecording(getUserName(), QDir(settings.getRecordingPath()), server.getBpm(), server.getBpi(), getSampleRate());
    }
}

QMap<int, bool> MainController::getXmitChannelsFlags() const{
    QMap<int, bool> xmitFlags;
    foreach (InputTrackGroup* inputGroup, trackGroups) {
        xmitFlags.insert( inputGroup->getIndex(), inputGroup->isTransmiting());
    }
    return xmitFlags;
}

void MainController::on_ninjamStartProcessing(int intervalPosition){
    Q_UNUSED(intervalPosition)
   // qDebug() << "MainController: on_ninjamStartProcessing";
}

void MainController::on_newNinjamInterval(){
    qCDebug(jtCore) << "MainController: on_newNinjamInterval";
    if(settings.isSaveMultiTrackActivated()){
        jamRecorder.newInterval();
    }
}

void MainController::on_ninjamBpiChanged(int newBpi){
    if(settings.isSaveMultiTrackActivated()){
        jamRecorder.setBpi(newBpi);
    }
}

void MainController::on_ninjamBpmChanged(int newBpm){
    if(settings.isSaveMultiTrackActivated()){
        jamRecorder.setBpm(newBpm);
    }
}

void MainController::on_ninjamEncodedAudioAvailableToSend(QByteArray encodedAudio, quint8 channelIndex, bool isFirstPart, bool isLastPart){

//    if(!ninjamService){
//        qCritical() << "ninjamService nulo";
//        return;
//    }
    //audio thread fire this event. This thread (main/gui thread)
    //write the encoded bytes in socket. We can't write in socket from audio thread.
    if(isFirstPart){
        if(intervalsToUpload.contains(channelIndex)){
            delete intervalsToUpload[channelIndex];
        }
        intervalsToUpload.insert(channelIndex, new MainController::UploadIntervalData());
        ninjamService.sendAudioIntervalBegin(intervalsToUpload[channelIndex]->getGUID(), channelIndex);
    }
    if(intervalsToUpload[channelIndex]){//just in case...
        MainController::UploadIntervalData* upload = intervalsToUpload[channelIndex];
        upload->appendData(encodedAudio);
        bool canSend = upload->getTotalBytes() >= 4096 || isLastPart;
        if( canSend  ){
            ninjamService.sendAudioIntervalPart( upload->getGUID(), upload->getStoredBytes(), isLastPart);
            //qWarning() << " sending " << upload->getTotalBytes();
            upload->clear();
        }
    }

    if(settings.isSaveMultiTrackActivated() && isPlayingInNinjamRoom()){
        jamRecorder.appendLocalUserAudio(encodedAudio, channelIndex, isFirstPart, isLastPart);
    }
}

//++++++++++++++++++++++++++++++++++++++++++++
MainController::MainController(Settings settings)
    :
      audioMixer(44100),
      currentStreamingRoomID(-1000),
      mutex(QMutex::Recursive),
      started(false),
      ipToLocationResolver( new Geo::WebIpToLocationResolver()),
      loginService(new Login::LoginService()),
      settings(settings),
      userNameChoosed(false),
      mainWindow(nullptr),
      jamRecorder(new Recorder::ReaperProjectGenerator())
{

}

//++++++++++++++++++++
int MainController::getMaxChannelsForEncodingInTrackGroup(uint trackGroupIndex) const{
    if(trackGroups.contains(trackGroupIndex)){
        InputTrackGroup* group = trackGroups[trackGroupIndex];
        if(group){
            return group->getMaxInputChannelsForEncoding();
        }
    }
    return 0;
}
//++++++++++++++++++++
void MainController::setUserName(QString newUserName){
    if(!newUserName.isEmpty()){
        settings.setUserName(newUserName);
        userNameChoosed = true;
    }
    else{
        qCritical() << "empty userNAme";
    }
}

QString MainController::getUserName() const{
    return settings.getUserName();
}
//++++++++++++++++++++

QStringList MainController::getBotNames() const{
    return Ninjam::Service::getBotNamesList();
}

Geo::Location MainController::getGeoLocation(QString ip) {
    return ipToLocationResolver->resolve(ip);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainController::mixGroupedInputs(int groupIndex, Audio::SamplesBuffer &out){
    if(groupIndex >= 0 && groupIndex < trackGroups.size()){
        trackGroups[groupIndex]->mixGroupedInputs(out);
    }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainController::updateInputTracksRange(){

    Audio::ChannelRange globalInputRange = audioDriver->getSelectedInputs();

    for (int trackIndex = 0; trackIndex < inputTracks.size(); ++trackIndex) {
        Audio::LocalInputAudioNode* inputTrack = getInputTrack(trackIndex);

        if(!inputTrack->isNoInput()){
            if(inputTrack->isAudio()){//audio track
                Audio::ChannelRange inputTrackRange = inputTrack->getAudioInputRange();
                inputTrack->setGlobalFirstInputIndex(globalInputRange.getFirstChannel());

                //    //If global input range is reduced to 2 channels and user previous selected inputs 3+4 the input range need be corrected to avoid a beautiful crash :)
                if(globalInputRange.getChannels() < inputTrackRange.getChannels()){
                    if(globalInputRange.isMono()){
                        setInputTrackToMono(trackIndex, globalInputRange.getFirstChannel());
                    }
                    else{
                        setInputTrackToNoInput(trackIndex);
                    }
                }

                //check if localInputRange is valid after the change in globalInputRange
                int firstChannel = inputTrackRange.getFirstChannel();
                int globalFirstChannel = globalInputRange.getFirstChannel();
                if( firstChannel < globalFirstChannel || inputTrackRange.getLastChannel() > globalInputRange.getLastChannel()){
                    if(globalInputRange.isMono()){
                        setInputTrackToMono(trackIndex, globalInputRange.getFirstChannel());
                    }else if(globalInputRange.getChannels() >= 2){
                        setInputTrackToStereo(trackIndex, globalInputRange.getFirstChannel());
                    }
                }
            }
            else{//midi track
                int selectedDevice = inputTrack->getMidiDeviceIndex();
                bool deviceIsValid = selectedDevice >= 0 && selectedDevice < midiDriver->getMaxInputDevices() && midiDriver->deviceIsGloballyEnabled(selectedDevice);
                if(!deviceIsValid){
                    //try another available midi input device
                    int firstAvailableDevice = midiDriver->getFirstGloballyEnableInputDevice();
                    if(firstAvailableDevice >= 0){
                        setInputTrackToMIDI(trackIndex, firstAvailableDevice, -1);//select all channels
                    }
                    else{
                        setInputTrackToNoInput(trackIndex);
                    }
                }
            }
        }
    }
}
//++++++++++++++++++++++++
//this is called when a new ninjam interval is received and the 'record multi track' option is enabled
void MainController::saveEncodedAudio(QString userName, quint8 channelIndex, QByteArray encodedAudio){
    if(settings.isSaveMultiTrackActivated()){//just in case
        jamRecorder.addRemoteUserAudio(userName, encodedAudio, channelIndex);
    }
}

//+++++++++++++++++++++++++++++++++++++++++++++++

void MainController::removeInputTrackNode(int inputTrackIndex){
    QMutexLocker locker(&mutex);
    if(inputTrackIndex >= 0 && inputTrackIndex < inputTracks.size()){
        //remove from group
        Audio::LocalInputAudioNode* inputTrack = inputTracks[inputTrackIndex];
        int trackGroupIndex = inputTrack->getGroupChannelIndex();
        if(trackGroups.contains(trackGroupIndex)){
            trackGroups[trackGroupIndex]->removeInput(inputTrack);
            if(trackGroups[trackGroupIndex]->isEmpty() ){
                trackGroups.remove(trackGroupIndex);
                //ninjamController->removeEncoder(trackGroupIndex);
            }
        }

        inputTracks.removeAt(inputTrackIndex);
        removeTrack(inputTrackIndex);
    }
}

int MainController::addInputTrackNode(Audio::LocalInputAudioNode *inputTrackNode){
    inputTracks.append(inputTrackNode);
    int inputTrackID = inputTracks.size() -1;
    addTrack(inputTrackID, inputTrackNode);

    int trackGroupIndex = inputTrackNode->getGroupChannelIndex();
    if(!trackGroups.contains(trackGroupIndex)){
        trackGroups.insert(trackGroupIndex, new MainController::InputTrackGroup(trackGroupIndex, inputTrackNode));
    }
    else{
        trackGroups[trackGroupIndex]->addInput(inputTrackNode);
    }

    if(isRunningAsVstPlugin()){//VST plugins always use audio as input
        //setInputTrackToStereo(0, 0);
        int firstChannelIndex = (inputTracks.size()-1) * 2;
        inputTrackNode->setAudioInputSelection(firstChannelIndex , 2);
    }

    return inputTrackID;
}

Audio::LocalInputAudioNode* MainController::getInputTrack(int localInputIndex){
    if(localInputIndex >= 0 && localInputIndex < inputTracks.size()){
        return inputTracks.at(localInputIndex);
    }
    qCritical() << "wrong input track index " << localInputIndex;
    return nullptr;
}

void MainController::setInputTrackToMono(int localChannelIndex, int inputIndexInAudioDevice){
    Audio::LocalInputAudioNode* inputTrack = getInputTrack(localChannelIndex);
    if(inputTrack){
        if( !inputIndexIsValid(inputIndexInAudioDevice) ){
            inputIndexInAudioDevice = audioDriver->getSelectedInputs().getFirstChannel();//use the first available channel
        }
        inputTrack->setAudioInputSelection(inputIndexInAudioDevice, 1);//mono
        //emit inputSelectionChanged(localChannelIndex);
        if(mainWindow){
            mainWindow->refreshTrackInputSelection(localChannelIndex);
        }
        if(isPlayingInNinjamRoom()){
            if(ninjamController){//just in case
                ninjamController->scheduleEncoderChangeForChannel(inputTrack->getGroupChannelIndex());
            }
        }
    }
}

bool MainController::inputIndexIsValid(int inputIndex){
    Audio::ChannelRange globalInputsRange = audioDriver->getSelectedInputs();
    return inputIndex >= globalInputsRange.getFirstChannel() && inputIndex <= globalInputsRange.getLastChannel();
}

void MainController::setInputTrackToStereo(int localChannelIndex, int firstInputIndex){
    Audio::LocalInputAudioNode* inputTrack = getInputTrack(localChannelIndex);
    if(inputTrack){

        if( !inputIndexIsValid(firstInputIndex) ){
            firstInputIndex = audioDriver->getSelectedInputs().getFirstChannel();//use the first available channel
        }
        inputTrack->setAudioInputSelection(firstInputIndex, 2);//stereo

        if(mainWindow){
            mainWindow->refreshTrackInputSelection(localChannelIndex);
        }
        if(isPlayingInNinjamRoom()){
            if(ninjamController){
                ninjamController->scheduleEncoderChangeForChannel(inputTrack->getGroupChannelIndex());
            }
        }
    }
}
void MainController::setInputTrackToMIDI(int localChannelIndex, int midiDevice, int midiChannel){
    Audio::LocalInputAudioNode* inputTrack = getInputTrack(localChannelIndex);
    if(inputTrack){
        inputTrack->setMidiInputSelection(midiDevice, midiChannel);
        if(mainWindow){
            mainWindow->refreshTrackInputSelection(localChannelIndex);
        }
        if(isPlayingInNinjamRoom()){
            if(ninjamController){
                ninjamController->scheduleEncoderChangeForChannel(inputTrack->getGroupChannelIndex());
            }
        }
    }
}
void MainController::setInputTrackToNoInput(int localChannelIndex){
    Audio::LocalInputAudioNode* inputTrack = getInputTrack(localChannelIndex);
    if(inputTrack){
        inputTrack->setToNoInput();
        if(mainWindow){
            mainWindow->refreshTrackInputSelection(localChannelIndex);
        }
        if(isPlayingInNinjamRoom()){//send the finish interval message
            if(intervalsToUpload.contains(localChannelIndex)){
                ninjamService.sendAudioIntervalPart(intervalsToUpload[localChannelIndex]->getGUID(), QByteArray(), true);
                if(ninjamController){
                    ninjamController->scheduleEncoderChangeForChannel(inputTrack->getGroupChannelIndex());
                }
            }
        }
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
Audio::AudioNode *MainController::getTrackNode(long ID){
    //QMutexLocker locker(&mutex);
    //checkThread("getTrackNode()");
    if(tracksNodes.contains(ID)){
        return tracksNodes[ID];
    }
    return nullptr;
}

bool MainController::addTrack(long trackID, Audio::AudioNode* trackNode){
    QMutexLocker locker(&mutex);
    //checkThread("addTrack()");

//    qDebug() << "adicionando track " << trackID;
//    if(!tracksNodes.contains(trackID)){
        tracksNodes.insert( trackID, trackNode );
        audioMixer.addNode(trackNode) ;
        return true;
//    }
//    else{
//        qCritical() << "Duplicated track ID";
//        foreach (long key, tracksNodes.keys()) {
//            qDebug() << "node:" << key;
//        }
//    }
//    return false;
}

//+++++++++++++++  SETTINGS +++++++++++
void MainController::storeRecordingMultiTracksStatus(bool savingMultiTracks){
    if(settings.isSaveMultiTrackActivated() && !savingMultiTracks){//user is disabling recording multi tracks?
        jamRecorder.stopRecording();
    }
    settings.setSaveMultiTrack(savingMultiTracks);
}

void MainController::storeRecordingPath(QString newPath){
    settings.setRecordingPath(newPath);
    if(settings.isSaveMultiTrackActivated()){
        jamRecorder.setRecordPath(newPath);
    }
}
//---------------------------------
void MainController::storePrivateServerSettings(QString server, int serverPort, QString password){
    settings.setPrivateServerData(server, serverPort, password);
}

//---------------------------------
void MainController::storeMetronomeSettings(float metronomeGain, float metronomePan, bool metronomeMuted){
    settings.setMetronomeSettings(metronomeGain, metronomePan, metronomeMuted);
}

void MainController::storeIntervalProgressShape(int shape){
    settings.setIntervalProgressShape(shape);
}



void MainController::storeWindowSettings(bool maximized, bool usingFullViewMode, QPointF location){
    settings.setWindowSettings(maximized, usingFullViewMode, location);
}

void MainController::storeIOSettings(int firstIn, int lastIn, int firstOut, int lastOut, int audioDevice, int sampleRate, int bufferSize, QList<bool> midiInputsStatus){
    settings.setAudioSettings(firstIn, lastIn, firstOut, lastOut, audioDevice, sampleRate, bufferSize);
    settings.setMidiSettings(midiInputsStatus);
}

//+++++++++++++++++

void MainController::removeTrack(long trackID){
    QMutexLocker locker(&mutex); //remove Track is called from ninjam service thread, and cause a crash if the process callback (audio Thread) is iterating over tracksNodes to render audio
    //checkThread("removeTrack();");

    Audio::AudioNode* trackNode = tracksNodes[trackID];
    if(trackNode){
        trackNode->suspendProcessors();
        audioMixer.removeNode(trackNode);
        tracksNodes.remove(trackID);
        delete trackNode;
    }
}



void MainController::doAudioProcess(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate){
    MidiBuffer midiBuffer ( midiDriver ? midiDriver->getBuffer() : MidiBuffer(0));
    int messages = midiBuffer.getMessagesCount();
    for(int m=0; m < messages; m++){
        Midi::MidiMessage msg = midiBuffer.getMessage(m);
        if(msg.isControl()){
            int inputTrackIndex = 0;//just for test for while, we need get this index from the mapping pair
            char cc = msg.getData1();
            char ccValue = msg.getData2();
            qCDebug(jtMidi) << "Control Change received: " << QString::number(cc) << " -> " << QString::number(ccValue);
            getInputTrack(inputTrackIndex)->setGain(ccValue/127.0);
        }
    }
    audioMixer.process(in, out, sampleRate, midiBuffer);
}


void MainController::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate){
    QMutexLocker locker(&mutex);
    if(!started){
        return;
    }

    if(!isPlayingInNinjamRoom()){
        doAudioProcess(in, out, sampleRate);
    }
    else{
        if(ninjamController){
            ninjamController->process(in, out, sampleRate);
        }
    }
    //recorder->addSamples(out);
}

Audio::AudioPeak MainController::getTrackPeak(int trackID){
    QMutexLocker locker(&mutex);
    //checkThread("getTrackPeak()");
    Audio::AudioNode* trackNode = tracksNodes[trackID];
    if(trackNode && !trackNode->isMuted()){
        return trackNode->getLastPeak();
    }
    if(!trackNode){
        qWarning() << "trackNode not found! ID:" << trackID;
    }
    return Audio::AudioPeak();
}

Audio::AudioPeak MainController::getRoomStreamPeak(){
    return roomStreamer->getLastPeak();
}

//++++++++++++++ XMIT +++++++++
void MainController::setTransmitingStatus(int channelID, bool transmiting){
    if(trackGroups.contains(channelID)){
        if(trackGroups[channelID]->isTransmiting() != transmiting){
            trackGroups[channelID]->setTransmitingStatus(transmiting);
            if(isPlayingInNinjamRoom()){
                ninjamController->setTransmitStatus(channelID, transmiting);
            }
        }
    }
}

bool MainController::isTransmiting(int channelID) const{
    if(trackGroups.contains(channelID)){
        return trackGroups[channelID]->isTransmiting();
    }
    return false;
}

//++++++++++ TRACKS ++++++++++++
void MainController::setTrackPan(int trackID, float pan, bool blockSignals){
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        node->blockSignals(blockSignals);
        node->setPan(pan);
        node->blockSignals(false);
    }
}

void MainController::setTrackBoost(int trackID, float boostInDecibels){
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        //qInfo() << "Boost: " << boostInDecibels << " linear:" << Utils::dbToLinear(boostInDecibels);
        node->setBoost(Utils::dbToLinear(boostInDecibels));
    }
}

void MainController::setTrackGain(int trackID, float gain, bool blockSignals){
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        node->blockSignals(blockSignals);
        node->setGain(Utils::linearGainToPower(gain));
        node->blockSignals(false);
    }
}

void MainController::setTrackMute(int trackID, bool muteStatus, bool blockSignals){
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        node->blockSignals(blockSignals);
        node->setMuteStatus(muteStatus);
        node->blockSignals(false);//unblock signals by default
    }
}

void MainController::setTrackSolo(int trackID, bool soloStatus, bool blockSignals){
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        node->blockSignals(blockSignals);
        node->setSoloStatus(soloStatus);
        node->blockSignals(false);
    }
}

bool MainController::trackIsMuted(int trackID) const{
    //QMutexLocker locker(&mutex);
    //checkThread("trackIsMuted()");
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        return node->isMuted();
    }
    return false;
}

bool MainController::trackIsSoloed(int trackID) const{
    //QMutexLocker locker(&mutex);
    //checkThread("trackIsSoloed()");
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        return node->isSoloed();
    }
    return false;
}

//+++++++++++++++++++++++++++++++++


MainController::~MainController(){
    qCDebug(jtCore()) << "MainController destrutor!";
    if(mainWindow){
        mainWindow->detachMainController();
    }

    stop();
    qCDebug(jtCore()) << "main controller stopped!";

    qCDebug(jtCore()) << "clearing tracksNodes...";
    tracksNodes.clear();
    foreach (Audio::LocalInputAudioNode* input, inputTracks) {
        delete input;
    }
    inputTracks.clear();

    foreach (MainController::InputTrackGroup* group, trackGroups) {
        delete group;
    }
    trackGroups.clear();
    qCDebug(jtCore()) << "clearing tracksNodes done!";

    qCDebug(jtCore) << "MainController destructor finished!";

}

void MainController::saveLastUserSettings(const Persistence::InputsSettings& inputsSettings){
    settings.save(inputsSettings);
}

//-------------------------      PRESETS   ----------------------------

void  MainController::loadPreset(QString name)
{
    settings.loadPreset(name);
}
QStringList MainController::getPresetList()
{
    return settings.getPresetList();
}

void  MainController::savePresets(const Persistence::InputsSettings &inputsSettings,QString name)
{
    settings.savePresets(inputsSettings,name);
}

void MainController::deletePreset(QString name)
{
    return settings.DeletePreset(name);
}

void MainController::setFullScreenView( bool fullScreen )
{
    settings.setFullScreenView(fullScreen);
}


void MainController::setAllTracksActivation(bool activated){
    foreach (Audio::AudioNode* track, tracksNodes) {
        if(activated)
            track->activate();
        else
            track->deactivate();
    }
}

void MainController::playRoomStream(Login::RoomInfo roomInfo){
    if(roomInfo.hasStream()){
        roomStreamer->setStreamPath(roomInfo.getStreamUrl());
        currentStreamingRoomID = roomInfo.getID();

        //mute all tracks and unmute the room Streamer
        setAllTracksActivation(true);
        roomStreamer->activate();
    }
}

void MainController::stopRoomStream(){
    roomStreamer->stopCurrentStream();
    currentStreamingRoomID = -1000;

    setAllTracksActivation(true);
    //roomStreamer->setMuteStatus(true);
}

bool MainController::isPlayingRoomStream() const{
    return roomStreamer->isStreaming();
}

void MainController::enterInRoom(Login::RoomInfo room, QStringList channelsNames, QString password){
    qCDebug(jtCore) << "EnterInRoom slot";
    if(isPlayingRoomStream()){
        stopRoomStream();
    }

    if(room.getType() == Login::RoomTYPE::NINJAM){
        tryConnectInNinjamServer(room, channelsNames, password);
    }
}

void MainController::sendNewChannelsNames(QStringList channelsNames){
    if(isPlayingInNinjamRoom()){
        ninjamService.sendNewChannelsListToServer(channelsNames);
    }
}

void MainController::sendRemovedChannelMessage(int removedChannelIndex){
    if(isPlayingInNinjamRoom()){
        ninjamService.sendRemovedChannelIndex(removedChannelIndex);
    }
}

void MainController::tryConnectInNinjamServer(Login::RoomInfo ninjamRoom, QStringList channelsNames, QString password){
    qCDebug(jtCore) << "connecting...";
    if(userNameWasChoosed()){//just in case :)
        QString serverIp = ninjamRoom.getName();
        int serverPort = ninjamRoom.getPort();
        QString userName = getUserName();
        QString pass = (password.isNull() || password.isEmpty()) ? "" : password;

        this->ninjamService.startServerConnection(serverIp, serverPort, userName, channelsNames, pass);
    }
    else{
        qCritical() << "user name not choosed yet!";
    }
}


void MainController::useNullAudioDriver(){
    qCWarning(jtCore) << "Audio driver can't be used, using NullAudioDriver!";
    audioDriver.reset(new Audio::NullAudioDriver());
}

void MainController::start(){

    if(!started){
        qCInfo(jtCore) << "Creating plugin finder...";
        pluginFinder.reset( createPluginFinder());

        if(!midiDriver){
            qCInfo(jtCore) << "Creating midi driver...";
            midiDriver.reset( createMidiDriver());
        }
        if(!audioDriver){
            qCInfo(jtCore) << "Creating audio driver...";
            Audio::AudioDriver* driver = nullptr;
            try{
                driver = createAudioDriver(settings);
            }
            catch(const std::runtime_error &error){
                qCCritical(jtCore) << "Audio initialization fail: " << QString::fromUtf8(error.what());
                QMessageBox::warning(mainWindow, "Audio Initialization Problem!", error.what());
            }
            if(!driver){
                driver = new Audio::NullAudioDriver();
            }
            audioDriver.reset( driver );
            QObject::connect(audioDriver.data(), SIGNAL(sampleRateChanged(int)), this, SLOT(on_audioDriverSampleRateChanged(int)));
            QObject::connect(audioDriver.data(), SIGNAL(stopped()), this, SLOT(on_audioDriverStopped()));
            QObject::connect(audioDriver.data(), SIGNAL(started()), this, SLOT(on_audioDriverStarted()));
        }

        qCInfo(jtCore) << "Creating roomStreamer ...";
        roomStreamer.reset( new Audio::NinjamRoomStreamerNode());//new Audio::AudioFileStreamerNode(":/teste.mp3");
        this->audioMixer.addNode( roomStreamer.data());

        QObject::connect(&ninjamService, SIGNAL(connectedInServer(Ninjam::Server)), this, SLOT(on_connectedInNinjamServer(Ninjam::Server)) );
        QObject::connect(&ninjamService, SIGNAL(disconnectedFromServer(Ninjam::Server)), this, SLOT(on_disconnectedFromNinjamServer(Ninjam::Server)));
        QObject::connect(&ninjamService, SIGNAL(error(QString)), this, SLOT(on_errorInNinjamServer(QString)));

        if(audioDriver ){
            if(!audioDriver->canBeStarted()){
                useNullAudioDriver();
            }
            audioDriver->start();
        }
        if(midiDriver){
            midiDriver->start();
        }

        NatMap map;//not used yet,will be used in future to real time rooms

        //connect with login server and receive a list of public rooms to play
        QString userEnvironment = getUserEnvironmentString();
        QString version = QApplication::applicationVersion();// applicationVersion();
        QString userName = settings.getUserName();
        if(userName.isEmpty()){
            userName = "No name!";
        }
        //CHAT TRANSLATION ?

        qCInfo(jtCore) << "Connecting in Jamtaba server...";
        loginService.connectInServer(userName, 0, "", map, version, userEnvironment, getSampleRate());
        //(QString userName, int instrumentID, QString channelName, const NatMap &localPeerMap, int version, QString environment, int sampleRate);

        qInfo() << "Starting " + userEnvironment;
        started = true;
    }
}

bool MainController::isUsingNullAudioDriver() const{
    return qobject_cast<Audio::NullAudioDriver*>(audioDriver.data()) != nullptr;
}

QString MainController::getUserEnvironmentString() const{
    QString systemName = QSysInfo::prettyProductName();
    QString userMachineArch = QSysInfo::currentCpuArchitecture();
    QString jamtabaArch = QSysInfo::buildCpuArchitecture();
    QString version = QApplication::applicationVersion();
    QString flavor = isRunningAsVstPlugin() ? "VST Plugin" : "Standalone";
    return "Jamtaba " + version + " " + flavor + " (" + jamtabaArch + ") running on " + systemName + " (" + userMachineArch + ")";
}

void MainController::stop()
{
    if(started){
        qCDebug(jtCore) << "Stopping MainController...";
        {
            //QMutexLocker locker(&mutex);
            if(audioDriver){
                this->audioDriver->release();
            }
            if(midiDriver){
                this->midiDriver->release();
            }
            qCDebug(jtCore) << "audio and midi drivers released";

            if(ninjamController){
                ninjamController->stop(false);//block disconnected signal
                //qCDebug(controllerMain) << "deleting ninjamController...";
                //delete ninjamController;
                //ninjamController = nullptr;
            }
            started = false;
        }

        qCDebug(jtCore) << "disconnecting from login server...";
        loginService.disconnectFromServer();

    }

}

//Audio::AudioDriver *MainController::getAudioDriver() const
//{
//    return audioDriver.data();
//}

Midi::MidiDriver* MainController::getMidiDriver() const{
    return midiDriver.data();
}

//+++++++++++
void MainController::cancelPluginFinder(){
    if(pluginFinder){
        pluginFinder->cancel();
    }
}

bool MainController::pluginDescriptorLessThan(const Audio::PluginDescriptor& d1, const Audio::PluginDescriptor& d2){
     return d1.getName().localeAwareCompare(d2.getName()) < 0;
}

void MainController::addPluginsScanPath(QString path){
    settings.addVstScanPath(path);
}

void MainController::removePluginsScanPath(int index){
   settings.removeVstScanPath(index);
}


void MainController::clearPluginsCache(){
    settings.clearVstCache();
}

//VST BlackList ...
void MainController::addBlackVstToSettings(QString path)
{
    settings.addVstToBlackList(path);
}

void MainController::removeBlackVst(int index){
   settings.RemVstFromBlackList(index);
}


QList<Audio::PluginDescriptor> MainController::getPluginsDescriptors(){

    qSort(pluginsDescriptors.begin(), pluginsDescriptors.end(), pluginDescriptorLessThan);
    return pluginsDescriptors;
    //Audio::getPluginsDescriptors(this->vstHost);

//        static std::vector<Audio::PluginDescriptor*> descriptors;
//        descriptors.clear();
//        //+++++++++++++++++
//        descriptors.push_back(new Audio::PluginDescriptor("Delay", "Jamtaba"));

//        Vst::PluginFinder finder;

//        //QString vstDir = "C:/Users/elieser/Desktop/TesteVSTs";
//        QString vstDir = "C:/Program Files (x86)/VSTPlugins/";
//        finder.addPathToScan(vstDir.toStdString());
//        std::vector<Audio::PluginDescriptor*> vstDescriptors = finder.scan(host);
//        //add all vstDescriptors
//        descriptors.insert(descriptors.end(), vstDescriptors.begin(), vstDescriptors.end());

//        //descriptors.push_back(new Audio::PluginDescriptor("OldSkool test", "VST", "C:/Program Files (x86)/VSTPlugins/OldSkoolVerb.dll"));

//        return descriptors;


}

Audio::Plugin* MainController::addPlugin(int inputTrackIndex, const Audio::PluginDescriptor& descriptor){
    Audio::Plugin* plugin = createPluginInstance(descriptor);
    if(plugin){
        plugin->start();
        QMutexLocker locker(&mutex);
        getInputTrack(inputTrackIndex)->addProcessor(plugin);
    }
    return plugin;
}


void MainController::removePlugin(int inputTrackIndex, Audio::Plugin *plugin){
    QMutexLocker locker(&mutex);
    QString pluginName = plugin->getName();
    try{
        getInputTrack(inputTrackIndex)->removeProcessor(plugin);
    }
    catch(...){
        qCritical() << "Erro removendo plugin " << pluginName;
    }
}


//+++++=

void MainController::configureStyleSheet(QString cssFile){
    QFile styleFile( ":/style/" + cssFile );
    if(!styleFile.open( QFile::ReadOnly )){
        qCritical("não carregou estilo!");
    }
    else{
        // Apply the loaded stylesheet
        setCSS(styleFile.readAll());
    }
}

Login::LoginService* MainController::getLoginService() const{
    return const_cast<Login::LoginService*>(&loginService);
}

bool MainController::isPlayingInNinjamRoom() const{
    if(ninjamController){
        return ninjamController->isRunning();
    }
    return false;
}

//++++++++++++= NINJAM ++++++++++++++++



void MainController::stopNinjamController(){
    QMutexLocker locker(&mutex);
    if(ninjamController && ninjamController->isRunning()){
        ninjamController->stop(true);
        //ninjamController->deleteLater();
        //delete ninjamController;
        //ninjamController = nullptr;
    }

}


