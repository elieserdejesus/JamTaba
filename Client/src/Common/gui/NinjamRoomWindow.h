#ifndef NINJAMROOMWINDOW_H
#define NINJAMROOMWINDOW_H

#include <QWidget>
#include "ninjam/User.h"
#include "ninjam/Server.h"
#include "loginserver/LoginService.h"
#include "ChatPanel.h"
#include <QLoggingCategory>
#include <QMessageBox>
#include "NinjamPanel.h"

class MainWindow;

class NinjamTrackGroupView;

namespace Ui {
class NinjamRoomWindow;
}

namespace Controller {
class NinjamController;
class MainController;
}

class NinjamRoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NinjamRoomWindow(MainWindow *parent, Login::RoomInfo roomInfo,
                              Controller::MainController *mainController);
    ~NinjamRoomWindow();
    void updatePeaks();
    void updateGeoLocations();

    // these two components are exposed to be showed in main window
    inline ChatPanel *getChatPanel() const
    {
        return chatPanel;
    }

    inline NinjamPanel *getNinjamPanel() const
    {
        return ninjamPanel;
    }

    void setFullViewStatus(bool fullView);

protected:
    Ui::NinjamRoomWindow *ui;
    Controller::MainController *mainController;
    NinjamPanel *ninjamPanel;// panel to show interval progress, ninjam BPM/BPI controls, metronome controls, etc
private:

    QMap<QString, NinjamTrackGroupView *> trackGroups;
    ChatPanel *chatPanel;

    void adjustTracksWidth();

    bool fullViewMode;

    void handleVoteMessage(Ninjam::User user, QString message);
    void handleChordProgressionMessage(Ninjam::User user, QString message);

    NinjamPanel *createNinjamPanel();

private slots:

    // ninjam panel controls
    void ninjamBpiComboChanged(QString);
    void ninjamBpmComboChanged(QString);
    void ninjamAccentsComboChanged(int);

    // metronome events
    void on_MetronomePanSliderMoved(int value);
    void on_MetronomeFaderMoved(int value);
    void on_MetronomeMuteClicked();
    void on_MetronomeSoloClicked();

    // ninjam controller events
    void on_channelAdded(Ninjam::User user, Ninjam::UserChannel channel, long channelID);
    void on_channelRemoved(Ninjam::User user, Ninjam::UserChannel channel, long channelID);
    void on_channelNameChanged(Ninjam::User user, Ninjam::UserChannel channel, long channelID);
    void on_remoteChannelXmitChanged(long channelID, bool transmiting);
    void on_channelAudioChunkDownloaded(long trackID);
    void on_channelAudioFullyDownloaded(long trackID);
    void on_chatMessageReceived(Ninjam::User, QString message);
    void on_userLeave(QString userName);
    void on_userEnter(QString userName);

    void userSendingNewChatMessage(QString msg);

    void on_licenceButton_clicked();

    // chat panel
    void on_userConfirmingVoteToChangeBpi(int newBpi);
    void on_userConfirmingVoteToChangeBpm(int newBpm);
};

#endif // NINJAMROOMWINDOW_H
