#pragma once

#include <QApplication>
#include <QDebug>
#include <memory>
#include <QList>
#include "audio/core/plugins.h"

class MainFrame;

namespace Audio {
    class AudioDriver;
    class AudioDriverListener;
    class AbstractMp3Streamer;
    class Plugin;
}

namespace Login {
    class LoginService;
    class LoginServiceParser;
    class LoginServiceListener;
    class AbstractJamRoom;
}


class JamtabaFactory;

//+++++++++++++

namespace Controller {

class AudioListener;
}

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
    void playRoomStream(Login::AbstractJamRoom *room);
    bool isPlayingRoomStream();
    Login::AbstractJamRoom* getCurrentStreamingRoom();
    void stopRoomStream();//stop currentRoom stream

    Audio::AudioDriver* getAudioDriver() const;
    Login::LoginService* getLoginService() const;

    struct Peaks{
        float lastStreamRoomPeak;
    };

    Peaks getPeaks() const{return peaks;}

    std::vector<Plugin::PluginDescriptor*> getPluginsDescriptors();

    Audio::Plugin* addPlugin(Plugin::PluginDescriptor* descriptor);

private:
    Audio::Plugin* createPluginInstance(Plugin::PluginDescriptor* descriptor);

    std::unique_ptr<Audio::AudioDriver> audioDriver;
    std::unique_ptr<Audio::AudioDriverListener> audioDriverListener;
    std::unique_ptr<Login::LoginService> loginService;


    std::unique_ptr<Audio::AbstractMp3Streamer> roomStreamer;
    Login::AbstractJamRoom* currentStreamRoom;
    //+++++++++++++++++++
    Peaks peaks;
    //+++++++++++++++++++++++++
    void configureStyleSheet();

private slots:
    void on_disconnectedFromServer();

};

//}
