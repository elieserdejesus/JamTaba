#pragma once

#include <QApplication>
#include <memory>
#include <QMutex>
#include "audio/core/AudioPeak.h"

#include "geo/IpToLocationResolver.h"
#include "../ninjam/Server.h"
#include "../loginserver/LoginService.h"
#include "../audio/core/AudioDriver.h"
#include "../audio/core/plugins.h"
#include "../audio/vst/PluginFinder.h"
#include "../persistence/Settings.h"

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
    class LocalInputAudioNode;
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
class NinjamController;


//++++++++++++++++++++++++++++
class MainController : public QApplication, public Audio::AudioDriverListener
{
    Q_OBJECT

    friend class Controller::AudioListener;
    friend class Controller::NinjamController;

public:
    MainController(JamtabaFactory *factory, Persistence::Settings settings, int& argc, char** argv);
    ~MainController();

    void start();
    void stop();

    void saveLastUserSettings(const Persistence::InputsSettings &inputsSettings);

    virtual void process(Audio::SamplesBuffer& in, Audio::SamplesBuffer& out);

    void sendNewChannelsNames(QStringList channelsNames);
    void sendRemovedChannelMessage(int removedChannelIndex);

    bool addTrack(long trackID, Audio::AudioNode* trackNode);
    void removeTrack(long trackID);

    void playRoomStream(Login::RoomInfo roomInfo);
    bool isPlayingRoomStream() const;

    bool isPlayingInNinjamRoom() const;

    void stopRoomStream();//stop currentRoom stream
    inline long long getCurrentStreamingRoomID() const{return currentStreamingRoomID;}

    void enterInRoom(Login::RoomInfo room, QStringList channelsNames);

    Audio::AudioDriver* getAudioDriver() const;
    Midi::MidiDriver* getMidiDriver() const;
    Login::LoginService* getLoginService() const;


    inline Controller::NinjamController* getNinjamController() const{return ninjamController;}

    QList<Audio::PluginDescriptor> getPluginsDescriptors();

    Audio::Plugin* addPlugin(int inputTrackIndex, const Audio::PluginDescriptor& descriptor);
    void removePlugin(int inputTrackIndex, Audio::Plugin* plugin);

    QStringList getBotNames() const;

    //tracks
    void setTrackMute(int trackID, bool muteStatus);
    bool trackIsMuted(int trackID) const;
    void setTrackSolo(int trackID, bool soloStatus);
    bool trackIsSoloed(int trackID) const;
    void setTrackLevel(int trackID, float level);
    void setTrackPan(int trackID, float pan);

    Audio::AudioPeak getRoomStreamPeak();
    Audio::AudioPeak getTrackPeak(int trackID);

    const Vst::PluginFinder& getPluginFinder() const{return pluginFinder;}
    void scanPlugins();
    void initializePluginsList(QStringList paths);

    Audio::AudioNode* getTrackNode(long ID);

    void updateInputTracksRange();//called when input range or method (audio or midi) are changed in preferences
    void setInputTrackToMono(int localChannelIndex, int inputIndexInAudioDevice);
    void setInputTrackToStereo(int localChannelIndex, int firstInputIndex);
    void setInputTrackToMIDI(int localChannelIndex, int midiDevice);
    void setInputTrackToNoInput(int localChannelIndex);

    inline bool isStarted() const{return started;}

    Geo::Location getGeoLocation(QString ip) ;

    //static const int INPUT_TRACK_ID = 1;
    Audio::LocalInputAudioNode* getInputTrack(int localInputIndex);
    int addInputTrackNode(Audio::LocalInputAudioNode* inputTrackNode);
    void removeInputTrackNode(int inputTrackIndex);

    //return the individual tracks (subchannels) count
    inline int getInputTracksCount() const{return inputTracks.size();}
    int getInputTrackGroupsCount() const{return trackGroups.size();} //return the track groups (channels) count

    inline int getAudioDriverSampleRate() const{return audioDriver->getSampleRate();}

    static QByteArray newGUID();

    //bool audioMonoInputIsFreeToSelect(int inputIndexInAudioDevice) const;
    //bool audioStereoInputIsFreeToSelect( int firstInputIndexInAudioDevice)const;

    inline const Persistence::Settings& getSettings() const{return settings;}

    //store settings
    void storeMetronomeSettings(float gain, float pan);
    void addVstScanPath(QString path);
    void removeVstScanPath(int index);
    void clearVstCache();
    void storeWindowSettings(bool maximized, QPointF location);
    void storeIOSettings(int firstIn, int lastIn, int firstOut, int lastOut, int inputDevice, int outputDevice, int sampleRate, int bufferSize, int midiDevice) ;
    //void storeInputChannels()
signals:
    void enteredInRoom(Login::RoomInfo room);
    void exitedFromRoom(bool error);

    //input selection
    void inputSelectionChanged(int inputTrackIndex);

private:

    void doAudioProcess(Audio::SamplesBuffer& in, Audio::SamplesBuffer& out);
    Audio::Plugin* createPluginInstance(const Audio::PluginDescriptor &descriptor);

    Audio::AudioDriver* audioDriver;
    Midi::MidiDriver* midiDriver;//TODO use unique_ptr

    std::unique_ptr<Login::LoginService> loginService;

    Audio::AudioMixer* audioMixer;

    Audio::AbstractMp3Streamer* roomStreamer;
    long long currentStreamingRoomID;

    QList<Audio::LocalInputAudioNode*> inputTracks;

    class InputTrackGroup;
    QMap<int, InputTrackGroup*> trackGroups;

    //ninjam
    Ninjam::Service* ninjamService;
    Controller::NinjamController* ninjamController;

    QMap<long, Audio::AudioNode*> tracksNodes;
    mutable QMutex mutex;

    bool started;

    //VST
    Vst::VstHost* vstHost;
    QList<Audio::PluginDescriptor> pluginsDescriptors;
    Vst::PluginFinder pluginFinder;
    //+++++++++++++++++++++++++
    void configureStyleSheet();

    void tryConnectInNinjamServer(Login::RoomInfo ninjamRoom, QStringList channels);

    Geo::IpToLocationResolver ipToLocationResolver;

    QMap<int, QByteArray> currentGUIDs;//map the input channel indexes to a GUID (used to upload audio to ninjam server)

    Persistence::Settings settings;



private slots:
    //Login server
    void on_disconnectedFromLoginServer();

    //VST
    void on_VSTPluginFounded(QString name, QString group, QString path);

    //ninjam
    void on_connectedInNinjamServer(Ninjam::Server server);
    void on_disconnectedFromNinjamServer(const Ninjam::Server& server);
    void on_errorInNinjamServer(QString error);
    void on_ninjamAudioAvailableToSend(QByteArray encodedAudio, quint8 channelIndex, bool isFirstPart, bool isLastPart);

    //audio driver
    void on_audioDriverSampleRateChanged(int newSampleRate);
    void on_audioDriverStopped();
};

}
