#include "MainController.h"
#include "audio/core/PortAudioDriver.h"
#include "audio/core/AudioMixer.h"
#include "audio/RoomStreamerNode.h"
#include "../loginserver/LoginService.h"
#include "../loginserver/JamRoom.h"
#include "../loginserver/natmap.h"
#include "JamtabaFactory.h"
#include "persistence/ConfigStore.h"
#include "mainframe.h"

#include <QFile>
#include <QDebug>
#include <QApplication>
#include <QThread>

using namespace Login;
using namespace Audio;
using namespace Persistence;
using namespace Controller;

namespace Controller {

class AudioListener : public AudioDriverListenerAdapter{

private:
    MainController* mainController;
    AudioMixer* audioMixer;
    AbstractMp3Streamer* streamer;
public:
    AudioListener(MainController* controller){
        this->mainController = controller;
        this->audioMixer = new AudioMixer();
        this->streamer = new RoomStreamerNode(QUrl("http://vprjazz.streamguys.net/vprjazz64.mp3"));
        //this->streamer = new RoomStreamerNode(QUrl("http://users.skynet.be/fa046054/home/P22/track56.mp3"));
        //this->streamer = new RoomStreamerNode(QUrl("http://ninbot.com:8000/2051"));
        //this->streamer = new AudioFileStreamerNode("D:/Documents/Estudos/ComputacaoMusical/Jamtaba2/teste.mp3");
        this->audioMixer->addNode(*this->streamer);
    }

    ~AudioListener(){
        delete audioMixer;
        delete streamer;
    }

    virtual void driverStarted(){
        qDebug() << "audio driver started";
    }

    virtual void driverStopped(){
        qDebug() << "audio driver stopped";
    }

    virtual void processCallBack(AudioSamplesBuffer& in, AudioSamplesBuffer& out){
        audioMixer->process(in, out);
        //output->processReplacing(in, out);
        //out.add(in);
    }

    virtual void driverException(const char* msg){
        qDebug() << msg;
    }
};

}
//++++++++++++++++++++++++++++++

MainController::MainController(JamtabaFactory* factory, int &argc, char **argv)
    :QApplication(argc, argv)
{

    setQuitOnLastWindowClosed(false);//wait disconnect from server to close
    configureStyleSheet();

    Login::LoginService* service = factory->createLoginService();
    this->loginService = std::unique_ptr<LoginService>( service );
    this->audioDriver = std::unique_ptr<AudioDriver>( new PortAudioDriver(
                ConfigStore::getLastInputDevice(), ConfigStore::getLastOutputDevice(),
                ConfigStore::getFirstAudioInput(), ConfigStore::getLastAudioInput(),
                ConfigStore::getFirstAudioOutput(), ConfigStore::getLastAudioOutput(),
                ConfigStore::getLastSampleRate(), ConfigStore::getLastBufferSize()
                ));
    audioDriverListener = std::unique_ptr<AudioListener>( new AudioListener(this));
    QObject::connect(service, SIGNAL(disconnectedFromServer()), this, SLOT(on_disconnectedFromServer()));
}

void MainController::on_disconnectedFromServer(){
    exit(0);
}

MainController::~MainController()
{
    this->audioDriver->stop();
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
