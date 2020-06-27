#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_MainWindow.h"

#include <QTranslator>
#include <QMainWindow>
#include <QMessageBox>
#include <QCamera>
#include <QCameraInfo>
#include <QVideoFrame>
#include <QComboBox>
#include <QFileInfo>

class PreferencesDialog;
class LocalTrackView;
class NinjamRoomWindow;
class JamRoomViewPanel;
class ChordProgression;
class ChordsPanel;
class ChatPanel;
class InactivityDetector;
class LocalTrackGroupView;
class CameraFrameGrabber;
class VideoWidget;
class BusyDialog;
class LooperWindow;
class UsersColorsPool;
class ScreensaverBlocker;
class PerformanceMonitor;
class TextEditorModifier;
class PrivateServerWindow;

namespace login {
class RoomInfo;
class MainChat;
}

namespace controller {
class MainController;
}

namespace ninjam { namespace client {
class User;
struct ChannelMetadata;
}}

namespace gui { namespace chat {
class SystemVotingMessage;
}}

namespace persistence {
class LocalInputTrackSettings;
class Preset;
class SubChannel;
class Channel;
}

using ninjam::client::User;
using persistence::LocalInputTrackSettings;
using persistence::Preset;
using persistence::Channel;
using persistence::SubChannel;
using gui::chat::SystemVotingMessage;
using login::MainChat;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Q_PROPERTY(QColor tintColor MEMBER tintColor WRITE setTintColor)

public:
    MainWindow(controller::MainController *mainController, QWidget *parent = 0);
    virtual ~MainWindow();

    virtual void initialize();

    void detachMainController();

    virtual LocalInputTrackSettings getInputsSettings() const;

    int getChannelGroupsCount() const;

    QString getChannelGroupName(int index) const;

    void highlightChannelGroup(int index) const;

    virtual void addChannelsGroup(int instrumentIndex);
    void removeChannelsGroup(int channelGroupIndex);

    void exitFromRoom(bool normalDisconnection, QString disconnectionMessage = "");

    virtual controller::MainController *getMainController() const;

    virtual void loadPreset(const Preset &preset);
    void resetLocalChannels();

    bool isTransmiting(int channelID) const;
    void setTransmitingStatus(int channelID, bool xmitStatus);

    void setVoiceChatStatus(int channelID, bool voiceChatStatus);


    QList<ninjam::client::ChannelMetadata> getChannelsMetadata() const;

    void showMetronomePreferencesDialog();

    virtual TextEditorModifier *createTextEditorModifier() = 0;

    QImage pickCameraFrame() const;

    bool cameraIsActivated() const;

    void closeAllFloatingWindows();

    void setTheme(const QString &themeName);

    NinjamRoomWindow* getNinjamRomWindow() const;

    UsersColorsPool *getUsersColorsPool() const;

    void setTintColor(const QColor &color);
    QColor getTintColor() const;

    void fillUserContextMenu(QMenu &menu, const QString &userFullName, bool addInvitationEntry);

    ChordsPanel* getChordsPanel() const;

public slots:
    void enterInRoom(const login::RoomInfo &roomInfo);
    void openLooperWindow(uint trackID);
    void tryEnterInRoom(const login::RoomInfo &roomInfo, const QString &password = "");

    void showFeedbackAboutBlockedUserInChat(const QString &userFullName);
    void showFeedbackAboutUnblockedUserInChat(const QString &userFullName);

    void addNinjamServerChatMessage(const User &, const QString &message);
    void addPrivateChatMessage(const User &, const QString &message);
    void addPrivateChat(const QString &remoteUserName, const QString &userIP);

protected:
    controller::MainController *mainController;
    Ui::MainFrameClass ui;
    QList<LocalTrackGroupView *> localGroupChannels;

    void centerDialog(QWidget *dialog);

    virtual NinjamRoomWindow *createNinjamWindow(const login::RoomInfo &, controller::MainController *) = 0;

    bool eventFilter(QObject *target, QEvent *event) override;

    LocalTrackGroupView *addLocalChannel(int channelGroupIndex, int instrumentIndex,
                                         bool createFirstSubchannel);

    // this factory method is overrided in derived classes to create more specific views
    virtual LocalTrackGroupView *createLocalTrackGroupView(int channelGroupIndex);

    virtual void initializeLocalSubChannel(LocalTrackView *localTrackView, const SubChannel &subChannel);

    void stopCurrentRoomStream();

    virtual void removeAllInputLocalTracks();

    template<class T>
    QList<T> getLocalChannels() const
    {
        QList<T> localChannels;

        for (auto trackGroupView : localGroupChannels)
            localChannels.append(dynamic_cast<T>(trackGroupView));

        return localChannels;
    }

    void updatePublicRoomsListLayout();

    virtual bool canUseTwoColumnLayoutInPublicRooms() const;

    virtual PreferencesDialog *createPreferencesDialog() = 0;

    virtual void setupPreferencesDialogSignals(PreferencesDialog *dialog);

    void closeEvent(QCloseEvent *) override;
    void changeEvent(QEvent *) override;
    void timerEvent(QTimerEvent *) override;
    void resizeEvent(QResizeEvent *) override;

    virtual void doWindowInitialization();

    virtual inline QSize getMinimumWindowSize() const { return MAIN_WINDOW_MIN_SIZE; }

    static const QSize MAIN_WINDOW_MIN_SIZE;

    QCamera *camera;
    CameraFrameGrabber *videoFrameGrabber;
    VideoWidget *cameraView;
    QComboBox *cameraCombo;
    QVBoxLayout *cameraLayout;
    QString preferredCameraName;

    QColor tintColor;

protected slots:
    void closeContentTab(int index);
    void changeTab(int index);

    // main menu
    void openPreferencesDialog(QAction *action);

    void showNinjamCommunityWebPage();
    void showNinjamOfficialWebPage();

    void showConnectWithPrivateServerDialog();
    void showPrivateServerWindow();

    // view menu
    void updateMeteringMenu();
    void handleMenuMeteringAction(QAction *);

    // ninjam controller
    void startTransmission();
    void prepareTransmission();

    // help menu
    void showJamtabaIssuesWebPage();
    void showJamtabaWikiWebPage();
    void showJamtabaUsersManual();
    void showJamtabaTranslators();

    // private server
    void connectInPrivateServer(const QString &server, int serverPort, const QString &userName, const QString &password);

    // login service
    void showNewVersionAvailableMessage(const QString &versionTag, const QString &publicationDate, const QString &latestVersionDetails);
    static QString sanitizeLatestVersionDetails(const QString &details);

    // +++++  ROOM FEATURES ++++++++
    void playPublicRoomStream(const login::RoomInfo &roomInfo);
    void stopPublicRoomStream(const login::RoomInfo &roomInfo);

    // collapse areas
    void toggleLocalTracksCollapseStatus();
    void toggleBottomAreaCollapseStatus();
    void setBottomCollapsedStatus(bool collapsed);
    void collapseBottomArea(bool collapse);

    // channel name changed
    void updateChannelsNames();

    // room streamer
    void handlePublicRoomStreamError(const QString &msg);

    // master fader
    void setMasterGain(int faderPosition);

    // chords progression
    void acceptChordProgression(const ChordProgression &chordProgression);

    void updateBpi(int bpi);
    void updateBpm(int bpm);
    void updateCurrentIntervalBeat(int beat);

    void initializeLocalInputChannels();

    QSize getSanitizedWindowSize(const QSize &size, const QSize &minimumSize) const;

    virtual void updateLocalInputChannelsGeometry();

    void setNetworkUsageUpdatePeriod(quint32 periodInMilliseconds);

private slots:

    void showJamtabaCurrentVersion();

    void refreshPublicRoomsList(const QList<login::RoomInfo> &publicRooms);

    void showChordsPanel();

    void chatCollapseChanged(bool chatCollapsed);

    // preferences dialog (these are just the common slots between Standalone and VST, the other slots are in MainWindowStandalone class)
    void setMultiTrackRecordingStatus(bool recording);
    void setJamRecorderStatus(const QString &writerId, bool status);
    void setRecordingPath(const QString &newRecordingPath);
    void setJamDirectoryDateFormat(const QString &newDateFormat);
    void setBuiltInMetronome(const QString &metronomeAlias);
    void setCustomMetronome(const QString &primaryBeatFile, const QString &offBeatFile, const QString &accentBeatFile);

    void setLanguage(QAction * languageMenuAction);

    void updateUserName();

    void changeTheme(QAction *action);
    void translateThemeMenu();

    void handleThemeChanged();

    void translateCollapseButtonsToolTips();

    void translatePublicChatCountryNames();

    void updateUserNameLineEditToolTip();

    void changeCameraStatus(bool activated);

    void selectNewCamera(int cameraIndex);

    void handleUserLeaving(const QString &userFullName);
    void handleUserEntering(const QString &userName);

    void handleChordProgressionMessage(const User &user, const QString &message);
    void sendChatMessageToNinjamServer(const QString &msg);
    void voteToChangeBpi(int newBpi);
    void voteToChangeBpm(int newBpm);

    void blockUserInChat();
    void unblockUserInChat();

    void fillConnectedUserContextMenu(QMenu &menu, const QString &userFullName);

    void connectInMainChat();

    bool chordsPanelIsVisible() const;

private:

    static const QString JAMTABA_CHAT_BOT_NAME;

    bool bottomCollapsed;

    BusyDialog *busyDialog;
    QTranslator jamtabaTranslator; // used to translate jamtaba texts
    QTranslator qtTranslator; // used to translate Qt texts (QMessageBox buttons, context menus, etc.)

    QMap<uint, LooperWindow *> looperWindows;

    QTimer *bpmVotingExpirationTimer;
    QTimer *bpiVotingExpiratonTimer;

    QPushButton *buttonCollapseLocalChannels;
    QPushButton *buttonCollapseChat;
    QPushButton *buttonCollapseBottomArea;

    QLabel *performanceMonitorLabel;

    InactivityDetector *xmitInactivityDetector;

    QScopedPointer<UsersColorsPool> usersColorsPool;

    QScopedPointer<ScreensaverBlocker> screensaverBlocker;

    QScopedPointer<MainChat> mainChat;

    QScopedPointer<PrivateServerWindow> privateServerWindow;

    void showBusyDialog(const QString &message);
    void showBusyDialog();
    void hideBusyDialog();
    void centerBusyDialog();

    void createPrivateChat(const QString &remoteUserName, const QString &remoteUserIP, bool focusNewChat);
    void setPrivateChatInputstatus(const QString userName, bool enabled);

    void closeAllLooperWindows();

    void initializeWindowSize();

    void removeTabCloseButton(QTabWidget *tabWidget, int buttonIndex);

    void initializeVotingExpirationTimers();

    void initializeCollapseButtons();
    void updateCollapseButtons();

    void showMessageBox(const QString &title, const QString &text, QMessageBox::Icon icon);

    void wireNinjamSignals();

    QList<login::RoomInfo> loadPrivateServersFromJson(const QFileInfo &privateServersFile);

    int timerID; // timer used to refresh the entire GUI: animations, peak meters, etc
    static const quint8 DEFAULT_REFRESH_RATE;
    static const quint8 MAX_REFRESH_RATE;

    QPointF computeLocation() const;

    QMap<QString, JamRoomViewPanel *> roomViewPanels;

    QScopedPointer<NinjamRoomWindow> ninjamWindow;

    QScopedPointer<login::RoomInfo> roomToJump; // store the next room reference when jumping from on room to another
    QString passwordToJump;

    QLabel *transmitTransferRateLabel;
    QLabel *transmitIcon;
    QLabel *receiveTransferRateLabel;
    QLabel *receiveIcon;

    quint32 networkUsageUpdatePeriod;
    const static quint32 DEFAULT_NETWORK_USAGE_UPDATE_PERIOD;

    static bool jamRoomLessThan(const login::RoomInfo &r1, const login::RoomInfo &r2);

    void setCameraComboVisibility(bool show);

    void initializeCamera(const QString &cameraDeviceName);

    void initializeLoginService();
    void initializeLocalInputChannels(const persistence::LocalInputTrackSettings &localInputSettings);

    void initializeMainTabWidget();
    void initializeViewMenu();

    void initializeMasterFader();

    void initializeThemeMenu();

    void initializeLanguageMenu();
    void initializeTranslator();

    void initializeMeteringOptions();

    void initializeGuiRefreshTimer();

    void initializeCameraWidget();

    QCamera::FrameRateRange getBestSupportedFrameRate() const;
    QSize getBestCameraResolution(const QList<QSize> resolutions) const;

    void updateUserNameLabel();

    void showPeakMetersOnlyInLocalControls(bool showPeakMetersOnly);

    void setInputTracksPreparingStatus(bool preparing);

    ChordsPanel *createChordsPanel();

    JamRoomViewPanel *createJamRoomViewPanel(const login::RoomInfo &roomInfo);

    void setupSignals();
    void setupWidgets();

    void restoreWindowPosition();

    void createMainChat(bool turnedOn);
    void createNinjamServerChat(const QString &serverName);
    void addNinjamPanelsInBottom();

    void createVoteButton(const SystemVotingMessage &votingMessage);
    bool canShowBlockButtonInChatMessage(const QString &userFullName) const;

    void loadTranslationFile(const QString &locale);

    void setUserNameReadOnlyStatus(bool readOnly);

    void openUrlInUserBrowser(const QString &url);

    QString getTranslatedThemeName(const QString &themeName);

    void enableLooperButtonInLocalTracks(bool enable);

    static bool themeCanUseNightModeWorldMaps(const QString &themeName);

    static QString getStripedThemeName(const QString &fullThemeName);

    void setupMainTabCornerWidgets();

    static QString buildServerInviteMessage(const QString &serverIP, quint16 serverPort, bool isPrivateServer, bool showPrivateServerIpAndPort);

    QScopedPointer<PerformanceMonitor> performanceMonitor; // cpu and memmory usage
    qint64 lastPerformanceMonitorUpdate; // TODO move to PerformenceMonitor
    static const int PERFORMANCE_MONITOR_REFRESH_TIME;

    static const QString NIGHT_MODE_SUFFIX;

};

inline QColor MainWindow::getTintColor() const
{
    return tintColor;
}

inline UsersColorsPool *MainWindow::getUsersColorsPool() const
{
    return usersColorsPool.data();
}

inline NinjamRoomWindow* MainWindow::getNinjamRomWindow() const
{
    return this->ninjamWindow.data();
}

inline controller::MainController *MainWindow::getMainController() const
{
    return mainController;
}

inline int MainWindow::getChannelGroupsCount() const
{
    return localGroupChannels.size();
}

#endif
