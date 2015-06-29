#ifndef NINJAMROOMWINDOW_H
#define NINJAMROOMWINDOW_H

#include <QWidget>
#include "../ninjam/User.h"
#include "ChatPanel.h"


class NinjamTrackView;


namespace Ui {
    class NinjamRoomWindow;
    //class ChatPanel;
}

namespace Login {
class NinjamRoom;
}

namespace Controller {
    class NinjamJamRoomController;
    class MainController;
}

class NinjamRoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NinjamRoomWindow(QWidget *parent, const Login::NinjamRoom& room, Controller::MainController *mainController);
    ~NinjamRoomWindow();
    void updatePeaks();

    inline ChatPanel* getChatPanel() const{return chatPanel;}

private:
    Ui::NinjamRoomWindow *ui;
    Controller::MainController* mainController;
    QList<NinjamTrackView*> tracks;
    ChatPanel* chatPanel;

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
    void chatMessageReceived(Ninjam::User, QString message);

    void userSendingNewChatMessage(QString msg);

    void on_licenceButton_clicked();
};

#endif // NINJAMROOMWINDOW_H
