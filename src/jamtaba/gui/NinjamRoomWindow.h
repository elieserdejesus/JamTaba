#ifndef NINJAMROOMWINDOW_H
#define NINJAMROOMWINDOW_H

#include <QWidget>
//#include "../loginserver/JamRoom.h"
#include "../ninjam/Server.h"
#include "../MainController.h"
#include "../audio/MetronomeTrackNode.h"

#include <QSpacerItem>

using namespace Login;

namespace Ui {
class NinjamRoomWindow;
}

class NinjamRoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NinjamRoomWindow(QWidget *parent, Ninjam::Server* server, Controller::MainController *mainController);
    ~NinjamRoomWindow();

private:
    Ui::NinjamRoomWindow *ui;
    Ninjam::Server* server;
    Controller::MainController* mainController;
    Audio::MetronomeTrackNode* metronomeTrackNode;

    long intervalPosition;
    long samplesInInterval;

    int newBpi;
    int newBpm;

    long computeTotalSamplesInInterval();
    long getSamplesPerBeat();

    void processScheduledChanges();

private slots:
    //ninjam events
    void ninjamServerBpmChanged(short newBpm);
    void ninjamServerBpiChanged(short oldBpi, short newBpi);

    //main controller
    void audioSamplesProcessed(int samplesProcessed);

    //ninja interval controls
    void ninjamBpiComboChanged(int);
    void ninjamBpmComboChanged(int);
};

#endif // NINJAMROOMWINDOW_H
