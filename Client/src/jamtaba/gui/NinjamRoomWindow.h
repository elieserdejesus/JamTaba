#ifndef NINJAMROOMWINDOW_H
#define NINJAMROOMWINDOW_H

#include <QWidget>
//#include "../loginserver/JamRoom.h"
//#include "../ninjam/Server.h"
//#include "../MainController.h"
//#include "../audio/MetronomeTrackNode.h"

#include <QSpacerItem>

namespace Ui {
class NinjamRoomWindow;
}

namespace Controller {
    class NinjamJamRoomController;
    class MainController;
}

namespace Ninjam {
    class Server;
    class UserChannel;
}

class NinjamRoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NinjamRoomWindow(QWidget *parent, Ninjam::Server* server, Controller::MainController *mainController);
    ~NinjamRoomWindow();

private:
    Ui::NinjamRoomWindow *ui;
    Controller::MainController* mainController;

private slots:
    //ninja interval controls
    void ninjamBpiComboChanged(QString);
    void ninjamBpmComboChanged(QString);
    void ninjamAccentsComboChanged(int );

    //ninjam controller events
    void bpiChanged(int bpi);
    void bpmChanged(int bpm);
    void intervalBeatChanged(int beat);
    void channelAdded(const Ninjam::UserChannel& channel, long channelID );
};

#endif // NINJAMROOMWINDOW_H
