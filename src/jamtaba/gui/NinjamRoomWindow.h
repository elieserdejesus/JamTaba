#ifndef NINJAMROOMWINDOW_H
#define NINJAMROOMWINDOW_H

#include <QWidget>
#include "../ninjam/User.h"
#include "../ninjam/Server.h"
#include "../loginserver/LoginService.h"
#include "ChatPanel.h"

class NinjamTrackGroupView;


namespace Ui {
    class NinjamRoomWindow;
    //class ChatPanel;
}

namespace Controller {
    class NinjamController;
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
    QMap<QString, NinjamTrackGroupView*> trackGroups;
    ChatPanel* chatPanel;

    void initializeMetronomeEvents();

private slots:
    //ninja interval controls
    void ninjamBpiComboChanged(QString);
    void ninjamBpmComboChanged(QString);
    void ninjamAccentsComboChanged(int );

    //metronome events
    void onPanSliderMoved(int value);
    void onFaderMoved(int value);
    void onMuteClicked();
    void onSoloClicked();

    //ninjam controller events
    void on_bpiChanged(int bpi);
    void on_bpmChanged(int bpm);
    void on_intervalBeatChanged(int beat);
    void on_channelAdded(  Ninjam::User user, Ninjam::UserChannel channel, long channelID );
    void on_channelRemoved(Ninjam::User user, Ninjam::UserChannel channel, long channelID );
    void on_channelNameChanged(Ninjam::User user, Ninjam::UserChannel channel, long channelID );
    void on_channelXmitChanged(long channelID, bool transmiting);
    void on_chatMessageReceived(Ninjam::User, QString message);

    void userSendingNewChatMessage(QString msg);

    void on_licenceButton_clicked();
};

#endif // NINJAMROOMWINDOW_H
