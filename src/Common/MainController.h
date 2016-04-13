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
#include "audio/core/AudioMixer.h"
#include "audio/RoomStreamerNode.h"
#include "midi/MidiDriver.h"
#include "UploadIntervalData.h"
#include "audio/core/LocalInputGroup.h"

class MainWindow;

namespace Controller {
class NinjamController;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class MainController : public QObject
{
    Q_OBJECT

    friend class Controller::NinjamController;

protected:
    MainController(const Persistence::Settings &settings);

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

    void saveLastUserSettings(const Persistence::LocalInputTrackSettings &inputsSettings);

    // presets
    virtual Persistence::Preset loadPreset(const QString &name);// one preset
    QStringList getPresetList();// return all presets names
    void savePreset(const Persistence::LocalInputTrackSettings &inputsSettings, const QString &name);
    void deletePreset(const QString &name); //not used yet

    // main audio processing routine
    virtual void process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate);

    void sendNewChannelsNames(const QStringList &channelsNames);
    void sendRemovedChannelMessage(int removedChannelIndex);

    bool addTrack(long trackID, Audio::AudioNode *trackNode);
    void removeTrack(long trackID);

    void playRoomStream(const Login::RoomInfo &roomInfo);
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

    inline Qt::Orientation getLastTracksLayoutOrientation() const
    {
        return settings.getLastTracksLayoutOrientation();
    }

    inline void storeTracksLayoutOrientation(Qt::Orientation newOrietation)
    {
        settings.storeTracksLayoutOrientation(newOrietation);
    }

    inline void storeTracksSize(bool usingNarrowedTracks)
    {
        settings.storeTracksSize(usingNarrowedTracks);
    }

    inline bool isUsingNarrowedTracks() const
    {
        return settings.isUsingNarrowedTracks();
    }

    void enterInRoom(const Login::RoomInfo &room, const QStringList &channelsNames, const QString &password = "");

    Login::LoginService *getLoginService() const;

    virtual inline Controller::NinjamController *getNinjamController() const
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

    Geo::Location getGeoLocation(const QString &ip);

    Audio::LocalInputNode *getInputTrack(int localInputIndex);
    virtual int addInputTrackNode(Audio::LocalInputNode *inputTrackNode);
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

    static QByteArray newGUID();

    inline const Persistence::Settings &getSettings() const
    {
        return settings;
    }

    // store settings
    void storeMetronomeSettings(float metronomeGain, float metronomePan, bool metronomeMuted);
    void storeIntervalProgressShape(int shape);

    void storeWindowSettings(bool maximized, bool usingFullViewMode, QPointF location);
    void storeIOSettings(int firstIn, int lastIn, int firstOut, int lastOut, int audioDevice, const QList<bool> &midiInputStatus);
    void storeRecordingPath(const QString &newPath);
    void storeRecordingMultiTracksStatus(bool savingMultiTracks);
    inline bool isRecordingMultiTracksActivated() const
    {
        return settings.isSaveMultiTrackActivated();
    }

    void storePrivateServerSettings(const QString &server, int serverPort, const QString &password);

    inline bool isUsingCustomMetronomeSounds() const
    {
        return settings.isUsingCustomMetronomeSounds();
    }

    void setTranslationLanguage(const QString &languageCode);

    void setBuiltInMetronome(const QString &metronomeAlias);
    void setCustomMetronome(const QString &primaryBeatFile, const QString &secondaryBeatFile);

    inline QString getMetronomeFirstBeatFile() const{
        return settings.getMetronomeFirstBeatFile();
    }

    inline QString getMetronomeSecondaryBeatFile() const{
        return settings.getMetronomeSecondaryBeatFile();
    }

    void saveEncodedAudio(const QString &userName, quint8 channelIndex, const QByteArray &encodedAudio);

    inline Audio::AbstractMp3Streamer *getRoomStreamer() const
    {
        return roomStreamer.data();
    }

    void setUserName(const QString &newUserName);

    QString getUserName() const;

    inline bool userNameWasChoosed() const
    {
        return !settings.getUserName().isEmpty();
    }

    // used to recreate audio encoder with enough channels
    int getMaxChannelsForEncodingInTrackGroup(uint trackGroupIndex) const;

    void configureStyleSheet(const QString &cssFile);

    //TODO: move this code to NinjamController.
    void finishUploads();// used to send the last part of ninjam intervals when audio is stopped.

    virtual QString getUserEnvironmentString() const;

    // to remembering ninjamers controls (pan, level, gain, boost)
    inline Persistence::UsersDataCache *getUsersDataCache() // TODO hide this from callers. Create a function in mainController to update the CacheEntries, so MainController is used as a Façade.
    {
        return &usersDataCache;
    }

    static QString getSuggestedUserName();

public slots:
    virtual void setSampleRate(int newSampleRate);

protected:

    static QString LOG_CONFIG_FILE;

    Login::LoginService loginService;

    Audio::AudioMixer audioMixer;

    // ninjam
    Ninjam::Service ninjamService;
    QScopedPointer<Controller::NinjamController> ninjamController;

    Persistence::Settings settings;

    QMap<int, Audio::LocalInputNode *> inputTracks;

    virtual Controller::NinjamController *createNinjamController() = 0;

    MainWindow *mainWindow;

    virtual void setCSS(const QString &css) = 0;

    virtual Midi::MidiMessageBuffer pullMidiBuffer() = 0;

    // map the input channel indexes to a GUID (used to upload audio to ninjam server)
    QMap<int, UploadIntervalData *> intervalsToUpload;

    QMutex mutex;

    virtual void setupNinjamControllerSignals();

private:
    void setAllTracksActivation(bool activated);

    // audio process is here too (see MainController::process)
    void doAudioProcess(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate);

    QScopedPointer<Audio::AbstractMp3Streamer> roomStreamer;
    long long currentStreamingRoomID;

    QMap<int, Audio::LocalInputGroup *> trackGroups;

    QMap<int, bool> getXmitChannelsFlags() const;

    QMap<long, Audio::AudioNode *> tracksNodes;

    bool started;

    void tryConnectInNinjamServer(const Login::RoomInfo &ninjamRoom, const QStringList &channels,
                                  const QString &password = "");

    QScopedPointer<Geo::IpToLocationResolver> ipToLocationResolver;

    Recorder::JamRecorder jamRecorder;

    // master
    float masterGain;
    Audio::AudioPeak masterPeak;

    Persistence::UsersDataCache usersDataCache;

    int lastInputTrackID; //used to generate a unique key/ID for each input track


    void recreateMetronome();

protected slots:

    // ninjam
    virtual void connectedNinjamServer(const Ninjam::Server &server);
    virtual void disconnectFromNinjamServer(const Ninjam::Server &server);
    virtual void quitFromNinjamServer(const QString &error);
    virtual void enqueueAudioDataToUpload(const QByteArray &, quint8 channelIndex,
                                          bool isFirstPart, bool isLastPart);
    virtual void updateBpi(int newBpi);
    virtual void updateBpm(int newBpm);

    // TODO move this slot to NinjamController
    virtual void on_newNinjamInterval();

};
}

#endif
