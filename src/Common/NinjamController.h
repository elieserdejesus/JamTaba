#ifndef NINJAMJAMROOMCONTROLLER_H
#define NINJAMJAMROOMCONTROLLER_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QMap>

#include "audio/Encoder.h"

class NinjamTrackNode;

namespace ninjam { namespace client {
class ServerInfo;
class User;
class UserChannel;
}}

namespace audio {
    class MetronomeTrackNode;
    class MidiSyncTrackNode;
    class SamplesBuffer;
}

namespace controller {

class MainController;

using ninjam::client::ServerInfo;
using ninjam::client::User;
using ninjam::client::UserChannel;
using audio::SamplesBuffer;
using audio::MetronomeTrackNode;
using audio::MidiSyncTrackNode;

class NinjamController : public QObject
{
    Q_OBJECT

public:
    explicit NinjamController(MainController *mainController);
    virtual ~NinjamController();
    virtual void process(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate);
    void start(const ServerInfo &server);
    void stop(bool emitDisconnectedSignal);
    bool isRunning() const;

    void setMetronomeBeatsPerAccent(int beatsPerAccent, int currentBpi);
    void setMetronomeAccentBeats(QList<int> accentBeats);
    QList<int> getMetronomeAccentBeats();

    int getCurrentBpi() const;
    int getCurrentBpm() const;

    void voteBpi(int newBpi);
    void voteBpm(int newBpm);

    void setBpm(int newBpm);
    void setBpmBpi(int initialBpm, int initalBpi);

    void setSyncEnabled(bool enabled);

    void sendChatMessage(const QString &msg);

    static const long METRONOME_TRACK_ID = 123456789;     // just a number :)
    static const long MIDI_SYNC_TRACK_ID = 1123581113;    // also just a number ;)

    void recreateEncoders();

    QByteArray encode(const SamplesBuffer &buffer, uint channelIndex);
    QByteArray encodeLastPartOfInterval(uint channelIndex);

    void scheduleEncoderChangeForChannel(int channelIndex, bool voiceChatActivated);
    void removeEncoder(int groupChannelIndex);

    void scheduleXmitChange(int channelID, bool transmiting);     // schedule the change for the next interval

    void setSampleRate(int newSampleRate);

    void reset();     // discard downloaded intervals and reset intervalPosition

    bool isPreparedForTransmit() const;

    void recreateMetronome(int newSampleRate);

    bool userIsBot(const QString userName) const;

    User getUserByName(const QString &userName) const;

    uint getSamplesPerInterval() const;

    QList<NinjamTrackNode *> getTrackNodes() const;

signals:
    void currentBpiChanged(int newBpi);     // emitted when a scheduled bpi change is processed in interval start (first beat).
    void currentBpmChanged(int newBpm);

    void intervalBeatChanged(int intervalBeat);
    void startingNewInterval();
    void startProcessing(int intervalPosition);
    void channelAdded(const User &user, const UserChannel &channel, long channelID);
    void channelRemoved(const User &user, const UserChannel &channel, long channelID);
    void channelChanged(const User &user, const UserChannel &channel, long channelID); // emmited when channel name or flags (intervalic or voice chat channel) changes
    void channelXmitChanged(long channelID, bool transmiting);
    void channelAudioChunkDownloaded(long channelID);
    void channelAudioFullyDownloaded(long channelID);
    void userLeave(const QString &userFullName);
    void userEnter(const QString &userFullName);

    void publicChatMessageReceived(const User &user, const QString &message);
    void privateChatMessageReceived(const User &user, const QString &message);
    void topicMessageReceived(const QString &message);

    void encodedAudioAvailableToSend(const QByteArray &encodedAudio, quint8 channelIndex,
                                     bool isFirstPart, bool isLastPart);

    void preparingTransmission();     // waiting for start transmission
    void preparedToTransmit();     // this signal is emmited one time, when Jamtaba is ready to transmit (after wait some complete itervals)

    void started();

protected:
    long intervalPosition;
    long samplesInInterval;

    QMap<QString, NinjamTrackNode *> trackNodes;     // the other users channels

    MainController *mainController;

    MetronomeTrackNode *metronomeTrackNode;
    MidiSyncTrackNode *midiSyncTrackNode;

private:
    static QString getUniqueKeyForChannel(const UserChannel &channel, const QString &userFullName);

    void addTrack(const User &user, const UserChannel &channel);
    void removeTrack(const User &user, const UserChannel &channel);

    bool running;
    int lastBeat;

    int currentBpi;
    int currentBpm;

    QMutex mutex;
    QMutex encodersMutex;

    long computeTotalSamplesInInterval();
    long getSamplesPerBeat();

    void processScheduledChanges();
    bool hasScheduledChanges() const;

    static long generateNewTrackID();

    MetronomeTrackNode *createMetronomeTrackNode(int sampleRate);

    QMap<int, AudioEncoder *> encoders;
    AudioEncoder *getEncoder(quint8 channelIndex);

    void handleNewInterval();
    void recreateEncoderForChannel(int channelIndex, bool voiceChannelActivated);

    void setXmitStatus(int channelID, bool transmiting);

    // ++++++++++++++++++++ nested classes to handle scheduled events +++++++++++++++++

    class SchedulableEvent;     // the interface for all events
    class BpiChangeEvent;
    class BpmChangeEvent;
    class InputChannelChangedEvent;    // user change the channel input selection from mono to stereo or vice-versa, or user added a new channel, both cases requires a new encoder in next interval
    QList<SchedulableEvent *> scheduledEvents;

    class EncodingThread;

    EncodingThread *encodingThread;

    bool preparedForTransmit;
    int waitingIntervals;
    static const int TOTAL_PREPARED_INTERVALS = 2;     // how many intervals Jamtaba will wait to start trasmiting?

private slots:
    // ninjam events
    void scheduleBpmChangeEvent(quint16 newBpm);
    void scheduleBpiChangeEvent(quint16 newBpi, quint16 oldBpi);
    void handleIntervalCompleted(const User &user, quint8 channelIndex,
                                 const QByteArray &encodedAudioData);
    void handleIntervalDownloading(const User &user, quint8 channelIndex, const QByteArray &encodedAudio, bool isFirstPart, bool isLastPart);
    void addNinjamRemoteChannel(const User &user, const UserChannel &channel);
    void removeNinjamRemoteChannel(const User &user, const UserChannel &channel);
    void updateNinjamRemoteChannel(const User &user, const UserChannel &channel);
    void handleNinjamUserExiting(const User &user);
    void handleNinjamUserEntering(const User &user);
    void handleReceivedPublicChatMessage(const User &user, const QString &message);
    void handleReceivedPrivateChatMessage(const User &user, const QString &message);
};     // end of class

inline QList<NinjamTrackNode *> NinjamController::getTrackNodes() const
{
    return trackNodes.values();
}

inline bool NinjamController::hasScheduledChanges() const
{
    return !scheduledEvents.isEmpty();
}

inline bool NinjamController::isPreparedForTransmit() const
{
    return preparedForTransmit;
}

inline int NinjamController::getCurrentBpi() const
{
    return currentBpi;
}

inline int NinjamController::getCurrentBpm() const
{
    return currentBpm;
}

inline bool NinjamController::isRunning() const
{
    return running;
}

inline uint NinjamController::getSamplesPerInterval() const
{
    return samplesInInterval;
}
} // namespace controller

#endif // NINJAMJAMROOMCONTROLLER_H
