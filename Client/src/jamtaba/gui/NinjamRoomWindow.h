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
    Controller::NinjamJamRoomController* ninjamController;

private slots:
    //ninja interval controls
    void ninjamBpiComboChanged(int);
    void ninjamBpmComboChanged(int);

    //ninjam controller events
    void bpiChanged(int bpi);
    void bpmChanged(int bpm);
    void intervalBeatChanged(int beat);
};

#endif // NINJAMROOMWINDOW_H
