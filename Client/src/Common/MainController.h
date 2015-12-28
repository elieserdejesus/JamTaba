#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include <QScopedPointer>

#include "geo/IpToLocationResolver.h"
#include "ninjam/Service.h"
#include "loginserver/LoginService.h"
#include "persistence/Settings.h"
#include "persistence/UsersDataCache.h"
#include "recorder/JamRecorder.h"
#include "audio/core/AudioNode.h"
#include "audio/core/Plugins.h"
#include "audio/vst/PluginFinder.h"
#include "audio/core/AudioMixer.h"
#include "audio/RoomStreamerNode.h"
#include "audio/core/PluginDescriptor.h"
#include "midi/MidiDriver.h"
#include "UploadIntervalData.h"

class MainWindow;

namespace Controller {
class NinjamController;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class MainController : public QObject
{
    Q_OBJECT

    friend class Controller::NinjamController;

protected:
    MainController(Persistence::Settings settings);

public:

    virtual ~MainController();

    virtual void start();
    virtual void stop();

    void setFullScreenView(bool fullScreen);

    virtual QString getJamtabaFlavor() const = 0; // return Standalone or VstPlugin

    inline virtual void setMainWindow(MainWindow *mainWindow)
    {
        this->mainWindow = mainWindow;
    }

    void saveLastUserSettings(const Persistence::InputsSettings &inputsSettings);

    // presets
    void loadPreset(QString name);// one preset
    QStringList getPresetList();// all presets
    void savePresets(const Persistence::InputsSettings &inputsSettings, QString name);
    void deletePreset(QString name);

    //main audio processing routine
    virtual void process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate);

    void sendNewChannelsNames(QStringList channelsNames);
    void sendRemovedChannelMessage(int removedChannelIndex);

    bool addTrack(long trackID, Audio::AudioNode *trackNode);
    void removeTrack(long trackID);

    void playRoomStream(Login::RoomInfo roomInfo);
    bool isPlayingRoomStream() const;

    bool isPlayingInNinjamRoom() const;
    virtual void stopNinjamController();

    void setTransmitingStatus(int channelID, bool transmiting);
    bool isTransmiting(int channelID) const;

    void stopRoomStream();// stop currentRoom stream
    inline long long getCurrentStreamingRoomID() const
    {
        return currentStreamingRoomID;
    }

    void enterInRoom(Login::RoomInfo room, QStringList channelsNames, QString password = "");

    Login::LoginService *getLoginService() const;

    inline Controller::NinjamController *getNinjamController() const
    {
        return ninjamController.data();
    }

    inline Ninjam::Service *getNinjamService()
    {
        return &this->ninjamService;
    }

    QStringList getBotNames() const;

    // tracks
    void setTrackMute(int trackID, bool muteStatus, bool blockSignals = false);
    bool trackIsMuted(int trackID) const;
    void setTrackSolo(int trackID, bool soloStatus, bool blockSignals = false);
    bool trackIsSoloed(int trackID) const;
    void setTrackGain(int trackID, float gain, bool blockSignals = false);
    void setTrackBoost(int trackID, float boostInDecibels);
    void setTrackPan(int trackID, float pan, bool blockSignals = false);
    void resetTrack(int trackID);// reset mute, solo, gain, pan, etc

    Audio::AudioPeak getRoomStreamPeak();
    Audio::AudioPeak getTrackPeak(int trackID);
    inline Audio::AudioPeak getMasterPeak()
    {
        return masterPeak;
    }

    inline float getMasterGain() const
    {
        return masterGain;
    }

    void setMasterGain(float newGain);

    Audio::AudioNode *getTrackNode(long ID);

    inline bool isStarted() const
    {
        return started;
    }

    Geo::Location getGeoLocation(QString ip);

    Audio::LocalInputAudioNode *getInputTrack(int localInputIndex);
    int addInputTrackNode(Audio::LocalInputAudioNode *inputTrackNode);
    void removeInputTrackNode(int inputTrackIndex);

    inline int getInputTracksCount() const
    {
        return inputTracks.size();// return the individual tracks (subchannels) count
    }

    int getInputTrackGroupsCount() const
    {
        return trackGroups.size();// return the track groups (channels) count
    }

    void mixGroupedInputs(int groupIndex, Audio::SamplesBuffer &out);

    virtual int getSampleRate() const = 0;

    virtual void setSampleRate(int newSampleRate);

    static QByteArray newGUID();

    inline const Persistence::Settings &getSettings() const
    {
        return settings;
    }

    // store settings
    void storeMetronomeSettings(float metronomeGain, float metronomePan, bool metronomeMuted);
    void storeIntervalProgressShape(int shape);

    void storeWindowSettings(bool maximized, bool usingFullViewMode, QPointF location);
    void storeIOSettings(int firstIn, int lastIn, int firstOut, int lastOut, int audioDevice,
                         int sampleRate, int bufferSize, QList<bool> midiInputStatus);
    void storeRecordingPath(QString newPath);
    void storeRecordingMultiTracksStatus(bool savingMultiTracks);
    inline bool isRecordingMultiTracksActivated() const
    {
        return settings.isSaveMultiTrackActivated();
    }

    void storePrivateServerSettings(QString server, int serverPort, QString password);

    void saveEncodedAudio(QString userName, quint8 channelIndex, QByteArray encodedAudio);

    inline Audio::AbstractMp3Streamer *getRoomStreamer() const
    {
        return roomStreamer.data();
    }

    void setUserName(QString newUserName);
    QString getUserName() const;
    inline bool userNameWasChoosed() const
    {
        return userNameChoosed;
    }

    // used to recreate audio encoder with enough channels
    int getMaxChannelsForEncodingInTrackGroup(uint trackGroupIndex) const;

    QList<Audio::PluginDescriptor> pluginsDescriptors;

    // used to sort plugins list
    static bool pluginDescriptorLessThan(const Audio::PluginDescriptor &d1,
                                         const Audio::PluginDescriptor &d2);

    virtual Audio::Plugin *createPluginInstance(const Audio::PluginDescriptor &descriptor) = 0;
    QList<Audio::PluginDescriptor> getPluginsDescriptors();
    Audio::Plugin *addPlugin(int inputTrackIndex, const Audio::PluginDescriptor &descriptor);
    void removePlugin(int inputTrackIndex, Audio::Plugin *plugin);

    void configureStyleSheet(QString cssFile);

    Vst::PluginFinder *getPluginFinder() const
    {
        return pluginFinder.data();
    }

    void addPluginsScanPath(QString path);
    void addBlackVstToSettings(QString path);
    virtual void addDefaultPluginsScanPath() = 0;// add vst path from registry
    void removePluginsScanPath(QString path);
    void removeBlackVst(int index);
    void clearPluginsCache();
    virtual void scanPlugins(bool scanOnlyNewPlugins = false) = 0;

    void finishUploads();// used to send the last part of ninjam intervals when audio is stopped.

    virtual QString getUserEnvironmentString() const;

    void cancelPluginFinder();

    // to remembering ninjamers controls (pan, level, gain, boost)
    inline Persistence::UsersDataCache *getUsersDataCache() // TODO hide this from callers. Create a function in mainController to update the CacheEntries, so MainController is used as a Façade.
    {
        return &usersDataCache;
    }

protected:

    static QString LOG_CONFIG_FILE;

    Login::LoginService loginService;

    Audio::AudioMixer audioMixer;

    // ninjam
    Ninjam::Service ninjamService;
    QScopedPointer<Controller::NinjamController> ninjamController;

    Persistence::Settings settings;

    QList<Audio::LocalInputAudioNode *> inputTracks;

    QScopedPointer<Vst::PluginFinder> pluginFinder;
    virtual Vst::PluginFinder *createPluginFinder() = 0;

    virtual Controller::NinjamController *createNinjamController(MainController *) = 0;

    MainWindow *mainWindow;

    virtual void setCSS(QString css) = 0;

    virtual Midi::MidiBuffer pullMidiBuffer() = 0;

    // map the input channel indexes to a GUID (used to upload audio to ninjam server)
    QMap<int, UploadIntervalData *> intervalsToUpload;

private:
    void setAllTracksActivation(bool activated);

    //audio process is here too (see MainController::process)
    void doAudioProcess(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate);

    QScopedPointer<Audio::AbstractMp3Streamer> roomStreamer;
    long long currentStreamingRoomID;

    class InputTrackGroup;
    QMap<int, InputTrackGroup *> trackGroups;

    QMap<int, bool> getXmitChannelsFlags() const;

    QMap<long, Audio::AudioNode *> tracksNodes;
    QMutex mutex;

    bool started;

    void tryConnectInNinjamServer(Login::RoomInfo ninjamRoom, QStringList channels,
                                  QString password = "");

    QScopedPointer<Geo::IpToLocationResolver> ipToLocationResolver;

    bool userNameChoosed;

    Recorder::JamRecorder jamRecorder;

    // master
    float masterGain;
    Audio::AudioPeak masterPeak;

    Persistence::UsersDataCache usersDataCache;

protected slots:

    // ninjam
    virtual void on_connectedInNinjamServer(Ninjam::Server server);
    virtual void on_disconnectedFromNinjamServer(const Ninjam::Server &server);
    virtual void on_errorInNinjamServer(QString error);
    virtual void on_ninjamEncodedAudioAvailableToSend(QByteArray, quint8 channelIndex,
                                                      bool isFirstPart, bool isLastPart);
    virtual void on_ninjamBpiChanged(int newBpi);
    virtual void on_ninjamBpmChanged(int newBpm);
    virtual void on_newNinjamInterval();
    virtual void on_ninjamStartProcessing(int intervalPosition);

    // audio driver
    virtual void on_audioDriverSampleRateChanged(int newSampleRate);
    virtual void on_audioDriverStarted();
    virtual void on_audioDriverStopped();

    virtual void on_VSTPluginFounded(QString name, QString group, QString path)
    {
        Q_UNUSED(name)
        Q_UNUSED(group)
        Q_UNUSED(path)
    }
};
}

#endif
