#ifndef NINJAMJAMROOMCONTROLLER_H
#define NINJAMJAMROOMCONTROLLER_H

#include <QObject>
#include <QMutex>
#include "../ninjam/User.h"
#include "../ninjam/Server.h"


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

signals:
    void currentBpiChanged(int newBpi);
    void currentBpmChanged(int newBpm);
    void intervalBeatChanged(int intervalBeat);
    void channelAdded(Ninjam::User user,   Ninjam::UserChannel channel, long channelID);
    void channelRemoved(Ninjam::User user, Ninjam::UserChannel channel, long channelID);
    void channelChanged(Ninjam::User user, Ninjam::UserChannel channel, long channelID);
    void chatMsgReceived(Ninjam::User user, QString message);

private:
    Controller::MainController* mainController;
    Audio::MetronomeTrackNode* metronomeTrackNode;
    QMap<QString, NinjamTrackNode*> trackNodes;

    static QString getUniqueKey(Ninjam::UserChannel channel);


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

    long computeTotalSamplesInInterval();
    long getSamplesPerBeat();

    void processScheduledChanges();
    inline bool hasScheduledChanges() const{return newBpi > 0 || newBpm > 0;}

    static long generateNewTrackID();

    QMutex mutex;

private slots:
    //ninjam events
    void ninjamServerBpmChanged(short newBpm);
    void ninjamServerBpiChanged(short oldBpi, short newBpi);
    void ninjamAudioAvailable(Ninjam::User user, int channelIndex, QByteArray encodedAudioData);
    void ninjamUserChannelCreated(Ninjam::User user, Ninjam::UserChannel channel);
    void ninjamUserChannelRemoved(Ninjam::User user, Ninjam::UserChannel channel);
    void ninjamUserChannelUpdated(Ninjam::User user, Ninjam::UserChannel channel);
    void ninjamUserLeave(Ninjam::User user);
    void ninjamDisconnectedFromServer(Ninjam::Server server);
};

}
#endif // NINJAMJAMROOMCONTROLLER_H
