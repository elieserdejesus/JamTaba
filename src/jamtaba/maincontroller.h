#pragma once

#include <QApplication>
#include "network/loginserver/LoginService.h"
#include <QDebug>

class AudioDriver;
class AudioDriverListener;
class LoginService;
class MainController;
class JamtabaFactory;
class LoginServiceResponse;

//+++++++++++++
class LoginServiceListenerImpl : public LoginServiceListener{
public:
    LoginServiceListenerImpl(MainController* app);
    ~LoginServiceListenerImpl(){qDebug() << "LoginServiceListenerImpl::destructor";}
    virtual void connected(LoginServiceResponse response);
    virtual void disconnected();
private:
    MainController* application;
};


//++++++++++++++++++++++++++++
class MainController : public QApplication
{
public:
    MainController(JamtabaFactory *factory, int& argc, char** argv);
    ~MainController();

    void start();
    void stop();
    AudioDriver* getAudioDriver() const;


private:
    AudioDriver* audioDriver;
    AudioDriverListener* audioDriverListener;
    LoginService* loginService;
    LoginServiceListener* loginServiceListener;
    //+++++++++++++++++++++++++
    void configureStyleSheet();
};

