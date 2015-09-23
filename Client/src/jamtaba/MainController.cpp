#include "MainController.h"
#include "recorder/ReaperProjectGenerator.h"
#include "audio/core/AudioDriver.h"
#include "midi/portmididriver.h"
//#include "midi/MidiDriver.h"
#include "audio/core/PortAudioDriver.h"
#include "audio/core/AudioMixer.h"
#include "audio/core/AudioNode.h"
#include "audio/RoomStreamerNode.h"
#include "../loginserver/LoginService.h"
#include "../loginserver/natmap.h"

#include "../audio/core/plugins.h"

#include "JamtabaFactory.h"
#include "audio/core/plugins.h"

    #include "audio/vst/VstPlugin.h"
    #include "audio/vst/vsthost.h"

#include "persistence/Settings.h"

#include "../ninjam/Service.h"
#include "../ninjam/Server.h"
#include "NinjamController.h"

//#include "geo/MaxMindIpToLocationResolver.h"
//#include "geo/IpToLocationLITEResolver.h"
#include "geo/WebIpToLocationResolver.h"

#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QApplication>
#include <QSysInfo>
#include <QUuid>
#include <QSettings>
#include <QDir>
#include "audio/NinjamTrackNode.h"
#include "Utils.h"

Q_LOGGING_CATEGORY(controllerMain, "controller.main")

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
        :groupIndex(groupIndex){
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

private:
    int groupIndex;
    QList<Audio::LocalInputAudioNode*> groupedInputs;
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

//++++++++++++++++++++++++++++++++++++++++++++
MainController::MainController(JamtabaFactory* factory, Settings settings, int &argc, char **argv)
    :QApplication(argc, argv),
      audioMixer(nullptr),
      roomStreamer(nullptr),
      currentStreamingRoomID(-1000),


      transmiting(true),
      ninjamService(nullptr),
      ninjamController(nullptr),
      mutex(QMutex::Recursive),
      started(false),
        vstHost(Vst::VstHost::getInstance()),
      //pluginFinder(std::unique_ptr<Vst::PluginFinder>(new Vst::PluginFinder())),
      ipToLocationResolver( buildIpToLocationResolver()),
      loginService(factory->createLoginService()),
      settings(settings),
      userNameChoosed(false),
      jamRecorder(new Recorder::ReaperProjectGenerator())

{

    //threadHandle = nullptr;//QThread::currentThreadId();

    setQuitOnLastWindowClosed(false);//wait disconnect from server to close
    configureStyleSheet();

    this->audioDriver = buildAudioDriver(settings);


    QObject::connect(this->audioDriver, SIGNAL(sampleRateChanged(int)), this, SLOT(on_audioDriverSampleRateChanged(int)));
    QObject::connect(this->audioDriver, SIGNAL(stopped()), this, SLOT(on_audioDriverStopped()));
    QObject::connect(this->audioDriver, SIGNAL(started()), this, SLOT(on_audioDriverStarted()));

    audioMixer = new Audio::AudioMixer(audioDriver->getSampleRate());
    roomStreamer = new Audio::RoomStreamerNode();//new Audio::AudioFileStreamerNode(":/teste.mp3");

    //QString dateString = QDateTime::currentDateTime().time().toString().replace(":", "-");
    //QString fileName = "output_" + dateString + ".wav";
    //QString fileName = "output.wav";
    //recorder = new SamplesBufferRecorder(fileName, audioDriver->getSampleRate());

    midiDriver = new PortMidiDriver(settings.getMidiInputDevicesStatus());

    QObject::connect(loginService, SIGNAL(disconnectedFromServer()), this, SLOT(on_disconnectedFromLoginServer()));

    this->audioMixer->addNode( roomStreamer);


    vstHost->setSampleRate(audioDriver->getSampleRate());
    vstHost->setBlockSize(audioDriver->getBufferSize());


    QObject::connect(&pluginFinder, SIGNAL(pluginScanFinished(QString,QString,QString)), this, SLOT(on_VSTPluginFounded(QString,QString,QString)));

    //ninjam service
    this->ninjamService = Ninjam::Service::getInstance();
    QObject::connect( this->ninjamService, SIGNAL(connectedInServer(Ninjam::Server)), SLOT(on_connectedInNinjamServer(Ninjam::Server)) );
    QObject::connect(this->ninjamService, SIGNAL(disconnectedFromServer(Ninjam::Server)), this, SLOT(on_disconnectedFromNinjamServer(Ninjam::Server)));
    QObject::connect(this->ninjamService, SIGNAL(error(QString)), this, SLOT(on_errorInNinjamServer(QString)));

    //addInputTrackNode(new Audio::LocalInputTestStreamer(440, getAudioDriverSampleRate()));

/*
    //test ninjam stream
    NinjamTrackNode* trackTest = new NinjamTrackNode(2);
    //QStringList testFiles({":/bateria mono.ogg"});
    //QStringList testFiles({":/loop 192KHz.wav.ogg"});
    QStringList testFiles({":/loop estereo 44100KHz.ogg"});
    addTrack(2, trackTest);
    for (int i = 0; i < testFiles.size(); ++i) {
        QFile file(testFiles.at(i));
        if(!file.exists()){
            qCritical() << "File not exists! " << file.errorString();
        }
        file.open(QIODevice::ReadOnly);
        trackTest->addVorbisEncodedInterval(file.readAll());
    }
    trackTest->startNewInterval();
*/

    //QString vstDir = "C:/Users/elieser/Desktop/TesteVSTs";
    //QString vstDir = "C:/Program Files (x86)/VSTPlugins/";
    //pluginFinder->addPathToScan(vstDir.toStdString());
    //scanPlugins();

    //qDebug() << "QSetting in " << ConfigStore::getSettingsFilePath();
}
//++++++++++++++++++++
Geo::IpToLocationResolver* MainController::buildIpToLocationResolver(){
    return new Geo::WebIpToLocationResolver();
}

Audio::AudioDriver* MainController::buildAudioDriver(const Persistence::Settings &settings){
#ifdef Q_OS_WIN
    return new Audio::PortAudioDriver(
                    this, //the AudioDriverListener instance
                    settings.getLastInputDevice(), settings.getLastOutputDevice(),
                    settings.getFirstGlobalAudioInput(), settings.getLastGlobalAudioInput(),
                    settings.getFirstGlobalAudioOutput(), settings.getLastGlobalAudioOutput(),
                    settings.getLastSampleRate(), settings.getLastBufferSize()
                    );
#else
    //MAC
    return new Audio::PortAudioDriver(this, settings.getLastSampleRate(), settings.getLastBufferSize());
#endif
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
//this method is called when a new ninjam interval is received and the 'record multi track' option is enabled
void MainController::saveEncodedAudio(QString userName, quint8 channelIndex, QByteArray encodedAudio){
    if(settings.isSaveMultiTrackActivated()){//just in case
        jamRecorder.addRemoteUserAudio(userName, encodedAudio, channelIndex);
    }
}

//+++++++++++++++++++++=
//+++++++++++++++++++++++++++++++++++++++++++++++
//bool MainController::audioMonoInputIsFreeToSelect(int inputIndexInAudioDevice) const{
//    foreach (Audio::LocalInputAudioNode* inputTrack, inputTracks) {
//        if(!inputTrack->isNoInput() && !inputTrack->isMidi()){
//            Audio::ChannelRange trackRange = inputTrack->getAudioInputRange();
//            if(trackRange.getFirstChannel() == inputIndexInAudioDevice || trackRange.getLastChannel() == inputIndexInAudioDevice){
//                return false;
//            }
//        }
//    }
//    return true;
//}

//bool MainController::audioStereoInputIsFreeToSelect(int firstInputIndexInAudioDevice) const{
//    bool firstChannelIsFree = audioMonoInputIsFreeToSelect(firstInputIndexInAudioDevice);
//    bool secondChannelIsFree = audioMonoInputIsFreeToSelect(firstInputIndexInAudioDevice + 1);
//    return firstChannelIsFree && secondChannelIsFree;
//}

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

    //updateInputTracksRange();
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
        inputTrack->setAudioInputSelection(inputIndexInAudioDevice, 1);//mono
        emit inputSelectionChanged(localChannelIndex);
        if(isPlayingInNinjamRoom()){
            if(ninjamController){//just in case
                ninjamController->scheduleEncoderChangeForChannel(inputTrack->getGroupChannelIndex());
            }
        }
    }
}
void MainController::setInputTrackToStereo(int localChannelIndex, int firstInputIndex){
    Audio::LocalInputAudioNode* inputTrack = getInputTrack(localChannelIndex);
    if(inputTrack){
        inputTrack->setAudioInputSelection(firstInputIndex, 2);//stereo
        emit inputSelectionChanged(localChannelIndex);
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
        emit inputSelectionChanged(localChannelIndex);
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
        emit inputSelectionChanged(localChannelIndex);
        if(isPlayingInNinjamRoom()){//send the finish interval message
            if(intervalsToUpload.contains(localChannelIndex)){
                ninjamService->sendAudioIntervalPart(intervalsToUpload[localChannelIndex]->getGUID(), QByteArray(), true);
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
        audioMixer->addNode(trackNode) ;
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

void MainController::addVstScanPath(QString path){
    settings.addVstScanPath(path);
}

void MainController::addDefaultVstScanPath(){
    /*
    On a 64-bit OS

    64-bit plugins path = HKEY_LOCAL_MACHINE\SOFTWARE\VST
    32-bit plugins path = HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\VST
    */
    QStringList vstPaths;
#ifdef Q_OS_WIN
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\VST\\", QSettings::NativeFormat);
    vstPaths.append(settings.value("VSTPluginsPath").toString());

    #ifdef _WIN64//64 bits?
        QSettings wowSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\VST\\", QSettings::NativeFormat);
        vstPaths.append(wowSettings.value("VSTPluginsPath").toString());
    #endif
#endif

#ifdef Q_OS_MACX
       vstPaths.append("/Library/Audio/Plug-Ins/VST");
       vstPaths.append( QDir::homePath() + "/Library/Audio/Plug-Ins/VST");
#endif
    foreach (QString vstPath, vstPaths) {
        if(!vstPath.isEmpty() && QDir(vstPath).exists()){
            addVstScanPath(vstPath);
        }
    }
}

void MainController::removeVstScanPath(int index){
   settings.removeVstScanPath(index);
}

void MainController::clearVstCache(){
    settings.clearVstCache();
}

void MainController::storeWindowSettings(bool maximized, QPointF location){
    settings.setWindowSettings(maximized, location);
}

void MainController::storeIOSettings(int firstIn, int lastIn, int firstOut, int lastOut, int inputDevice, int outputDevice, int sampleRate, int bufferSize, QList<bool> midiInputsStatus){
    settings.setAudioSettings(firstIn, lastIn, firstOut, lastOut, inputDevice, outputDevice, sampleRate, bufferSize);
    settings.setMidiSettings(midiInputsStatus);
}

//+++++++++++++++++

void MainController::removeTrack(long trackID){
    QMutexLocker locker(&mutex); //remove Track is called from ninjam service thread, and cause a crash if the process callback (audio Thread) is iterating over tracksNodes to render audio
    //checkThread("removeTrack();");

    Audio::AudioNode* trackNode = tracksNodes[trackID];
    if(trackNode){
        trackNode->suspendProcessors();
        audioMixer->removeNode(trackNode);
        tracksNodes.remove(trackID);
        delete trackNode;
    }
}

void MainController::initializePluginsList(QStringList paths){
    pluginsDescriptors.clear();
    foreach (QString path, paths) {
        QFile file(path);
        if(file.exists()){
            QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(path);
            pluginsDescriptors.append(Audio::PluginDescriptor(pluginName, "VST", path));
        }
    }
}

void MainController::scanPlugins(){
    pluginsDescriptors.clear();
    //ConfigStore::clearVstCache();
    pluginFinder.clearScanPaths();
    QStringList scanPaths = settings.getVstScanPaths();

    foreach (QString path, scanPaths) {
        pluginFinder.addPathToScan(path);
    }
    pluginFinder.scan(vstHost);
}

void MainController::on_VSTPluginFounded(QString name, QString group, QString path){
    pluginsDescriptors.append(Audio::PluginDescriptor(name, group, path));
    settings.addVstPlugin(path);
}

void MainController::doAudioProcess(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate){
//    if(!threadHandle){
//        threadHandle = QThread::currentThreadId();
//    }
    //QMutexLocker locker(&mutex);
//    checkThread("doAudioProcess();");

    MidiBuffer midiBuffer = midiDriver->getBuffer();
    //vstHost->fillMidiEvents(midiBuffer);//pass midi events to vst host

    audioMixer->process(in, out, sampleRate, midiBuffer);
}


void MainController::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate){
    QMutexLocker locker(&mutex);
    //checkThread("process();");
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
        return trackNode->getLastPeak(true);//get last peak and reset
    }
    if(!trackNode){
        qWarning() << "trackNode not found! ID:" << trackID;
    }
    return Audio::AudioPeak();
}

Audio::AudioPeak MainController::getRoomStreamPeak(){
    return roomStreamer->getLastPeak(true);
}


void MainController::setTransmitingStatus(bool transmiting){
    if(this->transmiting != transmiting){
        this->transmiting = transmiting;
        if(isPlayingInNinjamRoom()){
            ninjamController->setTransmitStatus(transmiting);
        }
    }
}

//++++++++++ TRACKS ++++++++++++
void MainController::setTrackPan(int trackID, float pan){
    //QMutexLocker locker(&mutex);
    //checkThread("setTrackPan();");
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        node->setPan(pan);
    }
}

void MainController::setTrackLevel(int trackID, float level){
    //QMutexLocker locker(&mutex);
    //checkThread("setTrackLevel();");
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        //node->setGain( std::pow( level, 4));
        node->setGain(Utils::linearGainToPower(level));
    }
}

void MainController::setTrackMute(int trackID, bool muteStatus){
    //QMutexLocker locker(&mutex);
    //checkThread("setTrackMute();");
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        node->setMuteStatus(muteStatus);
    }
}

void MainController::setTrackSolo(int trackID, bool soloStatus){
    //QMutexLocker locker(&mutex);
    //checkThread("setTrackSolo();");
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        node->setSoloStatus(soloStatus);
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

bool MainController::pluginDescriptorLessThan(const Audio::PluginDescriptor& d1, const Audio::PluginDescriptor& d2){
     return d1.getName().localeAwareCompare(d2.getName()) < 0;
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
        //Audio::PluginWindow* window = plugin->getEditor();

        getInputTrack(inputTrackIndex)->removeProcessor(plugin);
        //window->deleteLater();
    }
    catch(...){
        qCritical() << "Erro removendo plugin " << pluginName;
    }
}

Audio::Plugin *MainController::createPluginInstance(const Audio::PluginDescriptor& descriptor)
{
    if(descriptor.isNative()){
        if(descriptor.getName() == "Delay"){
            return new Audio::JamtabaDelay(audioDriver->getSampleRate());
        }
    }
    else if(descriptor.isVST()){
            Vst::VstPlugin* vstPlugin = new Vst::VstPlugin(this->vstHost);
            if(vstPlugin->load( descriptor.getPath())){
                return vstPlugin;
            }
    }
    return nullptr;
}


void MainController::on_disconnectedFromLoginServer(){
    exit(0);
}

MainController::~MainController(){
    qCDebug(controllerMain()) << "MainController destrutor!";
    stop();

    QObject::disconnect(this->audioDriver, SIGNAL(sampleRateChanged(int)), this, SLOT(on_audioDriverSampleRateChanged(int)));
    //delete audioMixer; crashing :(
    delete audioDriver;
    delete midiDriver;

    delete roomStreamer;



//    foreach (Audio::AudioNode* track, tracksNodes) {
//        delete track;
//    }
    tracksNodes.clear();
    foreach (Audio::LocalInputAudioNode* input, inputTracks) {
        delete input;
    }
    inputTracks.clear();

    foreach (MainController::InputTrackGroup* group, trackGroups) {
        delete group;
    }
    trackGroups.clear();

    pluginsDescriptors.clear();

    if(this->ninjamController){
        delete ninjamController;
        ninjamController = nullptr;
    }
    //delete recorder;
    qCDebug(controllerMain) << "Finishing MainController destructor!";
}

void MainController::saveLastUserSettings(const Persistence::InputsSettings& inputsSettings){
    settings.save(inputsSettings);
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
        setAllTracksActivation(false);
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
    qCDebug(controllerMain) << "EnterInRoom slot";
    if(isPlayingRoomStream()){
        stopRoomStream();
    }

    if(room.getType() == Login::RoomTYPE::NINJAM){
        tryConnectInNinjamServer(room, channelsNames, password);
    }
}

void MainController::sendNewChannelsNames(QStringList channelsNames){
    if(isPlayingInNinjamRoom()){
        ninjamService->sendNewChannelsListToServer(channelsNames);
    }
}

void MainController::sendRemovedChannelMessage(int removedChannelIndex){
    if(isPlayingInNinjamRoom()){
        ninjamService->sendRemovedChannelIndex(removedChannelIndex);
    }
}

void MainController::tryConnectInNinjamServer(Login::RoomInfo ninjamRoom, QStringList channelsNames, QString password){
    qCDebug(controllerMain) << "connecting...";
    if(userNameWasChoosed()){//just in case :)
        QString serverIp = ninjamRoom.getName();
        int serverPort = ninjamRoom.getPort();
        QString userName = getUserName();
        QString pass = (password.isNull() || password.isEmpty()) ? "" : password;

        this->ninjamService->startServerConnection(serverIp, serverPort, userName, channelsNames, pass);
    }
    else{
        qCritical() << "user name not choosed yet!";
    }
}

void MainController::on_audioDriverSampleRateChanged(int newSampleRate){
    vstHost->setSampleRate(newSampleRate);
    audioMixer->setSampleRate(newSampleRate);

    if(settings.isSaveMultiTrackActivated()){
        jamRecorder.setSampleRate(newSampleRate);
    }
}

void MainController::on_audioDriverStarted(){
    vstHost->setSampleRate(audioDriver->getSampleRate());
    vstHost->setBlockSize(audioDriver->getBufferSize());

    foreach (Audio::LocalInputAudioNode* inputTrack, inputTracks) {
        inputTrack->resumeProcessors();
    }
}

void MainController::on_audioDriverStopped(){
    //threadHandle = nullptr;
    if(isPlayingInNinjamRoom()){
        //send the last interval part when audio driver is stopped
        foreach (int channelIndex, intervalsToUpload.keys()) {
            ninjamService->sendAudioIntervalPart(intervalsToUpload[channelIndex]->getGUID(), QByteArray(), true);
        }

    }

    foreach (Audio::LocalInputAudioNode* inputTrack, inputTracks) {
        inputTrack->suspendProcessors();//suspend plugins
    }


}

void MainController::start()
{
    if(!started){
        started = true;

        audioDriver->start();
        midiDriver->start();

        NatMap map;//not used yet,will be used in future to real time rooms

        //connect with login server and receive a list of public rooms to play

        QString userEnvironment = getUserEnvironmentString();
        QString version = applicationVersion();
        loginService->connectInServer("Jamtaba2 USER", 0, "", map, version, userEnvironment, getAudioDriverSampleRate());
        //(QString userName, int instrumentID, QString channelName, const NatMap &localPeerMap, int version, QString environment, int sampleRate);

        qCWarning(controllerMain) << "Starting " + userEnvironment;
    }
}

QString MainController::getUserEnvironmentString() const{
    QString systemName = QSysInfo::prettyProductName();
    QString userMachineArch = QSysInfo::currentCpuArchitecture();
    QString jamtabaArch = QSysInfo::buildCpuArchitecture();
    QString version = applicationVersion();
    return "Jamtaba " + version + " (" + jamtabaArch + ") running on " + systemName + " (" + userMachineArch + ")";
}

void MainController::stop()
{
    qCDebug(controllerMain) << "Stopping MainController...";
    if(started){
        {
            //QMutexLocker locker(&mutex);
            this->audioDriver->release();
            this->midiDriver->release();
            qCDebug(controllerMain) << "audio and midi drivers released";

            if(ninjamController){
                qCDebug(controllerMain) << "deleting ninjamController...";
                delete ninjamController;
                ninjamController = nullptr;
            }
            started = false;
        }

        qCDebug(controllerMain) << "disconnecting from login server...";
        loginService->disconnectFromServer();

    }
}

Audio::AudioDriver *MainController::getAudioDriver() const
{
    return audioDriver;
}

Midi::MidiDriver* MainController::getMidiDriver() const{
    return midiDriver;
}

void MainController::configureStyleSheet(){
    QFile styleFile( ":/style/jamtaba.css" );
    if(!styleFile.open( QFile::ReadOnly )){
        qFatal("não carregou estilo!");
    }

    // Apply the loaded stylesheet
    QString style( styleFile.readAll() );
    setStyleSheet( style );
}

Login::LoginService* MainController::getLoginService() const{
    return &*loginService;
}

bool MainController::isPlayingInNinjamRoom() const{
    if(ninjamController){
        return ninjamController->isRunning();
    }
    return false;
}

//++++++++++++= NINJAM ++++++++++++++++

void MainController::on_newNinjamInterval(){
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

    if(!ninjamService){
        qCritical() << "ninjamService nulo";
        return;
    }
    //audio thread fire this event. This thread (main/gui thread)
    //write the encoded bytes in socket. We can't write in socket from audio thread.
    if(isFirstPart){
        if(intervalsToUpload.contains(channelIndex)){
            delete intervalsToUpload[channelIndex];
        }
        intervalsToUpload.insert(channelIndex, new UploadIntervalData());
        ninjamService->sendAudioIntervalBegin(intervalsToUpload[channelIndex]->getGUID(), channelIndex);
    }
    if(intervalsToUpload[channelIndex]){//just in case...
        UploadIntervalData* upload = intervalsToUpload[channelIndex];
        upload->appendData(encodedAudio);
        bool canSend = upload->getTotalBytes() >= 4096 || isLastPart;
        if( canSend  ){
            ninjamService->sendAudioIntervalPart( upload->getGUID(), upload->getStoredBytes(), isLastPart);
            //qWarning() << " sending " << upload->getTotalBytes();
            upload->clear();
        }
    }

    if(settings.isSaveMultiTrackActivated()){
        jamRecorder.appendLocalUserAudio(encodedAudio, channelIndex, isFirstPart, isLastPart);
    }
}

void MainController::stopNinjamController(){
    QMutexLocker locker(&mutex);
    if(ninjamController){
        ninjamController->deleteLater();
        //delete ninjamController;
        ninjamController = nullptr;
    }
    vstHost->setPlayingFlag(false);
}

void MainController::on_errorInNinjamServer(QString error){
    qCWarning(controllerMain) << error;
    stopNinjamController();
    emit exitedFromRoom(false);//not a normal disconnection
}

void MainController::on_disconnectedFromNinjamServer(const Server &server){
    Q_UNUSED(server);
    stopNinjamController();
    emit exitedFromRoom(true);//normal disconnection
    if(settings.isSaveMultiTrackActivated()){
        jamRecorder.stopRecording();
    }
}

void MainController::on_connectedInNinjamServer(Ninjam::Server server){
    qCDebug(controllerMain) << "connected in ninjam server";
    stopNinjamController();
    ninjamController = new Controller::NinjamController(this);
    QObject::connect(ninjamController,
                     SIGNAL(encodedAudioAvailableToSend(QByteArray,quint8,bool,bool)),
                     this, SLOT(on_ninjamEncodedAudioAvailableToSend(QByteArray,quint8,bool,  bool)));
    QObject::connect(ninjamController, SIGNAL(startingNewInterval()), this, SLOT(on_newNinjamInterval()));
    QObject::connect(ninjamController, SIGNAL(currentBpiChanged(int)), this, SLOT(on_ninjamBpiChanged(int)));
    QObject::connect(ninjamController, SIGNAL(currentBpmChanged(int)), this, SLOT(on_ninjamBpmChanged(int)));

   //emit event after start controller to create view widgets before start
    emit enteredInRoom(Login::RoomInfo(server.getHostName(), server.getPort(), Login::RoomTYPE::NINJAM, server.getMaxUsers(), server.getMaxChannels()));

    qCDebug(controllerMain) << "starting ninjamController...";
    ninjamController->start(server, transmiting);
    vstHost->setTempo(server.getBpm());

    if(settings.isSaveMultiTrackActivated()){
        jamRecorder.startRecording(getUserName(), QDir(settings.getRecordingPath()), server.getBpm(), server.getBpi(), getAudioDriverSampleRate());
    }
}
