#include "MainController.h"
#include "audio/PortAudioDriver.h"
#include "audio/AudioMixer.h"
#include "network/loginserver/DefaultLoginService.h"
#include "JamtabaFactory.h"
#include "ConfigStore.h"
#include <QFile>
#include <QDebug>
#include <QApplication>
#include "network/loginserver/LoginServiceResponse.h"
#include "../model/JamRoom.h"
#include "../model/Peer.h"

using namespace Login;
using namespace Model;
using namespace Audio;
using namespace Persistence;
using namespace Controller;

namespace Controller {

//declarei esta classe aqui no cpp porque não foi possível usar ela como uma classe interna de MainController.
//Para usar como classe interna seria necessário incluir o header onde esta o LoginServiceListener no MainController.h,
//o que não seria legal. Não é possível herdar usando forwarding declaration. Acabei adotando essa solução para evitar
//a inclusão do header.
class LoginServiceListenerImpl : public Login::LoginServiceListener{
public:
    LoginServiceListenerImpl(MainController* controller):
        mainController(controller){

    }

    ~LoginServiceListenerImpl(){qDebug() << "LoginServiceListenerImpl::destructor";}

    virtual void connected(Login::LoginServiceResponse response){
        qDebug() << "CONNECTED +++++++++++++++++++++++++++++++++\n";
        qDebug() << "\t total users online: " << response.getTotalOnlineUsers();

        QList<Model::RealTimeRoom*> rooms = response.getRealtimeRooms();
        foreach (Model::RealTimeRoom* room, rooms) {
            qDebug() << room->getName();
            QList<Model::Peer*> peers = room->getPeers();
            foreach (Model::Peer* peer, peers) {
                qDebug() << "\t" << peer->getUserName();
            }
        }
        qDebug() << "+++++++++++++++++++++++++++++++++\n";
        QList<Model::NinjamRoom*> ninjamRooms = response.getNinjamRooms();
        foreach (Model::NinjamRoom* room, ninjamRooms) {
            qDebug() << room->getName();

        }
    }
    virtual void disconnected(){
        mainController->quit();
    }

private:
    MainController* mainController;
};

}

//++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++
class AudioListener : public AudioDriverListenerAdapter{

private:
    MainController* mainController;
    AudioMixer* audioMixer;
public:
    AudioListener(MainController* controller){
        this->mainController = controller;
        this->audioMixer = new AudioMixer();
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

    virtual void processCallBack(AudioSamplesBuffer& in, AudioSamplesBuffer& out){
        audioMixer->process(in, out);
        //output->processReplacing(in, out);

        //out.add(in);

    }

    virtual void driverException(const char* msg){
        qDebug() << msg;
    }


};
//++++++++++++++++++++++++++++++

MainController::MainController(JamtabaFactory* factory, int &argc, char **argv)
    :QApplication(argc, argv)
{
    setQuitOnLastWindowClosed(false);
    configureStyleSheet();

    LoginServiceListener* listener = new LoginServiceListenerImpl(this);
    this->loginServiceListener = std::unique_ptr<LoginServiceListener>( listener );
    this->loginService = std::unique_ptr<LoginService>( factory->createLoginService(listener));
    this->audioDriver = std::unique_ptr<AudioDriver>( new PortAudioDriver(
                ConfigStore::getLastInputDevice(), ConfigStore::getLastOutputDevice(),
                ConfigStore::getFirstAudioInput(), ConfigStore::getLastAudioInput(),
                ConfigStore::getFirstAudioOutput(), ConfigStore::getLastAudioOutput(),
                ConfigStore::getLastSampleRate(), ConfigStore::getLastBufferSize()
                ));
    audioDriverListener = std::unique_ptr<AudioListener>( new AudioListener(this));
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
