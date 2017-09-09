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
    ChatPanel *getChatPanel() const;

    NinjamPanel *getNinjamPanel() const;

    void setTracksOrientation(Qt::Orientation orientation);

    enum TracksSize
    {
        NARROW, WIDE
    };

    void setTracksSize(TracksSize size);

    QString getRoomName() const;

    Login::RoomInfo getRoomInfo() const;

    bool metronomeFloatingWindowIsVisible() const;
    void closeMetronomeFloatingWindow();

    QList<NinjamTrackGroupView *> getTrackGroups() const;

public slots:
    void setChannelXmitStatus(long channelID, bool transmiting);

protected:
    Ui::NinjamRoomWindow *ui;
    MainWindow *mainWindow;
    Controller::MainController *mainController;
    NinjamPanel *ninjamPanel; // panel to show interval progress, ninjam BPM/BPI controls, metronome controls, etc

    void changeEvent(QEvent *) override;

private:
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

    void updateTracksSizeButtons(); // enable or disable tracks size buttons

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
    void handleAccentBeatsComboChange(int);
    void handleCustomAccentBeatsChange(const QString &);
    void setNewIntervalShape(int);

    // metronome events
    void setMetronomePanSliderPosition(int value);
    void setMetronomeFaderPosition(int value);
    void toggleMetronomeMuteStatus();
    void toggleMetronomeSoloStatus();
    void showMetronomePreferences();

    // video
    void setVideoInterval(const Ninjam::User &user, const QByteArray &encodedVideoData);

    // ninjam controller events
    void addChannel(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID);
    void removeChannel(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID);
    void changeChannelName(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID);
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
    void toggleTracksSize(QAbstractButton *buttonClicked); // narrow or wide

    void resetBpiComboBox();
    void resetBpmComboBox();

    void setEstimatatedChunksPerIntervalInAllTracks();

    void updateStylesheet();
};

inline QList<NinjamTrackGroupView *> NinjamRoomWindow::getTrackGroups() const
{
    return trackGroups.values();
}

inline ChatPanel *NinjamRoomWindow::getChatPanel() const
{
    return chatPanel;
}

inline NinjamPanel *NinjamRoomWindow::getNinjamPanel() const
{
    return ninjamPanel;
}

inline QString NinjamRoomWindow::getRoomName() const
{
    return roomInfo.getName();
}

inline Login::RoomInfo NinjamRoomWindow::getRoomInfo() const
{
    return roomInfo;
}

#endif // NINJAMROOMWINDOW_H
