#ifndef NINJAMROOMWINDOW_H
#define NINJAMROOMWINDOW_H

#include <QWidget>
#include "../ninjam/User.h"
#include "../ninjam/Server.h"
#include "../loginserver/LoginService.h"
#include "ChatPanel.h"


class NinjamTrackView;


namespace Ui {
    class NinjamRoomWindow;
    //class ChatPanel;
}

namespace Controller {
    class NinjamJamRoomController;
    class MainController;
}

class NinjamRoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NinjamRoomWindow(QWidget *parent, Login::RoomInfo roomInfo, Controller::MainController *mainController);
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
    void channelNameChanged(Ninjam::User user, Ninjam::UserChannel channel, long channelID );
    void channelXmitChanged(long channelID, bool transmiting);
    void chatMessageReceived(Ninjam::User, QString message);

    void userSendingNewChatMessage(QString msg);

    void on_licenceButton_clicked();
};

#endif // NINJAMROOMWINDOW_H
