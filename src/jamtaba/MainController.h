#pragma once

#include <QApplication>
#include <QDebug>
#include <memory>

class MainFrame;

namespace Audio {
    class AudioDriver;
    class AudioDriverListener;
    class AbstractMp3Streamer;
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

class AudioListener;

//++++++++++++++++++++++++++++
class MainController : public QApplication
{
    Q_OBJECT

    friend class Controller::AudioListener;

public:
    MainController(JamtabaFactory *factory, int& argc, char** argv);
    ~MainController();

    void start();
    void stop();
    void playRoomStream(QString roomStreamURL);
    void stopRoomStream();//stop currentRoom stream

    Audio::AudioDriver* getAudioDriver() const;
    Login::LoginService* getLoginService() const;


signals:
    //void on_freshDataAvailableFromServer(const Login::LoginServiceParser& response);


private:
    std::unique_ptr< Audio::AudioDriver> audioDriver;
    std::unique_ptr<Audio::AudioDriverListener> audioDriverListener;
    std::unique_ptr<Login::LoginService> loginService;

    std::unique_ptr<Audio::AbstractMp3Streamer> roomStreamer;

    //+++++++++++++++++++++++++
    void configureStyleSheet();


private slots:
    void on_disconnectedFromServer();

};

}
