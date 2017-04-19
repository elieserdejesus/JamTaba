#ifndef NINJAMROOMWINDOW_H
#define NINJAMROOMWINDOW_H

#include <QWidget>
#include <QTimer>
#include "ninjam/UserChannel.h"
#include "ninjam/User.h"
#include "ninjam/Server.h"
#include "loginserver/LoginService.h"
#include "chat/ChatPanel.h"
#include "chat/NinjamVotingMessageParser.h"
#include <QMessageBox>
#include "NinjamPanel.h"
#include "UsersColorsPool.h"

class MainWindow;
class NinjamTrackGroupView;
class NinjamTrackView;
class QToolButton;

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
    explicit NinjamRoomWindow(MainWindow *parent, const Login::RoomInfo &roomInfo,
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

    void setTracksOrientation(Qt::Orientation orientation);

    enum TracksSize
    {
        NARROW, WIDE
    };

    void setTracksSize(TracksSize size);

    inline QString getRoomName() const { return roomInfo.getName(); }

    Login::RoomInfo getRoomInfo() const { return roomInfo; }

    bool metronomeFloatingWindowIsVisible() const;
    void closeMetronomeFloatingWindow();

protected:
    Ui::NinjamRoomWindow *ui;
    Controller::MainController *mainController;
    NinjamPanel *ninjamPanel;// panel to show interval progress, ninjam BPM/BPI controls, metronome controls, etc

    void changeEvent(QEvent *) override;

private:
    MainWindow *mainWindow;
    QMap<QString, NinjamTrackGroupView *> trackGroups;
    ChatPanel *chatPanel;

    QTimer *bpmVotingExpirationTimer;
    QTimer *bpiVotingExpiratonTimer;

    Login::RoomInfo roomInfo;

    void createVoteButton(const Gui::Chat::SystemVotingMessage &votingMessage);
    void handleChordProgressionMessage(const Ninjam::User &user, const QString &message);

    NinjamPanel *createNinjamPanel();

    void setupSignals(Controller::NinjamController *ninjamController);
    void disconnectFromNinjamControllerSignals(Controller::NinjamController *ninjamController);

    NinjamTrackView *getTrackViewByID(long trackID);

    Qt::Orientation tracksOrientation;
    TracksSize tracksSize;

    void createLayoutDirectionButtons(Qt::Orientation initialOrientation);
    QToolButton *horizontalLayoutButton;
    QToolButton *verticalLayoutButton;

    void createTracksSizeButtons(TracksSize lastTracksSize);
    QToolButton *narrowButton;
    QToolButton *wideButton;

    UsersColorsPool usersColorsPool;

    int calculateEstimatedChunksPerInterval() const;

    void updateTracksSizeButtons();// enable or disable tracks size buttons

    void updateUserNameLabel();

    void translate();

    bool canShowBlockButtonInChatMessage(const QString &userName) const;

    void updateBpmBpiLabel();

    void initializeVotingExpirationTimers();

    void showLastChordsInChat();

    static const QString JAMTABA_CHAT_BOT_NAME;

private slots:

    // ninjam panel controls
    void setNewBpi(const QString &);
    void setNewBpm(const QString &);
    void setNewBeatsPerAccent(int);
    void setNewIntervalShape(int);

    // metronome events
    void setMetronomePanSliderPosition(int value);
    void setMetronomeFaderPosition(int value);
    void toggleMetronomeMuteStatus();
    void toggleMetronomeSoloStatus();
    void showMetronomePreferences();

    // ninjam controller events
    void addChannel(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID);
    void removeChannel(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID);
    void changeChannelName(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID);
    void setChannelXmitStatus(long channelID, bool transmiting);
    void updateIntervalDownloadingProgressBar(long trackID);
    void hideIntervalDownloadingProgressBar(long trackID);
    void addChatMessage(const Ninjam::User &, const QString &message);
    void addServerTopicMessage(const QString &topicMessage);
    void handleUserLeaving(const QString &userName);
    void handleUserEntering(const QString &userName);
    void handleBpiChanges();
    void handleBpmChanges();
    void sendNewChatMessage(const QString &msg);

    void showServerLicence();

    // chat panel
    void voteToChangeBpi(int newBpi);
    void voteToChangeBpm(int newBpm);
    void blockUserInChat(const QString &userNameToBlock);
    void showFeedbackAboutBlockedUserInChat(const QString &userName);
    void showFeedbackAboutUnblockedUserInChat(const QString &userName);

    void toggleTracksLayoutOrientation(QAbstractButton *buttonClicked); // horizontal or vertical
    void toggleTracksSize(QAbstractButton *buttonClicked);// narrow or wide

    void resetBpiComboBox();
    void resetBpmComboBox();

    void setEstimatatedChunksPerIntervalInAllTracks();

    void updateStylesheet();
};

#endif // NINJAMROOMWINDOW_H
