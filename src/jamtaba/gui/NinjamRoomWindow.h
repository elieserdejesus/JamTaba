#ifndef NINJAMROOMWINDOW_H
#define NINJAMROOMWINDOW_H

#include <QWidget>
#include <QSpacerItem>
#include "../ninjam/User.h"
#include "../ninjam/UserChannel.h"

namespace Ui {
class NinjamRoomWindow;
}

namespace Login {
class NinjamRoom;
}

namespace Controller {
    class NinjamJamRoomController;
    class MainController;
}

namespace Ninjam {
    //class Server;
    class UserChannel;
    class User;
}

class NinjamRoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NinjamRoomWindow(QWidget *parent, const Login::NinjamRoom& room, Controller::MainController *mainController);
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
    void channelAdded(  Ninjam::User user, Ninjam::UserChannel channel, long channelID );
    void channelRemoved(Ninjam::User user, Ninjam::UserChannel channel, long channelID );
    void channelChanged(Ninjam::User user, Ninjam::UserChannel channel, long channelID );
};

#endif // NINJAMROOMWINDOW_H
