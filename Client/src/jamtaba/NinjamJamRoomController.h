#ifndef NINJAMJAMROOMCONTROLLER_H
#define NINJAMJAMROOMCONTROLLER_H

#include <QObject>
#include "audio/MetronomeTrackNode.h"

namespace Controller {

class MainController;


class NinjamJamRoomController : public QObject
{
    Q_OBJECT

public:
    NinjamJamRoomController(Controller::MainController* mainController);
    ~NinjamJamRoomController();
    void process(Audio::SamplesBuffer& in, Audio::SamplesBuffer& out);
    void start(int initialBpm, int initialBpi);
    void stop();
    bool inline isRunning() const{return running;}
signals:
    void currentBpiChanged(int newBpi);
    void currentBpmChanged(int newBpm);
    void intervalBeatChanged(int intervalBeat);

private:
    Controller::MainController* mainController;
    Audio::MetronomeTrackNode* metronomeTrackNode;

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

private slots:
    //ninjam events
    void ninjamServerBpmChanged(short newBpm);
    void ninjamServerBpiChanged(short oldBpi, short newBpi);

};

}
#endif // NINJAMJAMROOMCONTROLLER_H
