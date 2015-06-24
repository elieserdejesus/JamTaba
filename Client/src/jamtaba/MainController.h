#pragma once

#include <QApplication>
#include <memory>
#include <QMutex>
#include "audio/core/AudioPeak.h"

class MainFrame;

namespace Ninjam{
    class Service;
    class Server;
}

namespace Audio {
    class AudioDriver;
    class AudioDriverListener;
    class AbstractMp3Streamer;
    class Plugin;
    class AudioMixer;
    class SamplesBuffer;
    class AudioNode;
    class PluginDescriptor;
}

namespace Midi{
    class MidiDriver;
    class MidiBuffer;
}

namespace Login {
    class LoginService;
    class LoginServiceParser;
    class LoginServiceListener;
    class AbstractJamRoom;
    class NinjamRoom;
    class AbstractPeer;
}

namespace Vst {
    class VstHost;
    class PluginFinder;
}

class JamtabaFactory;

//+++++++++++++

namespace Controller {

class AudioListener;
class NinjamJamRoomController;



//++++++++++++++++++++++++++++
class MainController : public QApplication
{
    Q_OBJECT

    friend class Controller::AudioListener;
    friend class Controller::NinjamJamRoomController;

public:
    MainController(JamtabaFactory *factory, int& argc, char** argv);
    ~MainController();

    //virtual void timerEvent(QTimerEvent *);

    void start();
    void stop();

    void process(Audio::SamplesBuffer& in, Audio::SamplesBuffer& out);

    void addTrack(long trackID, Audio::AudioNode* trackNode);
    void removeTrack(long trackID);

    void playRoomStream(Login::AbstractJamRoom *room);
    bool isPlayingRoomStream();
    Login::AbstractJamRoom* getCurrentStreamingRoom();
    void stopRoomStream();//stop currentRoom stream

    void enterInRoom(Login::AbstractJamRoom* room);

    Audio::AudioDriver* getAudioDriver() const;
    Midi::MidiDriver* getMidiDriver() const;
    Login::LoginService* getLoginService() const;


    inline Controller::NinjamJamRoomController* getNinjamController() const{return ninjamController;}

    std::vector<Audio::PluginDescriptor*> getPluginsDescriptors();

    Audio::Plugin* addPlugin(Audio::PluginDescriptor* descriptor);
    void removePlugin(Audio::Plugin* plugin);

    //tracks
    void setTrackMute(int trackID, bool muteStatus);
    bool trackIsMuted(int trackID) const;
    void setTrackSolo(int trackID, bool soloStatus);
    bool trackIsSoloed(int trackID) const;
    void setTrackLevel(int trackID, float level);
    void setTrackPan(int trackID, float pan);
    Audio::AudioPeak getInputPeaks();
    Audio::AudioPeak getRoomStreamPeak();
    Audio::AudioPeak getTrackPeak(int trackID);

    const Vst::PluginFinder* getPluginFinder() const{return &*pluginFinder;}
    void scanPlugins();
    void initializePluginsList(QStringList paths);

    Audio::AudioNode* getTrackNode(long ID);

    inline bool isStarted() const{return started;}
signals:
    void enteredInRoom(Login::AbstractJamRoom* room);
    void exitedFromRoom(bool error);
    //void

private:
    static const int INPUT_TRACK_ID = 1;
    void doAudioProcess(Audio::SamplesBuffer& in, Audio::SamplesBuffer& out);
    Audio::Plugin* createPluginInstance(Audio::PluginDescriptor* descriptor);

    std::unique_ptr<Audio::AudioDriver> audioDriver;
    Midi::MidiDriver* midiDriver;//TODO use unique_ptr

    std::unique_ptr<Audio::AudioDriverListener> audioDriverListener;
    std::unique_ptr<Login::LoginService> loginService;

    std::unique_ptr<Audio::AudioMixer> audioMixer;

    std::unique_ptr<Audio::AbstractMp3Streamer> roomStreamer;
    Login::AbstractJamRoom* currentStreamRoom;

    //ninjam
    Ninjam::Service* ninjamService;
    Controller::NinjamJamRoomController* ninjamController;


    QMap<long, Audio::AudioNode*> tracksNodes;

    mutable QMutex mutex;

    bool started;

    Audio::AudioPeak inputPeaks;
    Audio::AudioPeak roomStreamerPeaks;
    //+++++++++++++++++++
    Vst::VstHost* vstHost;
    std::vector<Audio::PluginDescriptor*> pluginsDescriptors;
    std::unique_ptr<Vst::PluginFinder> pluginFinder;
    //+++++++++++++++++++++++++
    void configureStyleSheet();

    void tryConnectInNinjamServer(const Login::NinjamRoom & ninjamRoom);

private slots:
    void on_disconnectedFromLoginServer();

    void onPluginFounded(Audio::PluginDescriptor* descriptor);

    //ninjam
    void connectedInNinjamServer(const Ninjam::Server& server);
    void disconnectedFromNinjamServer(const Ninjam::Server& server);
    void errorInNinjamServer(QString error);
};

}
