#include "MainController.h"
#include "audio/AudioDriverListener.h"
#include "audio/PortAudioDriver.h"
#include "audio/AudioMixer.h"
#include "network/loginserver/DefaultLoginService.h"
#include "JamtabaFactory.h"
#include <QFile>
#include <QDebug>
#include <QApplication>
#include "network/loginserver/LoginServiceResponse.h"
#include "../model/JamRoom.h"
#include "../model/Peer.h"

LoginServiceListenerImpl::LoginServiceListenerImpl(MainController *app){
    this->application = app;
}

void LoginServiceListenerImpl::connected(LoginServiceResponse response){
    qDebug() << "CONNECTED +++++++++++++++++++++++++++++++++\n";
    qDebug() << "\t total users online: " << response.getTotalOnlineUsers();
    QList<AbstractJamRoom*> rooms = response.getRooms();
    foreach (AbstractJamRoom* room, rooms) {
        qDebug() << room->getName();
        QList<Peer*> peers = room->getPeers();
        foreach (Peer* peer, peers) {
            qDebug() << "\t" << peer->getUserName();
        }
    }

    qDebug() << "+++++++++++++++++++++++++++++++++\n";
}

void LoginServiceListenerImpl::disconnected(){
    application->quit();
}
//++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++
class Listener : public AudioDriverListenerAdapter{

private:
    MainController* mainController;
    AudioMixer* audioMixer;
public:
    Listener(MainController* controller){
        this->mainController = controller;
        this->audioMixer = new AudioMixer();
    }

    ~Listener(){
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

    this->loginServiceListener = new LoginServiceListenerImpl(this);
    this->loginService = factory->createLoginService(loginServiceListener);
    this->audioDriver = new PortAudioDriver();
    audioDriverListener = new Listener(this);
}

MainController::~MainController()
{
    if(this->audioDriver != NULL){
        this->audioDriver->stop();
        delete this->audioDriver;
        this->audioDriver = NULL;
    }
    if(this->audioDriverListener != NULL){
        delete this->audioDriverListener;
        this->audioDriverListener = NULL;
    }

    if(loginServiceListener != NULL){
        delete loginServiceListener;
    }
    if(loginService != NULL){
        delete loginService;
    }
}

void MainController::start()
{
    if(this->audioDriver != NULL){
        audioDriver->addListener(*audioDriverListener);
        audioDriver->start();
    }
    NatMap map;
    loginService->connectInServer("elieser teste", 0, "channel", map, 0, "teste env", 44100);
}

void MainController::stop()
{
    if(this->audioDriver != NULL){
        this->audioDriver->release();
    }
    if(loginService != NULL){
        qDebug() << "disconnecting...";
        loginService->disconnect();
    }
}

AudioDriver *MainController::getAudioDriver() const
{
    return audioDriver;
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
