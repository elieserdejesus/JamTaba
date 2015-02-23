#include "MainController.h"
#include "audio/core/AudioDriver.h"
#include "audio/core/PortAudioDriver.h"
#include "audio/core/AudioMixer.h"
#include "audio/RoomStreamerNode.h"
#include "../loginserver/LoginService.h"
#include "../loginserver/JamRoom.h"
#include "../loginserver/natmap.h"
#include "JamtabaFactory.h"
#include "audio/core/plugins.h"
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
    Audio::AudioMixer* audioMixer;
    Audio::AbstractMp3Streamer* streamer;
public:
    AudioListener(MainController* controller){
        this->mainController = controller;
        this->audioMixer = new Audio::AudioMixer();

        //this->streamer = new RoomStreamerNode(QUrl("http://vprjazz.streamguys.net/vprjazz64.mp3"));
        //this->streamer = new RoomStreamerNode(QUrl("http://users.skynet.be/fa046054/home/P22/track56.mp3"));
        //this->streamer = new RoomStreamerNode(QUrl("http://ninbot.com:8000/2050"));
        //this->streamer = new Audio::AudioFileStreamerNode("D:/Documents/Estudos/ComputacaoMusical/Jamtaba2/teste.mp3");
        this->streamer = &*mainController->roomStreamer;
        this->audioMixer->addNode( *this->streamer);
    }

    ~AudioListener(){
        delete audioMixer;
    }

    virtual void driverStarted(){
        qDebug() << "audio driver started";
    }

    virtual void driverStopped(){
        qDebug() << "audio driver stopped";
    }

    virtual void processCallBack(Audio::SamplesBuffer& in, Audio::SamplesBuffer& out){
        audioMixer->process(in, out);
        //output->processReplacing(in, out);
        //out.add(in);

        mainController->peaks.lastStreamRoomPeak = streamer->getLastPeak();

    }

    virtual void driverException(const char* msg){
        qDebug() << msg;
    }
};

}//namespace Controller::
//++++++++++++++++++++++++++++++

MainController::MainController(JamtabaFactory* factory, int &argc, char **argv)
    :QApplication(argc, argv),
      roomStreamer(new Audio::RoomStreamerNode()),
      currentStreamRoom(nullptr)
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
}

std::vector<Plugin::PluginDescriptor*> MainController::getPluginsDescriptors(){
    return Plugin::getDescriptors();
}

Audio::Plugin* MainController::addPlugin(Plugin::PluginDescriptor *descriptor){
    Audio::Plugin* plugin = createPluginInstance(descriptor);
    //preciso acessar o mixer para conectar o plugin na input track,
    //sendo assim o mixer precisa ser um atributo do mainController
    return plugin;
}

Audio::Plugin *MainController::createPluginInstance(Plugin::PluginDescriptor *descriptor)
{
    if(descriptor->getGroup() == "Jamtaba"){
        if(descriptor->getName() == "Delay"){
            return new Plugin::JamtabaDelay();
        }
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
