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
#include "persistence/ConfigStore.h"
#include "../audio/vst/PluginFinder.h"

#include "../ninjam/Service.h"
#include "../ninjam/Server.h"
#include "NinjamJamRoomController.h"
#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QApplication>
//#include "audio/NinjamTrackNode.h"

using namespace Persistence;
using namespace Midi;
using namespace Ninjam;

namespace Controller {

class AudioListener : public Audio::AudioDriverListenerAdapter{

private:
    MainController* mainController;
public:
    explicit AudioListener(MainController* controller){
        this->mainController = controller;
        //this->streamer = new RoomStreamerNode(QUrl("http://vprjazz.streamguys.net/vprjazz64.mp3"));
        //this->streamer = new RoomStreamerNode(QUrl("http://users.skynet.be/fa046054/home/P22/track56.mp3"));
        //this->streamer = new RoomStreamerNode(QUrl("http://ninbot.com:8000/2050"));
        //this->streamer = new Audio::AudioFileStreamerNode("D:/Documents/Estudos/ComputacaoMusical/Jamtaba2/teste.mp3");
    }

    ~AudioListener(){
        //delete audioMixer;
    }

    virtual void driverStarted(){
        //qDebug() << "audio driver started";
    }

    virtual void driverStopped(){
        qDebug() << "audio driver stopped";
    }

    virtual void processCallBack(Audio::SamplesBuffer& in, Audio::SamplesBuffer& out){
        mainController->process(in, out);
    }

    virtual void driverException(const char* msg){
        qDebug() << msg;
    }
};

}//namespace Controller::
//++++++++++++++++++++++++++++++

using namespace Controller;

MainController::MainController(JamtabaFactory* factory, int &argc, char **argv)
    :QApplication(argc, argv),
      audioMixer(nullptr),
      roomStreamer(nullptr),
      currentStreamingRoomID(-1000),
      mutex(QMutex::Recursive),
      started(false),
      vstHost(Vst::VstHost::getInstance()),
      pluginFinder(std::unique_ptr<Vst::PluginFinder>(new Vst::PluginFinder())),
      ipToLocationResolver("../Jamtaba2/GeoLite2-Country.mmdb")
{

    setQuitOnLastWindowClosed(false);//wait disconnect from server to close
    configureStyleSheet();

    Login::LoginService* loginService = factory->createLoginService();
    this->loginService = std::unique_ptr<Login::LoginService>( loginService );
    this->audioDriver = std::unique_ptr<Audio::AudioDriver>( new Audio::PortAudioDriver(
                ConfigStore::getLastInputDevice(), ConfigStore::getLastOutputDevice(),
                ConfigStore::getFirstAudioInput(), ConfigStore::getLastAudioInput(),
                ConfigStore::getFirstAudioOutput(), ConfigStore::getLastAudioOutput(),
                ConfigStore::getLastSampleRate(), ConfigStore::getLastBufferSize()
                ));
    audioDriverListener = std::unique_ptr<Controller::AudioListener>( new Controller::AudioListener(this));

    audioMixer = new Audio::AudioMixer(audioDriver->getSampleRate());
    roomStreamer = new Audio::RoomStreamerNode();

    midiDriver = new PortMidiDriver();
    midiDriver->setInputDeviceIndex(Persistence::ConfigStore::getLastMidiDeviceIndex());

    QObject::connect(loginService, SIGNAL(disconnectedFromServer()), this, SLOT(on_disconnectedFromLoginServer()));


    //this->audioMixer->addNode( this->roomStreamer);

    tracksNodes.insert(INPUT_TRACK_ID, audioMixer->getLocalInput());

    vstHost->setSampleRate(audioDriver->getSampleRate());
    vstHost->setBlockSize(audioDriver->getBufferSize());

    QObject::connect(&*pluginFinder, SIGNAL(vstPluginFounded(Audio::PluginDescriptor*)), this, SLOT(onPluginFounded(Audio::PluginDescriptor*)));

    //ninjam service
    this->ninjamService = Ninjam::Service::getInstance();
    QObject::connect( this->ninjamService, SIGNAL(connectedInServer(Ninjam::Server)), SLOT(connectedInNinjamServer(Ninjam::Server)) );
    QObject::connect(this->ninjamService, SIGNAL(disconnectedFromServer(Ninjam::Server)), this, SLOT(disconnectedFromNinjamServer(Ninjam::Server)));
    QObject::connect(this->ninjamService, SIGNAL(error(QString)), this, SLOT(errorInNinjamServer(QString)));

    this->ninjamController = new Controller::NinjamJamRoomController(this);


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

Geo::Location MainController::getLocation(QString ip) {
    return ipToLocationResolver.resolve(ip);
}

Audio::AudioNode *MainController::getTrackNode(long ID){
    QMutexLocker locker(&mutex);
    if(tracksNodes.contains(ID)){
        return tracksNodes[ID];
    }
    return nullptr;
}

void MainController::addTrack(long trackID, Audio::AudioNode* trackNode){
    QMutexLocker locker(&mutex);
    tracksNodes.insert( trackID, trackNode );
    audioMixer->addNode(trackNode) ;
}

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
            pluginsDescriptors.push_back(new Audio::PluginDescriptor(pluginName, "VST", path));
        }
    }
}

void MainController::scanPlugins(){
    foreach (Audio::PluginDescriptor* descriptor, pluginsDescriptors) {
        delete descriptor;
    }
    pluginsDescriptors.clear();
    //ConfigStore::clearVstCache();
    pluginFinder->clearScanPaths();
    QStringList scanPaths = Persistence::ConfigStore::getVstScanPaths();
    foreach (QString path, scanPaths) {
        pluginFinder->addPathToScan(path);
    }
    pluginFinder->scan(vstHost);
}

void MainController::onPluginFounded(Audio::PluginDescriptor* descriptor){
    pluginsDescriptors.push_back(descriptor);
    ConfigStore::addVstPlugin(descriptor->getPath());
}

void MainController::doAudioProcess(Audio::SamplesBuffer &in, Audio::SamplesBuffer &out){
    QMutexLocker locker(&mutex);
    MidiBuffer midiBuffer = midiDriver->getBuffer();
    vstHost->fillMidiEvents(midiBuffer);//pass midi events to vst host

    audioMixer->process(in, out);
}


void MainController::process(Audio::SamplesBuffer &in, Audio::SamplesBuffer &out){
    QMutexLocker locker(&mutex);
    if(!ninjamController->isRunning()){
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
    return Audio::AudioPeak();
}

Audio::AudioPeak MainController::getInputPeaks(){
    return getTrackPeak(INPUT_TRACK_ID);
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
        node->setGain( std::pow( level, 4));
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

std::vector<Audio::PluginDescriptor*> MainController::getPluginsDescriptors(){
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

Audio::Plugin* MainController::addPlugin(Audio::PluginDescriptor *descriptor){
    Audio::Plugin* plugin = createPluginInstance(descriptor);
    plugin->start(audioDriver->getSampleRate(), audioDriver->getBufferSize());
    this->audioMixer->getLocalInput()->addProcessor(*plugin);

    return plugin;
}

void MainController::removePlugin(Audio::Plugin *plugin){
    this->audioMixer->getLocalInput()->removeProcessor(*plugin);
}

Audio::Plugin *MainController::createPluginInstance(Audio::PluginDescriptor *descriptor)
{
    if(descriptor->getGroup() == "Jamtaba"){
        if(descriptor->getName() == "Delay"){
            return new Audio::JamtabaDelay(audioDriver->getSampleRate());
        }
    }
    else if(descriptor->getGroup() == "VST"){
        Vst::VstPlugin* vst = new Vst::VstPlugin(this->vstHost);
        if(vst->load(this->vstHost, descriptor->getPath())){
            return vst;
        }
        return nullptr;
    }
    return nullptr;
}


void MainController::on_disconnectedFromLoginServer(){
    exit(0);
}

MainController::~MainController()
{
    stop();
    delete roomStreamer;
    foreach (Audio::PluginDescriptor* descriptor, pluginsDescriptors) {
        delete descriptor;
    }
    pluginsDescriptors.clear();

    if(this->ninjamController){
        delete ninjamController;
        ninjamController = nullptr;
    }

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

void MainController::start()
{
    if(!started){
        started = true;
        audioDriver->addListener(*audioDriverListener);
        audioDriver->start();
        midiDriver->start();

        NatMap map;
        loginService->connectInServer("elieser teste", 0, "channel", map, 0, "teste env", 44100);
    }
}

void MainController::stop()
{
    if(started){
        QMutexLocker locker(&mutex);
        this->audioDriver->release();
        this->midiDriver->release();
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
    return audioDriver.get();
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


//++++++++++++= NINJAM ++++++++++++++++
void MainController::errorInNinjamServer(QString error){
    qWarning() << error;
    ninjamController->stop();
    emit exitedFromRoom(false);//not a normal disconnection
}

void MainController::disconnectedFromNinjamServer(const Server &server){
    Q_UNUSED(server);
    ninjamController->stop();
    emit exitedFromRoom(true);//normal disconnection
}

void MainController::connectedInNinjamServer(Ninjam::Server server){

    emit enteredInRoom(Login::RoomInfo(server.getHostName(), server.getPort(), Login::RoomTYPE::NINJAM, server.getMaxUsers()));
    ninjamController->start(server);

}
