#ifndef NINJAMJAMROOMCONTROLLER_H
#define NINJAMJAMROOMCONTROLLER_H

#include <QObject>
#include <QMutex>
#include "../ninjam/User.h"
#include "../ninjam/Server.h"
#include "../audio/vorbis/VorbisEncoder.h"

#include <QThread>

class NinjamTrackNode;

namespace Audio {
    class MetronomeTrackNode;
    class SamplesBuffer;
}


namespace Controller {

class MainController;


//++++++++++++++++++++

class NinjamController : public QObject
{
    Q_OBJECT

public:
    explicit NinjamController(Controller::MainController* mainController);
    virtual ~NinjamController();
    virtual void process(const Audio::SamplesBuffer& in, Audio::SamplesBuffer& out, int sampleRate);
    void start(const Ninjam::Server& server, bool transmiting);
    void stop(bool emitDisconnectedingSignal);
    bool inline isRunning() const{return running;}
    void setMetronomeBeatsPerAccent(int beatsPerAccent);
    inline int getCurrentBpi() const{return currentBpi;}
    inline int getCurrentBpm() const{return currentBpm;}
    void voteBpi(int newBpi);
    void voteBpm(int newBpm);

    void setBpm(int newBpm);

    void sendChatMessage(QString msg);

    static const long METRONOME_TRACK_ID = 123456789; //just a number :)

    void recreateEncoders();

    QByteArray encode(const Audio::SamplesBuffer& buffer, uint channelIndex);
    QByteArray encodeLastPartOfInterval(uint channelIndex);

    void scheduleEncoderChangeForChannel(int channelIndex);
    void removeEncoder(int groupChannelIndex);

    void setTransmitStatus(bool transmiting);

    void setSampleRate(int newSampleRate);

    void reset();//discard downloaded intervals and reset intervalPosition

signals:
    void currentBpiChanged(int newBpi);
    void currentBpmChanged(int newBpm);
    void intervalBeatChanged(int intervalBeat);
    void startingNewInterval();
    void startProcessing(int intervalPosition);
    void channelAdded(Ninjam::User user,   Ninjam::UserChannel channel, long channelID);
    void channelRemoved(Ninjam::User user, Ninjam::UserChannel channel, long channelID);
    void channelNameChanged(Ninjam::User user, Ninjam::UserChannel channel, long channelID);
    void channelXmitChanged(long channelID, bool transmiting);
    void userLeave(QString userName);
    void userEnter(QString userName);

    void chatMsgReceived(Ninjam::User user, QString message);

    void encodedAudioAvailableToSend(QByteArray encodedAudio, quint8 channelIndex, bool isFirstPart, bool isLastPart);
private:
    Controller::MainController* mainController;
    Audio::MetronomeTrackNode* metronomeTrackNode;

    QMap<QString, NinjamTrackNode*> trackNodes;//the other users channels

    static QString getUniqueKey(Ninjam::UserChannel channel);

    //SamplesBufferRecorder recorder;

    void addTrack(Ninjam::User user, Ninjam::UserChannel channel);
    void removeTrack(Ninjam::User, Ninjam::UserChannel channel);

    //void deleteDeactivatedTracks();

    bool running;
    bool transmiting;

    long intervalPosition;
    long samplesInInterval;

    int currentBpi;
    int currentBpm;

    QMutex mutex;

    QMutex encodersMutex;

    long computeTotalSamplesInInterval();
    long getSamplesPerBeat();

    void processScheduledChanges();
    inline bool hasScheduledChanges() const{return !scheduledEvents.isEmpty();}

    static long generateNewTrackID();

    static Audio::MetronomeTrackNode* createMetronomeTrackNode(int sampleRate);

    QMap<int, VorbisEncoder*> encoders;
    VorbisEncoder* getEncoder(quint8 channelIndex);

    void handleNewInterval();
    void recreateEncoderForChannel(int channelIndex);

    //++++++++++++++++++++ nested classes to handle scheduled events +++++++++++++++++
    class SchedulableEvent;//the interface for all events
    class BpiChangeEvent;
    class BpmChangeEvent;
    class InputChannelChangedEvent;//user change the channel input selection from mono to stereo or vice-versa, or user added a new channel, both cases requires a new encoder in next interval
    class XmitChangedEvent;
    QList<SchedulableEvent*> scheduledEvents;

    class EncodingThread;

    EncodingThread* encodingThread;

    //QList<NinjamTrackNode*> tracksToDelete;



private slots:
    //ninjam events
    void on_ninjamServerBpmChanged(short newBpm);
    void on_ninjamServerBpiChanged(short oldBpi, short newBpi);
    void on_ninjamAudiointervalCompleted(Ninjam::User user, int channelIndex, QByteArray encodedAudioData);
    void on_ninjamAudiointervalDownloading(Ninjam::User user, int channelIndex, int downloadedBytes);
    void on_ninjamUserChannelCreated(Ninjam::User user, Ninjam::UserChannel channel);
    void on_ninjamUserChannelRemoved(Ninjam::User user, Ninjam::UserChannel channel);
    void on_ninjamUserChannelUpdated(Ninjam::User user, Ninjam::UserChannel channel);
    void on_ninjamUserLeave(Ninjam::User user);
    void on_ninjamUserEnter(Ninjam::User user);
    //void on_ninjamDisconnectedFromServer(Ninjam::Server server);
};

}
#endif // NINJAMJAMROOMCONTROLLER_H
