#pragma once

#include <QApplication>
#include <QDebug>
#include <memory>

class MainFrame;

namespace Audio {
    class AudioDriver;
    class AudioDriverListener;
}

namespace Login {
    class LoginService;
    class LoginServiceParser;
    class LoginServiceListener;
}

namespace Model {
    class AbstractJamRoom;
}

class JamtabaFactory;

//+++++++++++++

namespace Controller {

//++++++++++++++++++++++++++++
class MainController : public QApplication
{
    Q_OBJECT

public:
    MainController(JamtabaFactory *factory, int& argc, char** argv);
    ~MainController();

    void start();
    void stop();
    Audio::AudioDriver* getAudioDriver() const;
    Login::LoginService* getLoginService() const;

signals:
    //void on_freshDataAvailableFromServer(const Login::LoginServiceParser& response);


private:
    std::unique_ptr< Audio::AudioDriver> audioDriver;
    std::unique_ptr<Audio::AudioDriverListener> audioDriverListener;
    std::unique_ptr<Login::LoginService> loginService;
    //+++++++++++++++++++++++++
    void configureStyleSheet();

private slots:
    void on_disconnectedFromServer();

};

}
