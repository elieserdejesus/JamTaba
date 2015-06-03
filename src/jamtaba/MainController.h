#pragma once

#include <QApplication>
#include <QDebug>
#include <memory>
#include <QList>
#include "audio/core/plugins.h"

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

struct Peaks{
    float left;
    float right;

    Peaks(float l, float r){
        left    = l;
        right   = r;
    }

    float max(){
        return std::max(std::abs(left), std::abs(right));
    }
};

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



    Peaks getInputPeaks();
    Peaks getRoomStreamPeaks();

    std::vector<Audio::PluginDescriptor*> getPluginsDescriptors();

    Audio::Plugin* addPlugin(Audio::PluginDescriptor* descriptor);
    void removePlugin(Audio::Plugin* plugin);

    //tracks
    void setTrackMute(int trackID, bool muteStatus);
    bool trackIsMuted(int trackID) const;
    void setTrackSolo(int trackID, bool soloStatus);
    void setTrackLevel(int trackID, float level);
    void setTrackPan(int trackID, float pan);

    const Vst::PluginFinder* getPluginFinder() const{return &*pluginFinder;}
    void scanPlugins();
    void initializePluginsList(QStringList paths);

signals:
    void enteredInRoom(Login::AbstractJamRoom* room);
    void audioSamplesProcessed(int samplesProcessed);
private:

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

    QMap<long, Audio::AudioNode*> tracksNodes;

    Peaks inputPeaks;
    Peaks roomStreamerPeaks;
    //+++++++++++++++++++
    Vst::VstHost* vstHost;
    std::vector<Audio::PluginDescriptor*> pluginsDescriptors;
    std::unique_ptr<Vst::PluginFinder> pluginFinder;
    //+++++++++++++++++++++++++
    void configureStyleSheet();

    void tryConnectInNinjamServer(const Login::NinjamRoom & ninjamRoom);

private slots:
    void on_disconnectedFromServer();
    //void onPluginScanStarted();
    //void onPluginScanFinished();
    void onPluginFounded(Audio::PluginDescriptor* descriptor);

    //ninjam
    void connectedInNinjamServer(const Ninjam::Server& server);
};

}
