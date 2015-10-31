#ifndef NINJAMROOMWINDOW_H
#define NINJAMROOMWINDOW_H

#include <QWidget>
#include "../ninjam/User.h"
#include "../ninjam/Server.h"
#include "../loginserver/LoginService.h"
#include "ChatPanel.h"
#include <QLoggingCategory>
#include <QMessageBox>


class MainWindow;

class NinjamTrackGroupView;


namespace Ui {
    class NinjamRoomWindow;
    //class ChatPanel;
}

namespace Controller {
    class NinjamController;
    class MainController;
}

//enum VoteConfirmationType {BPM_CONFIRMATION_VOTE, BPI_CONFIRMATION_VOTE};

//class VoteConfirmationDialog : public QMessageBox{
//public:
//    VoteConfirmationDialog(QWidget *parent, QString title, QString text, int voteValue, VoteConfirmationType voteType);
//    inline int getVoteValue() const{return voteValue;}
//    inline VoteConfirmationType getVoteType() const{return voteType;}
//private:
//    int voteValue;//bpm or bpi value
//    VoteConfirmationType voteType;
//};


class NinjamRoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NinjamRoomWindow(MainWindow *parent, Login::RoomInfo roomInfo, Controller::MainController *mainController);
    ~NinjamRoomWindow();
    void updatePeaks();
    void updateGeoLocations();
    inline ChatPanel* getChatPanel() const{return chatPanel;}

protected:
    Ui::NinjamRoomWindow *ui;
    Controller::MainController* mainController;
private:

    QMap<QString, NinjamTrackGroupView*> trackGroups;
    ChatPanel* chatPanel;

    void initializeMetronomeEvents();

    void adjustTracksWidth();

    //void showVoteConfirmationMessageBox(QString title, QString text, int voteValue, VoteConfirmationType voteType);

    //VoteConfirmationDialog* voteConfirmationDialog;

private slots:
    //ninjam panel controls
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
    void on_userLeave(QString userName);
    void on_userEnter(QString userName);

    void userSendingNewChatMessage(QString msg);

    void on_licenceButton_clicked();

    //chat panel
    void on_userConfirmingVoteToChangeBpi(int newBpi);
    void on_userConfirmingVoteToChangeBpm(int newBpm);
    //void on_voteConfirmationDialogClosed(QAbstractButton *);
};

#endif // NINJAMROOMWINDOW_H
