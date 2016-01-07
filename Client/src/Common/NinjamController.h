#ifndef NINJAMJAMROOMCONTROLLER_H
#define NINJAMJAMROOMCONTROLLER_H

#include <QObject>
#include <QMutex>
#include "ninjam/User.h"
#include "ninjam/Server.h"
#include "audio/vorbis/VorbisEncoder.h"

#include <QThread>

class NinjamTrackNode;

namespace Audio {
class MetronomeTrackNode;
class SamplesBuffer;
}

namespace Controller {
class MainController;

// ++++++++++++++++++++

class NinjamController : public QObject
{
    Q_OBJECT

public:
    explicit NinjamController(Controller::MainController *mainController);
    virtual ~NinjamController();
    virtual void process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate);
    void start(const Ninjam::Server &server, QMap<int, bool> channelsXmitFlags);
    void stop(bool emitDisconnectedingSignal);
    bool inline isRunning() const
    {
        return running;
    }

    void setMetronomeBeatsPerAccent(int beatsPerAccent);
    inline int getCurrentBpi() const
    {
        return currentBpi;
    }

    inline int getCurrentBpm() const
    {
        return currentBpm;
    }

    void voteBpi(int newBpi);
    void voteBpm(int newBpm);

    void setBpm(int newBpm);

    void sendChatMessage(QString msg);

    static const long METRONOME_TRACK_ID = 123456789; // just a number :)

    void recreateEncoders();

    QByteArray encode(const Audio::SamplesBuffer &buffer, uint channelIndex);
    QByteArray encodeLastPartOfInterval(uint channelIndex);

    void scheduleEncoderChangeForChannel(int channelIndex);
    void removeEncoder(int groupChannelIndex);

    void setTransmitStatus(int channelID, bool transmiting);

    void setSampleRate(int newSampleRate);

    void reset();// discard downloaded intervals and reset intervalPosition

    inline bool isPreparedForTransmit() const
    {
        return preparedForTransmit;
    }

signals:
    void currentBpiChanged(int newBpi);
    void currentBpmChanged(int newBpm);
    void intervalBeatChanged(int intervalBeat);
    void startingNewInterval();
    void startProcessing(int intervalPosition);
    void channelAdded(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID);
    void channelRemoved(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID);
    void channelNameChanged(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID);
    void channelXmitChanged(long channelID, bool transmiting);
    void channelAudioChunkDownloaded(long channelID);
    void channelAudioFullyDownloaded(long channelID);
    void userLeave(const QString &userName);
    void userEnter(const QString &userName);

    void chatMsgReceived(const Ninjam::User &user, const QString &message );

    void encodedAudioAvailableToSend(const QByteArray &encodedAudio, quint8 channelIndex, bool isFirstPart, bool isLastPart);

    void preparingTransmission();// waiting for start transmission
    void preparedToTransmit(); // this signal is emmited one time, when Jamtaba is ready to transmit (after wait some complete itervals)
private:
    Controller::MainController *mainController;
    Audio::MetronomeTrackNode *metronomeTrackNode;

    QMap<QString, NinjamTrackNode *> trackNodes;// the other users channels

    static QString getUniqueKey(Ninjam::UserChannel channel);

    void addTrack(Ninjam::User user, Ninjam::UserChannel channel);
    void removeTrack(Ninjam::User, Ninjam::UserChannel channel);

    bool running;

    long intervalPosition;
    int lastBeat;
    long samplesInInterval;

    int currentBpi;
    int currentBpm;

    QMutex mutex;

    QMutex encodersMutex;

    long computeTotalSamplesInInterval();
    long getSamplesPerBeat();

    void processScheduledChanges();
    inline bool hasScheduledChanges() const
    {
        return !scheduledEvents.isEmpty();
    }

    static long generateNewTrackID();

    static Audio::MetronomeTrackNode *createMetronomeTrackNode(int sampleRate);

    QMap<int, VorbisEncoder *> encoders;
    VorbisEncoder *getEncoder(quint8 channelIndex);

    void handleNewInterval();
    void recreateEncoderForChannel(int channelIndex);

    // ++++++++++++++++++++ nested classes to handle scheduled events +++++++++++++++++
    class SchedulableEvent;// the interface for all events
    class BpiChangeEvent;
    class BpmChangeEvent;
    class InputChannelChangedEvent;// user change the channel input selection from mono to stereo or vice-versa, or user added a new channel, both cases requires a new encoder in next interval
    class XmitChangedEvent;
    QList<SchedulableEvent *> scheduledEvents;

    class EncodingThread;

    EncodingThread *encodingThread;

    bool preparedForTransmit;
    int waitingIntervals;
    static const int TOTAL_PREPARED_INTERVALS = 2;// how many intervals Jamtaba will wait to start trasmiting?

    bool userIsBot(const QString userName) const;

private slots:
    // ninjam events
    void on_ninjamServerBpmChanged(quint16 newBpm);
    void on_ninjamServerBpiChanged(quint16 oldBpi, quint16 newBpi);
    void on_ninjamAudiointervalCompleted(const Ninjam::User &user, quint8 channelIndex, const QByteArray &encodedAudioData);
    void on_ninjamAudioIntervalDownloading(const Ninjam::User &user, quint8 channelIndex, int downloadedBytes);
    void on_ninjamUserChannelCreated(const Ninjam::User &user, const Ninjam::UserChannel &channel);
    void on_ninjamUserChannelRemoved(const Ninjam::User &user, const Ninjam::UserChannel &channel);
    void on_ninjamUserChannelUpdated(const Ninjam::User &user, const Ninjam::UserChannel &channel);
    void on_ninjamUserExited(const Ninjam::User &user);
    void on_ninjamUserEntered(const Ninjam::User &user);
};
}
#endif // NINJAMJAMROOMCONTROLLER_H
