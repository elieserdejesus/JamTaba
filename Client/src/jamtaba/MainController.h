#pragma once

#include <QApplication>
#include <memory>
#include <QMutex>
#include "audio/core/AudioPeak.h"

#include "geo/IpToLocationResolver.h"
#include "../ninjam/Server.h"
#include "../loginserver/LoginService.h"
#include "../audio/core/AudioDriver.h"

class MainFrame;

namespace Ninjam{
    class Service;
    //class Server;
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
class MainController : public QApplication, public Audio::AudioDriverListener
{
    Q_OBJECT

    friend class Controller::AudioListener;
    friend class Controller::NinjamJamRoomController;

public:
    MainController(JamtabaFactory *factory, int& argc, char** argv);
    ~MainController();

    void start();
    void stop();

    virtual void process(Audio::SamplesBuffer& in, Audio::SamplesBuffer& out);

    bool addTrack(long trackID, Audio::AudioNode* trackNode);
    void removeTrack(long trackID);

    void playRoomStream(Login::RoomInfo roomInfo);
    bool isPlayingRoomStream() const;

    bool isPlayingInNinjamRoom() const;

    void stopRoomStream();//stop currentRoom stream
    inline long long getCurrentStreamingRoomID() const{return currentStreamingRoomID;}

    void enterInRoom(Login::RoomInfo room);

    Audio::AudioDriver* getAudioDriver() const;
    Midi::MidiDriver* getMidiDriver() const;
    Login::LoginService* getLoginService() const;


    inline Controller::NinjamJamRoomController* getNinjamController() const{return ninjamController;}

    std::vector<Audio::PluginDescriptor*> getPluginsDescriptors();

    Audio::Plugin* addPlugin(Audio::PluginDescriptor* descriptor);
    void removePlugin(Audio::Plugin* plugin);

    QStringList getBotNames() const;

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

    void updateInputTrackRange();//called when input range or method (audio or midi) are changed in preferences
    void setInputTrackToMono(int inputIndex);
    void setInputTrackToStereo(int firstInputIndex);
    void setInputTrackToMIDI(int midiDevice);
    void setInputTrackToNoInput();

    inline bool isStarted() const{return started;}

    Geo::Location getLocation(QString ip) ;

    static const int INPUT_TRACK_ID = 1;

signals:
    void enteredInRoom(Login::RoomInfo room);
    void exitedFromRoom(bool error);

    //input selection
    void inputSelectionChanged();

private:

    void doAudioProcess(Audio::SamplesBuffer& in, Audio::SamplesBuffer& out);
    Audio::Plugin* createPluginInstance(Audio::PluginDescriptor* descriptor);

    Audio::AudioDriver* audioDriver;
    Midi::MidiDriver* midiDriver;//TODO use unique_ptr

    std::unique_ptr<Login::LoginService> loginService;

    Audio::AudioMixer* audioMixer;

    Audio::AbstractMp3Streamer* roomStreamer;
    long long currentStreamingRoomID;

    //ninjam
    Ninjam::Service* ninjamService;
    Controller::NinjamJamRoomController* ninjamController;

    QMap<long, Audio::AudioNode*> tracksNodes;
    mutable QMutex mutex;

    bool started;

    //VST
    Vst::VstHost* vstHost;
    std::vector<Audio::PluginDescriptor*> pluginsDescriptors;
    std::unique_ptr<Vst::PluginFinder> pluginFinder;
    //+++++++++++++++++++++++++
    void configureStyleSheet();

    void tryConnectInNinjamServer(Login::RoomInfo ninjamRoom);

    Geo::IpToLocationResolver ipToLocationResolver;

private slots:
    //Login server
    void on_disconnectedFromLoginServer();

    //VST
    void on_VSTPluginFounded(Audio::PluginDescriptor* descriptor);

    //ninjam
    void on_connectedInNinjamServer(Ninjam::Server server);
    void on_disconnectedFromNinjamServer(const Ninjam::Server& server);
    void on_errorInNinjamServer(QString error);

    //audio driver
    void on_audioDriverSampleRateChanged(int newSampleRate);
};

}
