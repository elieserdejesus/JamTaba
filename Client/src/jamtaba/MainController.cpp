#include "MainController.h"
#include "audio/core/AudioDriver.h"
#include "audio/core/PortAudioDriver.h"
#include "audio/core/AudioMixer.h"
#include "audio/core/AudioNode.h"
#include "audio/RoomStreamerNode.h"
#include "../loginserver/LoginService.h"
#include "../loginserver/JamRoom.h"
#include "../loginserver/natmap.h"

#include "JamtabaFactory.h"
#include "audio/core/plugins.h"
#include "audio/vst/vstplugin.h"
#include "persistence/ConfigStore.h"
//#include "mainframe.h"

#include <QFile>
#include <QDebug>
#include <QApplication>

using namespace Persistence;

namespace Controller {

class AudioListener : public Audio::AudioDriverListenerAdapter{

private:
    MainController* mainController;
public:
    AudioListener(MainController* controller){
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
        qDebug() << "audio driver started";
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

      audioMixer(new Audio::AudioMixer()),
      roomStreamer(new Audio::RoomStreamerNode()),
      currentStreamRoom(nullptr),
      inputPeaks(0,0),
      roomStreamerPeaks(0,0)

{

    setQuitOnLastWindowClosed(false);//wait disconnect from server to close
    configureStyleSheet();

    Login::LoginService* service = factory->createLoginService();
    this->loginService = std::unique_ptr<Login::LoginService>( service );
    this->audioDriver = std::unique_ptr<Audio::AudioDriver>( new Audio::PortAudioDriver(
                ConfigStore::getLastInputDevice(), ConfigStore::getLastOutputDevice(),
                ConfigStore::getFirstAudioInput(), ConfigStore::getLastAudioInput(),
                ConfigStore::getFirstAudioOutput(), ConfigStore::getLastAudioOutput(),
                ConfigStore::getLastSampleRate(), ConfigStore::getLastBufferSize()
                ));
    audioDriverListener = std::unique_ptr<Controller::AudioListener>( new Controller::AudioListener(this));
    QObject::connect(service, SIGNAL(disconnectedFromServer()), this, SLOT(on_disconnectedFromServer()));

    this->audioMixer->addNode( *this->roomStreamer);

    tracksNodes.insert(std::pair<int, Audio::AudioNode*>(1, audioMixer->getLocalInput()));

    //this->addPlugin()
}

void MainController::process(Audio::SamplesBuffer &in, Audio::SamplesBuffer &out){
    audioMixer->process(in, out);
    //output->processReplacing(in, out);
    //out.add(in);

    inputPeaks.left = audioMixer->getLocalInput()->getLastPeakLeft();
    inputPeaks.right = audioMixer->getLocalInput()->getLastPeakRight();

    roomStreamerPeaks.left = roomStreamer->getLastPeak();
    roomStreamerPeaks.right = roomStreamer->getLastPeak();
}

Controller::Peaks MainController::getInputPeaks(){
    return inputPeaks;
}

Peaks MainController::getRoomStreamPeaks(){
    return roomStreamerPeaks;
}

//++++++++++ TRACKS ++++++++++++
void MainController::setTrackPan(int trackID, float pan){
    auto it = tracksNodes.find(trackID);
    if(it != tracksNodes.end()){
        (*it).second->setPan(pan);
    }
}

void MainController::setTrackLevel(int trackID, float level){
    auto it = tracksNodes.find(trackID);
    if(it != tracksNodes.end()){
        (*it).second->setGain( std::pow( level, 4));
    }
}

void MainController::setTrackMute(int trackID, bool muteStatus){
    auto it = tracksNodes.find(trackID);
    if(it != tracksNodes.end()){
        (*it).second->setMuteStatus(muteStatus);
    }
}

bool MainController::trackIsMuted(int trackID) const{
    auto it = tracksNodes.find(trackID);
    if(it != tracksNodes.end()){
        return (*it).second->isMuted();
    }
    return false;
}

//+++++++++++++++++++++++++++++++++

std::vector<Audio::PluginDescriptor*> MainController::getPluginsDescriptors(){
    return Audio::getPluginsDescriptors();
}

Audio::Plugin* MainController::addPlugin(Audio::PluginDescriptor *descriptor){
    Audio::Plugin* plugin = createPluginInstance(descriptor);
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
        return Vst::load(descriptor->getPath());
    }
    return nullptr;
}

bool MainController::isPlayingRoomStream(){
    return currentStreamRoom != nullptr;
}

Login::AbstractJamRoom* MainController::getCurrentStreamingRoom(){
    return currentStreamRoom;
}

void MainController::on_disconnectedFromServer(){
    exit(0);
}

MainController::~MainController()
{
    this->audioDriver->stop();
}

void MainController::playRoomStream(Login::AbstractJamRoom* room){
    if(room->hasStreamLink()){
        roomStreamer->setStreamPath(room->getStreamLink());
        currentStreamRoom = room;
    }
}

void MainController::stopRoomStream(){
    roomStreamer->stopCurrentStream();
    currentStreamRoom = nullptr;
}

void MainController::start()
{
    audioDriver->addListener(*audioDriverListener);
    audioDriver->start();

    NatMap map;
    loginService->connectInServer("elieser teste", 0, "channel", map, 0, "teste env", 44100);
}

void MainController::stop()
{
    this->audioDriver->release();
    qDebug() << "disconnecting...";
    loginService->disconnect();
}

Audio::AudioDriver *MainController::getAudioDriver() const
{
    return audioDriver.get();
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
