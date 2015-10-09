#pragma once

#include <QApplication>
#include <memory>
#include <QMutex>
#include <QScopedPointer>
#include "audio/core/AudioPeak.h"

#include "geo/IpToLocationResolver.h"
#include "../ninjam/Server.h"
#include "../loginserver/LoginService.h"
#include "../audio/core/AudioDriver.h"
#include "../audio/core/plugins.h"
#include "../audio/vst/PluginFinder.h"
#include "../persistence/Settings.h"

#include "../audio/samplesbufferrecorder.h"

#include "../recorder/JamRecorder.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(controllerMain)

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

    virtual void process(const Audio::SamplesBuffer& in, Audio::SamplesBuffer& out, int sampleRate);

    void sendNewChannelsNames(QStringList channelsNames);
    void sendRemovedChannelMessage(int removedChannelIndex);

    bool addTrack(long trackID, Audio::AudioNode* trackNode);
    void removeTrack(long trackID);

    void playRoomStream(Login::RoomInfo roomInfo);
    bool isPlayingRoomStream() const;

    bool isPlayingInNinjamRoom() const;
    void stopNinjamController();

    void setTransmitingStatus(bool transmiting);
    inline bool isTransmiting() const{return transmiting;}

    void stopRoomStream();//stop currentRoom stream
    inline long long getCurrentStreamingRoomID() const{return currentStreamingRoomID;}

    void enterInRoom(Login::RoomInfo room, QStringList channelsNames, QString password = "");

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

    void setInputTrackToMIDI(int localChannelIndex, int midiDevice, int midiChannel);//use -1 to select all channels
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

    void mixGroupedInputs(int groupIndex, Audio::SamplesBuffer& out);


    inline int getAudioDriverSampleRate() const{return audioDriver->getSampleRate();}

    static QByteArray newGUID();

    //bool audioMonoInputIsFreeToSelect(int inputIndexInAudioDevice) const;
    //bool audioStereoInputIsFreeToSelect( int firstInputIndexInAudioDevice)const;

    inline const Persistence::Settings& getSettings() const{return settings;}

    //store settings
    void storeMetronomeSettings(float metronomeGain, float metronomePan, bool metronomeMuted);
    void storeIntervalProgressShape(int shape);
    void addVstScanPath(QString path);
    void addDefaultVstScanPath();//add vst path from registry
    void removeVstScanPath(int index);
    void clearVstCache();
    void storeWindowSettings(bool maximized, QPointF location);
    void storeIOSettings(int firstIn, int lastIn, int firstOut, int lastOut, int inputDevice, int outputDevice, int sampleRate, int bufferSize, QList<bool> midiInputStatus) ;
    void storeRecordingPath(QString newPath);
    void storeRecordingMultiTracksStatus(bool savingMultiTracks);
    inline bool isRecordingMultiTracksActivated() const{return settings.isSaveMultiTrackActivated();}
    void storePrivateServerSettings(QString server, int serverPort, QString password);

    void saveEncodedAudio(QString userName, quint8 channelIndex, QByteArray encodedAudio);


    inline Audio::AbstractMp3Streamer* getRoomStreamer() const{return roomStreamer;}

    void setUserName(QString newUserName);
    QString getUserName() const;
    inline bool userNameWasChoosed() const{return userNameChoosed;}

    //used to recreate audio encoder with enough channels
    int getMaxChannelsForEncodingInTrackGroup(uint trackGroupIndex) const;

    inline Vst::VstHost* getVstHost() const{return vstHost;}

    void configureStyleSheet(QString cssFile);
signals:
    void enteredInRoom(Login::RoomInfo room);
    void exitedFromRoom(bool error);

    //input selection
    void inputSelectionChanged(int inputTrackIndex);

private:

    void setAllTracksActivation(bool activated);

    void doAudioProcess(const Audio::SamplesBuffer& in, Audio::SamplesBuffer& out, int sampleRate);
    Audio::Plugin* createPluginInstance(const Audio::PluginDescriptor &descriptor);

    Audio::AudioDriver* audioDriver;
    Midi::MidiDriver* midiDriver;

    Login::LoginService* loginService;

    Audio::AudioMixer* audioMixer;

    Audio::AbstractMp3Streamer* roomStreamer;
    long long currentStreamingRoomID;

    QList<Audio::LocalInputAudioNode*> inputTracks;

    class InputTrackGroup;
    QMap<int, InputTrackGroup*> trackGroups;

    bool transmiting;

    //ninjam
    Ninjam::Service* ninjamService;
    Controller::NinjamController* ninjamController;

    QMap<long, Audio::AudioNode*> tracksNodes;
    QMutex mutex;
    //void* threadHandle;
    //void checkThread(QString methodName) const;

    bool started;

    //VST
    Vst::VstHost* vstHost;
    QList<Audio::PluginDescriptor> pluginsDescriptors;
    Vst::PluginFinder pluginFinder;
    //+++++++++++++++++++++++++


    void tryConnectInNinjamServer(Login::RoomInfo ninjamRoom, QStringList channels, QString password="");

    QScopedPointer<Geo::IpToLocationResolver> ipToLocationResolver;


    class UploadIntervalData;

    QMap<int, UploadIntervalData*> intervalsToUpload;//map the input channel indexes to a GUID (used to upload audio to ninjam server)

    Persistence::Settings settings;

    bool userNameChoosed;

    QString getUserEnvironmentString() const;

    Recorder::JamRecorder jamRecorder;
    //SamplesBufferRecorder* recorder;

    static Geo::IpToLocationResolver* buildIpToLocationResolver();
    Audio::AudioDriver* buildAudioDriver(const Persistence::Settings& settings);

    //used to sort plugins list
    static bool pluginDescriptorLessThan(const Audio::PluginDescriptor& d1, const Audio::PluginDescriptor& d2);


private slots:
    //Login server
    void on_disconnectedFromLoginServer();

    //VST
    void on_VSTPluginFounded(QString name, QString group, QString path);

    //ninjam
    void on_connectedInNinjamServer(Ninjam::Server server);
    void on_disconnectedFromNinjamServer(const Ninjam::Server& server);
    void on_errorInNinjamServer(QString error);
    void on_ninjamEncodedAudioAvailableToSend(QByteArray, quint8 channelIndex, bool isFirstPart, bool isLastPart);
    void on_ninjamBpiChanged(int newBpi);
    void on_ninjamBpmChanged(int newBpm);
    void on_newNinjamInterval();

    //audio driver
    void on_audioDriverSampleRateChanged(int newSampleRate);
    void on_audioDriverStopped();
    void on_audioDriverStarted();
};

}
