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
#include "../persistence/Settings.h"

#include "../audio/samplesbufferrecorder.h"

#include "../recorder/JamRecorder.h"

#include "audio/core/plugins.h"
#include "audio/vst/PluginFinder.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(controllerMain)

class MainWindow;

namespace Ninjam{
    class Service;
    //class Server;
}

namespace Audio {
    class AudioDriver;
    class AudioDriverListener;
    class AbstractMp3Streamer;
    //class Plugin;
    class AudioMixer;
    class SamplesBuffer;
    class AudioNode;
    //class PluginDescriptor;
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


class JamtabaFactory;

//+++++++++++++

class MainWindow;

namespace Controller {

class AudioListener;
class NinjamController;


//++++++++++++++++++++++++++++

class MainController;

class MainControllerSignalsHandler : public QObject{//using this class to avoid MainController inheriting from QObject
    Q_OBJECT
public:
    MainControllerSignalsHandler(MainController* controller);
protected:
    MainController* mainController;
public slots:
    //Login server
    //virtual void on_disconnectedFromLoginServer();

    //ninjam
    virtual void on_connectedInNinjamServer(Ninjam::Server server);
    virtual void on_disconnectedFromNinjamServer(const Ninjam::Server& server);
    virtual void on_errorInNinjamServer(QString error);
    virtual void on_ninjamEncodedAudioAvailableToSend(QByteArray, quint8 channelIndex, bool isFirstPart, bool isLastPart);
    virtual void on_ninjamBpiChanged(int newBpi);
    virtual void on_ninjamBpmChanged(int newBpm);
    virtual void on_newNinjamInterval();
    virtual void on_ninjamStartProcessing(int intervalPosition);

    //audio driver
    virtual void on_audioDriverSampleRateChanged(int newSampleRate);
    virtual void on_audioDriverStarted();
    virtual void on_audioDriverStopped();

    //
    virtual void on_VSTPluginFounded(QString name, QString group, QString path){}
};




class MainController :  public Audio::AudioDriverListener
{

    friend class Controller::AudioListener;
    friend class Controller::NinjamController;
    friend class Controller::MainControllerSignalsHandler;

protected:
    MainController(Persistence::Settings settings);
public:

    virtual ~MainController();

    void start();
    void stop();

    inline void setMainWindow(MainWindow* mainWindow){this->mainWindow = mainWindow;}

    void saveLastUserSettings(const Persistence::InputsSettings &inputsSettings);

    virtual void process(const Audio::SamplesBuffer& in, Audio::SamplesBuffer& out, int sampleRate);

    void sendNewChannelsNames(QStringList channelsNames);
    void sendRemovedChannelMessage(int removedChannelIndex);

    bool addTrack(long trackID, Audio::AudioNode* trackNode);
    void removeTrack(long trackID);

    void playRoomStream(Login::RoomInfo roomInfo);
    bool isPlayingRoomStream() const;

    bool isPlayingInNinjamRoom() const;
    virtual void stopNinjamController();

    void setTransmitingStatus(bool transmiting);
    inline bool isTransmiting() const{return transmiting;}

    void stopRoomStream();//stop currentRoom stream
    inline long long getCurrentStreamingRoomID() const{return currentStreamingRoomID;}

    void enterInRoom(Login::RoomInfo room, QStringList channelsNames, QString password = "");

    Audio::AudioDriver* getAudioDriver() const;
    Midi::MidiDriver* getMidiDriver() const;
    Login::LoginService* getLoginService() const;


    inline Controller::NinjamController* getNinjamController() const{return ninjamController;}



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

    QList<Audio::PluginDescriptor> pluginsDescriptors;
    //used to sort plugins list
    static bool pluginDescriptorLessThan(const Audio::PluginDescriptor& d1, const Audio::PluginDescriptor& d2);

    virtual Audio::Plugin* createPluginInstance(const Audio::PluginDescriptor &descriptor) = 0;
    QList<Audio::PluginDescriptor> getPluginsDescriptors();
    Audio::Plugin* addPlugin(int inputTrackIndex, const Audio::PluginDescriptor& descriptor);
    void removePlugin(int inputTrackIndex, Audio::Plugin* plugin);

    void configureStyleSheet(QString cssFile);

    Vst::PluginFinder* getPluginFinder() const{return pluginFinder;}

    void addPluginsScanPath(QString path);
    virtual void addDefaultPluginsScanPath() = 0;//add vst path from registry
    void removePluginsScanPath(int index);
    void clearPluginsCache();
    virtual void scanPlugins() = 0;
signals:
    //void enteredInRoom(Login::RoomInfo room);
    //void exitedFromRoom(bool error);

    //input selection
    //void inputSelectionChanged(int inputTrackIndex);

protected:
    Audio::AudioDriver* audioDriver;
    Midi::MidiDriver* midiDriver;

    Login::LoginService* loginService;

    Audio::AudioMixer* audioMixer;

    //ninjam
    Ninjam::Service* ninjamService;
    Controller::NinjamController* ninjamController;

    Persistence::Settings settings;

    QList<Audio::LocalInputAudioNode*> inputTracks;

    Vst::PluginFinder* pluginFinder;

    virtual void exit() = 0;

    //factory methods
    virtual MainControllerSignalsHandler* createSignalsHandler();
    virtual Midi::MidiDriver* createMidiDriver() = 0;
    virtual Audio::AudioDriver* buildAudioDriver(const Persistence::Settings& settings) = 0;
    virtual Vst::PluginFinder* createPluginFinder() = 0;

    MainWindow* mainWindow;

    MainControllerSignalsHandler* signalsHandler;

    virtual void setCSS(QString css) = 0;

    virtual bool useMidiDriver(){ return true;}

private:
    void setAllTracksActivation(bool activated);
    void doAudioProcess(const Audio::SamplesBuffer& in, Audio::SamplesBuffer& out, int sampleRate);


    Audio::AbstractMp3Streamer* roomStreamer;
    long long currentStreamingRoomID;



    class InputTrackGroup;
    QMap<int, InputTrackGroup*> trackGroups;

    bool transmiting;



    QMap<long, Audio::AudioNode*> tracksNodes;
    QMutex mutex;
    //void* threadHandle;
    //void checkThread(QString methodName) const;

    bool started;


    //+++++++++++++++++++++++++


    void tryConnectInNinjamServer(Login::RoomInfo ninjamRoom, QStringList channels, QString password="");

    QScopedPointer<Geo::IpToLocationResolver> ipToLocationResolver;


    class UploadIntervalData;

    QMap<int, UploadIntervalData*> intervalsToUpload;//map the input channel indexes to a GUID (used to upload audio to ninjam server)



    bool userNameChoosed;

    QString getUserEnvironmentString() const;

    Recorder::JamRecorder jamRecorder;
    //SamplesBufferRecorder* recorder;

    static Geo::IpToLocationResolver* buildIpToLocationResolver();




};

}
