#include "MainController.h"
#include "audio/core/AudioDriver.h"
#include "midi/portmididriver.h"
#include "audio/core/PortAudioDriver.h"
#include "audio/core/AudioMixer.h"
#include "audio/core/AudioNode.h"
#include "audio/RoomStreamerNode.h"
#include "../loginserver/LoginService.h"
#include "../loginserver/natmap.h"

#include "JamtabaFactory.h"
#include "audio/core/plugins.h"
#include "audio/vst/VstPlugin.h"
#include "audio/vst/vsthost.h"
#include "persistence/Settings.h"

#include "../ninjam/Service.h"
#include "../ninjam/Server.h"
#include "NinjamJamRoomController.h"
#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QApplication>
#include <QUuid>
#include "audio/NinjamTrackNode.h"
#include "Utils.h"

using namespace Persistence;
using namespace Midi;
using namespace Ninjam;

//++++++++++++++++++++++++++++++

using namespace Controller;

MainController::MainController(JamtabaFactory* factory, Settings settings, int &argc, char **argv)
    :QApplication(argc, argv),
      audioMixer(nullptr),
      roomStreamer(nullptr),
      currentStreamingRoomID(-1000),
      ninjamService(nullptr),
      ninjamController(nullptr),
      mutex(QMutex::Recursive),
      started(false),
      vstHost(Vst::VstHost::getInstance()),
      //pluginFinder(std::unique_ptr<Vst::PluginFinder>(new Vst::PluginFinder())),
      ipToLocationResolver("../Jamtaba2/GeoLite2-Country.mmdb"),
      settings(settings)
{

    setQuitOnLastWindowClosed(false);//wait disconnect from server to close
    configureStyleSheet();

    Login::LoginService* loginService = factory->createLoginService();
    this->loginService = std::unique_ptr<Login::LoginService>( loginService );
    this->audioDriver = new Audio::PortAudioDriver(
                this, //the AudioDriverListener instance
                settings.getLastInputDevice(), settings.getLastOutputDevice(),
                settings.getFirstGlobalAudioInput(), settings.getLastGlobalAudioInput(),
                settings.getFirstGlobalAudioOutput(), settings.getLastGlobalAudioOutput(),
                settings.getLastSampleRate(), settings.getLastBufferSize()
                );

    QObject::connect(this->audioDriver, SIGNAL(sampleRateChanged(int)), this, SLOT(on_audioDriverSampleRateChanged(int)));
    QObject::connect(this->audioDriver, SIGNAL(stopped()), this, SLOT(on_audioDriverStopped()));

    audioMixer = new Audio::AudioMixer(audioDriver->getSampleRate());
    roomStreamer = new Audio::RoomStreamerNode();

    midiDriver = new PortMidiDriver();
    midiDriver->setInputDeviceIndex(settings.getLastMidiDeviceIndex());

    QObject::connect(loginService, SIGNAL(disconnectedFromServer()), this, SLOT(on_disconnectedFromLoginServer()));


    //this->audioMixer->addNode( this->roomStreamer);

    //tracksNodes.insert(INPUT_TRACK_ID, audioMixer->getLocalInput());

    vstHost->setSampleRate(audioDriver->getSampleRate());
    vstHost->setBlockSize(audioDriver->getBufferSize());

    QObject::connect(&pluginFinder, SIGNAL(vstPluginFounded(QString,QString,QString)), this, SLOT(on_VSTPluginFounded(QString,QString,QString)));

    //ninjam service
    this->ninjamService = Ninjam::Service::getInstance();
    QObject::connect( this->ninjamService, SIGNAL(connectedInServer(Ninjam::Server)), SLOT(on_connectedInNinjamServer(Ninjam::Server)) );
    QObject::connect(this->ninjamService, SIGNAL(disconnectedFromServer(Ninjam::Server)), this, SLOT(on_disconnectedFromNinjamServer(Ninjam::Server)));
    QObject::connect(this->ninjamService, SIGNAL(error(QString)), this, SLOT(on_errorInNinjamServer(QString)));


    this->ninjamController = new Controller::NinjamJamRoomController(this);
    //QObject::connect(this->ninjamController, SIGNAL(startingNewInterval()), this, SLOT(on_ninjamStartingNewInterval()));
    QObject::connect(this->ninjamController,
                     SIGNAL(encodedAudioAvailableToSend(QByteArray,quint8, bool, bool)),
                     this, SLOT(on_ninjamAudioAvailableToSend(QByteArray,quint8,bool,  bool)));

    //test ninjam stream
//    NinjamTrackNode* trackTest = new NinjamTrackNode(2);
//    //QStringList testFiles({":/bateria mono.ogg"});
//    QStringList testFiles({":/loop 192KHz.wav.ogg"});
//    addTrack(2, trackTest);
//    for (int i = 0; i < testFiles.size(); ++i) {
//        QFile file(testFiles.at(i));
//        if(!file.exists()){
//            qCritical() << "File not exists! " << file.errorString();
//        }
//        file.open(QIODevice::ReadOnly);
//        trackTest->addVorbisEncodedInterval(file.readAll());
//    }
//    trackTest->startNewInterval();


    //QString vstDir = "C:/Users/elieser/Desktop/TesteVSTs";
    //QString vstDir = "C:/Program Files (x86)/VSTPlugins/";
    //pluginFinder->addPathToScan(vstDir.toStdString());
    //scanPlugins();

    //qDebug() << "QSetting in " << ConfigStore::getSettingsFilePath();
}

QStringList MainController::getBotNames() const{
    return Ninjam::Service::getBotNamesList();
}

Geo::Location MainController::getGeoLocation(QString ip) {
    return ipToLocationResolver.resolve(ip);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainController::updateInputTracksRange(){

    Audio::ChannelRange globalInputRange = audioDriver->getSelectedInputs();


    for (int trackIndex = 0; trackIndex < inputTracks.size(); ++trackIndex) {
        Audio::LocalInputAudioNode* inputTrack = getInputTrack(trackIndex);
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

        if(!inputTrack->isNoInput()){
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
    }
}
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
    if(inputTrackIndex >= 0 && inputTrackIndex < inputTracks.size()){
        //Audio::LocalInputAudioNode* inputTrack = inputTracks.at(inputTrackIndex);
        inputTracks.removeAt(inputTrackIndex);
        removeTrack(inputTrackIndex);;
    }
}

int MainController::addInputTrackNode(Audio::LocalInputAudioNode *inputTrackNode){
    inputTracks.append(inputTrackNode);
    int inputTrackID = inputTracks.size() -1;
    addTrack(inputTrackID, inputTrackNode);

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
    }
    if(isPlayingInNinjamRoom()){
        ninjamController->recreateEncoders();
    }
}
void MainController::setInputTrackToStereo(int localChannelIndex, int firstInputIndex){
    Audio::LocalInputAudioNode* inputTrack = getInputTrack(localChannelIndex);
    if(inputTrack){
        inputTrack->setAudioInputSelection(firstInputIndex, 2);//stereo
        emit inputSelectionChanged(localChannelIndex);
    }
    if(isPlayingInNinjamRoom()){
        ninjamController->recreateEncoders();
    }
}
void MainController::setInputTrackToMIDI(int localChannelIndex, int midiDevice){
    Audio::LocalInputAudioNode* inputTrack = getInputTrack(localChannelIndex);
    if(inputTrack){
        midiDriver->setInputDeviceIndex(midiDevice);
        inputTrack->setMidiInputSelection(midiDevice);
        emit inputSelectionChanged(localChannelIndex);
    }
}
void MainController::setInputTrackToNoInput(int localChannelIndex){
    Audio::LocalInputAudioNode* inputTrack = getInputTrack(localChannelIndex);
    if(inputTrack){
        inputTrack->setToNoInput();
        emit inputSelectionChanged(localChannelIndex);
        if(isPlayingInNinjamRoom()){//send the finish interval message
            ninjamService->sendAudioIntervalPart(currentGUID, QByteArray(), true);
        }
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
Audio::AudioNode *MainController::getTrackNode(long ID){
    QMutexLocker locker(&mutex);
    if(tracksNodes.contains(ID)){
        return tracksNodes[ID];
    }
    return nullptr;
}

bool MainController::addTrack(long trackID, Audio::AudioNode* trackNode){
    QMutexLocker locker(&mutex);
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
void MainController::storeMetronomeSettings(float gain, float pan){
    settings.setMetronomeSettings(gain, pan);
}

void MainController::addVstScanPath(QString path){
    settings.addVstScanPath(path);
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

void MainController::storeIOSettings(int firstIn, int lastIn, int firstOut, int lastOut, int inputDevice, int outputDevice, int sampleRate, int bufferSize, int midiDevice){
    settings.setAudioSettings(firstIn, lastIn, firstOut, lastOut, inputDevice, outputDevice, sampleRate, bufferSize, midiDevice);
}

//+++++++++++++++++

void MainController::removeTrack(long trackID){
    QMutexLocker locker(&mutex); //remove Track is called from ninjam service thread, and cause a crash if the process callback (audio Thread) is iterating over tracksNodes to render audio
    Audio::AudioNode* trackNode = tracksNodes[trackID];
    if(trackNode){
        audioMixer->removeNode(trackNode);
        tracksNodes.remove(trackID);
        //delete trackNode;
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

void MainController::doAudioProcess(Audio::SamplesBuffer &in, Audio::SamplesBuffer &out){
    QMutexLocker locker(&mutex);
    MidiBuffer midiBuffer = midiDriver->getBuffer();
    vstHost->fillMidiEvents(midiBuffer);//pass midi events to vst host

    audioMixer->process(in, out);
}


void MainController::process(Audio::SamplesBuffer &in, Audio::SamplesBuffer &out){
    QMutexLocker locker(&mutex);
    if(!isPlayingInNinjamRoom()){
        doAudioProcess(in, out);
    }
    else{
        ninjamController->process(in, out);
    }
}

Audio::AudioPeak MainController::getTrackPeak(int trackID){
    QMutexLocker locker(&mutex);
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

//++++++++++ TRACKS ++++++++++++
void MainController::setTrackPan(int trackID, float pan){
    QMutexLocker locker(&mutex);
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        node->setPan(pan);
    }
}

void MainController::setTrackLevel(int trackID, float level){
    QMutexLocker locker(&mutex);
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        //node->setGain( std::pow( level, 4));
        node->setGain(Utils::linearGainToPower(level));
    }
}

void MainController::setTrackMute(int trackID, bool muteStatus){
    QMutexLocker locker(&mutex);
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        node->setMuteStatus(muteStatus);
    }
}

void MainController::setTrackSolo(int trackID, bool soloStatus){
    QMutexLocker locker(&mutex);
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        node->setSoloStatus(soloStatus);
    }
}

bool MainController::trackIsMuted(int trackID) const{
    QMutexLocker locker(&mutex);
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        return node->isMuted();
    }
    return false;
}

bool MainController::trackIsSoloed(int trackID) const{
    QMutexLocker locker(&mutex);
    Audio::AudioNode* node = tracksNodes[trackID];
    if(node){
        return node->isSoloed();
    }
    return false;
}

//+++++++++++++++++++++++++++++++++

QList<Audio::PluginDescriptor> MainController::getPluginsDescriptors(){
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
    plugin->start(audioDriver->getSampleRate(), audioDriver->getBufferSize());
    getInputTrack(inputTrackIndex)->addProcessor(*plugin);
    return plugin;
}

void MainController::removePlugin(int inputTrackIndex, Audio::Plugin *plugin){
    getInputTrack(inputTrackIndex)->removeProcessor(*plugin);
    delete plugin;
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
        if(vstPlugin->load(this->vstHost, descriptor.getPath())){
            return vstPlugin;
        }
        return nullptr;
    }
    return nullptr;
}


void MainController::on_disconnectedFromLoginServer(){
    exit(0);
}

MainController::~MainController(){
    stop();
    delete roomStreamer;

    foreach (Audio::AudioNode* track, tracksNodes) {
        delete track;
    }
    tracksNodes.clear();

    pluginsDescriptors.clear();

    if(this->ninjamController){
        delete ninjamController;
        ninjamController = nullptr;
    }
    QObject::disconnect(this->audioDriver, SIGNAL(sampleRateChanged(int)), this, SLOT(on_audioDriverSampleRateChanged(int)));
}

void MainController::saveLastUserSettings(const Persistence::InputsSettings& inputsSettings){
    settings.save(inputsSettings);
}

void MainController::playRoomStream(Login::RoomInfo roomInfo){
    if(roomInfo.hasStream()){
        roomStreamer->setStreamPath(roomInfo.getStreamUrl());
        currentStreamingRoomID = roomInfo.getID();
    }
}

void MainController::stopRoomStream(){
    roomStreamer->stopCurrentStream();
    currentStreamingRoomID = -1000;
}

bool MainController::isPlayingRoomStream() const{
    return roomStreamer->isStreaming();
}

void MainController::enterInRoom(Login::RoomInfo room){
    if(isPlayingRoomStream()){
        stopRoomStream();
    }

    if(room.getType() == Login::RoomTYPE::NINJAM){
        tryConnectInNinjamServer(room);
    }
}

void MainController::tryConnectInNinjamServer(Login::RoomInfo ninjamRoom){

    QString serverIp = ninjamRoom.getName();
    int serverPort = ninjamRoom.getPort();
    QString userName = "Test user name";
    QStringList channelsNames("channel name");
    QString password = "";

    this->ninjamService->startServerConnection(serverIp, serverPort, userName, channelsNames, password);

}

void MainController::on_audioDriverSampleRateChanged(int newSampleRate){
    Q_UNUSED(newSampleRate);
    audioMixer->setSampleRate(newSampleRate);
}

void MainController::on_audioDriverStopped(){
    if(isPlayingInNinjamRoom()){
        //send the last interval part when audio driver is stopped
        ninjamService->sendAudioIntervalPart(currentGUID, QByteArray(), true);
    }
}

void MainController::start()
{
    if(!started){
        started = true;

        audioDriver->start();
        midiDriver->start();

        NatMap map;
        loginService->connectInServer("elieser teste", 0, "channel", map, 0, "teste env", 44100);
    }
}

void MainController::stop()
{
    if(started){
        {
            //QMutexLocker locker(&mutex);
            this->audioDriver->release();
            this->midiDriver->release();
        }
        //qDebug() << "disconnecting...";
        loginService->disconnect();
        if(ninjamController){
            ninjamController->stop();
        }
        started = false;
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
QByteArray MainController::newGUID(){
    QUuid uuid = QUuid::createUuid();
    return uuid.toRfc4122();
}


void MainController::on_ninjamAudioAvailableToSend(QByteArray encodedAudio, quint8 channelIndex, bool isFirstPart, bool isLastPart){
    Q_UNUSED(channelIndex);
    //audio thread fire this event. This thread (main/gui thread)
    //write the encoded bytes in socket. We can't write in socket from audio thread.
    if(isFirstPart){
        currentGUID = newGUID();
        ninjamService->sendAudioIntervalBegin(currentGUID, (quint8)0);
    }

    static QByteArray dataToSend;
    dataToSend.append(encodedAudio);
    if(dataToSend.size() >= 1024 * 4 || isLastPart ){
        ninjamService->sendAudioIntervalPart(currentGUID, dataToSend, isLastPart);
        dataToSend.clear();
    }
}

void MainController::on_errorInNinjamServer(QString error){
    qWarning() << error;
    if(ninjamController){
        ninjamController->stop();
    }
    emit exitedFromRoom(false);//not a normal disconnection
}

void MainController::on_disconnectedFromNinjamServer(const Server &server){
    Q_UNUSED(server);
    if(ninjamController){
        ninjamController->stop();
    }
    emit exitedFromRoom(true);//normal disconnection
}

void MainController::on_connectedInNinjamServer(Ninjam::Server server){
    //emit event after start controller to create view widgets before start
    emit enteredInRoom(Login::RoomInfo(server.getHostName(), server.getPort(), Login::RoomTYPE::NINJAM, server.getMaxUsers()));
    ninjamController->start(server);
}
