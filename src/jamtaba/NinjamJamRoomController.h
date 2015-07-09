#ifndef NINJAMJAMROOMCONTROLLER_H
#define NINJAMJAMROOMCONTROLLER_H

#include <QObject>
#include <QMutex>
#include "../ninjam/User.h"
#include "../ninjam/Server.h"

#include "audio/samplesbufferrecorder.h"

class NinjamTrackNode;

namespace Audio {
    class MetronomeTrackNode;
    class SamplesBuffer;
}


namespace Controller {

class MainController;


class NinjamJamRoomController : public QObject
{
    Q_OBJECT

public:
    explicit NinjamJamRoomController(Controller::MainController* mainController);
    ~NinjamJamRoomController();
    void process(Audio::SamplesBuffer& in, Audio::SamplesBuffer& out);
    void start(const Ninjam::Server& server);
    void stop();
    bool inline isRunning() const{return running;}
    void setMetronomeBeatsPerAccent(int beatsPerAccent);
    inline int getCurrentBpi() const{return currentBpi;}
    inline int getCurrentBpm() const{return currentBpm;}
    void voteBpi(int newBpi);
    void voteBpm(int newBpm);

    void sendChatMessage(QString msg);

    static const long METRONOME_TRACK_ID = -123; //just a number :)

signals:
    void currentBpiChanged(int newBpi);
    void currentBpmChanged(int newBpm);
    void intervalBeatChanged(int intervalBeat);
    void channelAdded(Ninjam::User user,   Ninjam::UserChannel channel, long channelID);
    void channelRemoved(Ninjam::User user, Ninjam::UserChannel channel, long channelID);
    void channelNameChanged(Ninjam::User user, Ninjam::UserChannel channel, long channelID);
    //void channelAudioReceived(Ninjam::User user, long channelID, int downloadedBytes);
    void channelXmitChanged(long channelID, bool transmiting);

    void chatMsgReceived(Ninjam::User user, QString message);

private:
    Controller::MainController* mainController;
    Audio::MetronomeTrackNode* metronomeTrackNode;


    QMap<QString, NinjamTrackNode*> trackNodes;

    static QString getUniqueKey(Ninjam::UserChannel channel);


    //SamplesBufferRecorder recorder;

    void addTrack(Ninjam::User user, Ninjam::UserChannel channel);
    void removeTrack(Ninjam::User, Ninjam::UserChannel channel);

    void deleteDeactivatedTracks();

    bool running;

    long intervalPosition;
    long samplesInInterval;

    int newBpi;
    int newBpm;
    int currentBpi;
    int currentBpm;

    QMutex mutex;

    long computeTotalSamplesInInterval();
    long getSamplesPerBeat();

    void processScheduledChanges();
    inline bool hasScheduledChanges() const{return newBpi > 0 || newBpm > 0;}

    static long generateNewTrackID();

    static Audio::MetronomeTrackNode* createMetronomeTrackNode(int sampleRate);

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
    void on_ninjamDisconnectedFromServer(Ninjam::Server server);

    //audio driver events
    void on_audioDriverSampleRateChanged(int newSampleRate);
    void on_audioDriverStopped();
};

}
#endif // NINJAMJAMROOMCONTROLLER_H
