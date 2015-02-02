#pragma once

#include <QApplication>
#include <QDebug>
#include <memory>

namespace Audio {
    class AudioDriver;
    class AudioDriverListener;
}

namespace Login {
    class LoginService;
    class LoginServiceResponse;
    class LoginServiceListener;
}

class JamtabaFactory;

//+++++++++++++

namespace Controller {

//++++++++++++++++++++++++++++
class MainController : public QApplication
{
public:
    MainController(JamtabaFactory *factory, int& argc, char** argv);
    ~MainController();

    void start();
    void stop();
    Audio::AudioDriver* getAudioDriver() const;


private:
    std::unique_ptr< Audio::AudioDriver> audioDriver;
    std::unique_ptr<Audio::AudioDriverListener> audioDriverListener;
    std::unique_ptr<Login::LoginService> loginService;
    std::unique_ptr<Login::LoginServiceListener> loginServiceListener;
    //+++++++++++++++++++++++++
    void configureStyleSheet();


};

}
