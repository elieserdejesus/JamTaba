#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_MainWindow.h"
#include "BusyDialog.h"
#include "persistence/Settings.h"
#include "LocalTrackGroupView.h"
#include "ScreensaverBlocker.h"
#include "TextEditorModifier.h"
#include "performance/PerformanceMonitor.h"
#include "LooperWindow.h"

#include <QTranslator>
#include <QMainWindow>
#include <QMessageBox>
#include <QCamera>
#include <QVideoFrame>

#include "video/VideoFrameGrabber.h"
#include "video/VideoWidget.h"

class PreferencesDialog;
class LocalTrackView;
class NinjamRoomWindow;
class JamRoomViewPanel;
class ChordProgression;
class ChordsPanel;

namespace Login {
class RoomInfo;
}

namespace Controller {
class MainController;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Controller::MainController *mainController, QWidget *parent = 0);
    virtual ~MainWindow();

    void initialize();

    void detachMainController();

    virtual Persistence::LocalInputTrackSettings getInputsSettings() const;

    int getChannelGroupsCount() const;

    QString getChannelGroupName(int index) const;

    void highlightChannelGroup(int index) const;

    virtual void addChannelsGroup(const QString &name);
    void removeChannelsGroup(int channelGroupIndex);

    void exitFromRoom(bool normalDisconnection, QString disconnectionMessage = "");

    virtual Controller::MainController *getMainController() const;

    virtual void loadPreset(const Persistence::Preset &preset);
    void resetLocalChannels();

    bool isTransmiting(int channelID) const;
    void setTransmitingStatus(int channelID, bool xmitStatus);

    QStringList getChannelsNames() const;

    void showMetronomePreferencesDialog();

    virtual TextEditorModifier *createTextEditorModifier() = 0;

    QImage pickCameraFrame() const;

    bool cameraIsActivated() const;

    void closeAllFloatingWindows();

    void setTheme(const QString &themeName);

    NinjamRoomWindow* getNinjamRomWindow() const;

public slots:
    void enterInRoom(const Login::RoomInfo &roomInfo);
    void openLooperWindow(uint trackID);
    void tryEnterInRoom(const Login::RoomInfo &roomInfo, const QString &password = "");

protected:
    Controller::MainController *mainController;
    Ui::MainFrameClass ui;
    QList<LocalTrackGroupView *> localGroupChannels;

    void centerDialog(QWidget *dialog);

    virtual NinjamRoomWindow *createNinjamWindow(const Login::RoomInfo &,
                                                 Controller::MainController *) = 0;

    bool eventFilter(QObject *target, QEvent *event);

    LocalTrackGroupView *addLocalChannel(int channelGroupIndex, const QString &channelName,
                                         bool createFirstSubchannel);

    // this factory method is overrided in derived classes to create more specific views
    virtual LocalTrackGroupView *createLocalTrackGroupView(int channelGroupIndex);

    virtual void initializeLocalSubChannel(LocalTrackView *localTrackView,
                                           const Persistence::Subchannel &subChannel);

    void stopCurrentRoomStream();

    virtual void removeAllInputLocalTracks();

    template<class T>
    QList<T> getLocalChannels() const
    {
        QList<T> localChannels;
        foreach (LocalTrackGroupView *trackGroupView, localGroupChannels)
            localChannels.append(dynamic_cast<T>(trackGroupView));
        return localChannels;
    }

    void updatePublicRoomsListLayout();

    virtual bool canUseTwoColumnLayout() const;

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

protected slots:
    void closeTab(int index);
    void changeTab(int index);

    // main menu
    void openPreferencesDialog(QAction *action);

    void showNinjamCommunityWebPage();
    void showNinjamOfficialWebPage();

    void showPrivateServerDialog();

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
    void showNewVersionAvailableMessage();
    void handleIncompatiblity();
    virtual void handleServerConnectionError(const QString &errorMsg);

    // +++++  ROOM FEATURES ++++++++
    void playPublicRoomStream(const Login::RoomInfo &roomInfo);
    void stopPublicRoomStream(const Login::RoomInfo &roomInfo);

    // collapse local controls
    void toggleLocalInputsCollapseStatus();

    // channel name changed
    void updateChannelsNames();

    // room streamer
    void handlePublicRoomStreamError(const QString &msg);

    // master fader
    void setMasterGain(int faderPosition);

    // chords progression
    void acceptChordProgression(const ChordProgression &chordProgression);
    void sendCurrentChordProgressionToChat();

    void updateBpi(int bpi);
    void updateBpm(int bpm);
    void updateCurrentIntervalBeat(int beat);

    void initializeLocalInputChannels();

    QSize getSanitizedWindowSize(const QSize &size, const QSize &minimumSize) const;

    virtual void updateLocalInputChannelsGeometry();

private slots:

    void showJamtabaCurrentVersion();

    void refreshPublicRoomsList(const QList<Login::RoomInfo> &publicRooms);

    void hideChordsPanel();

    // preferences dialog (these are just the common slots between Standalone and VST, the other slots are in MainWindowStandalone class)
    void setMultiTrackRecordingStatus(bool recording);
    void setJamRecorderStatus(const QString &writerId, bool status);
    void setRecordingPath(const QString &newRecordingPath);
    void setBuiltInMetronome(const QString &metronomeAlias);
    void setCustomMetronome(const QString &primaryBeatFile, const QString &offBeatFile, const QString &accentBeatFile);

    void setLanguage(QAction * languageMenuAction);

    void updateUserName();

    void changeTheme(QAction *action);
    void translateThemeMenu();

    void handleThemeChanged();

    void updateUserNameLineEditToolTip();

    void changeCameraStatus(bool activated);
private:

    BusyDialog busyDialog;
    QTranslator jamtabaTranslator; // used to translate jamtaba texts
    QTranslator qtTranslator; // used to translate Qt texts (QMessageBox buttons, context menus, etc.)

    QMap<uint, LooperWindow *> looperWindows;

    ScreensaverBlocker screensaverBlocker;

    void showBusyDialog(const QString &message);
    void showBusyDialog();
    void hideBusyDialog();
    void centerBusyDialog();

    void closeAllLooperWindows();

    void initializeWindowSize();

    void showMessageBox(const QString &title, const QString &text, QMessageBox::Icon icon);

    int timerID; // timer used to refresh the entire GUI: animations, peak meters, etc
    static const quint8 DEFAULT_REFRESH_RATE;
    static const quint8 MAX_REFRESH_RATE;

    QPointF computeLocation() const;

    QMap<long long, JamRoomViewPanel *> roomViewPanels;

    QScopedPointer<NinjamRoomWindow> ninjamWindow;

    QScopedPointer<Login::RoomInfo> roomToJump; // store the next room reference when jumping from on room to another
    QString passwordToJump;

    static bool jamRoomLessThan(const Login::RoomInfo &r1, const Login::RoomInfo &r2);

    void initializeLoginService();
    void initializeLocalInputChannels(const Persistence::LocalInputTrackSettings &localInputSettings);

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

    void updateUserNameLabel();

    void showPeakMetersOnlyInLocalControls(bool showPeakMetersOnly);

    void setInputTracksPreparingStatus(bool preparing);

    ChordsPanel *chordsPanel;

    ChordsPanel *createChordsPanel();

    JamRoomViewPanel *createJamRoomViewPanel(const Login::RoomInfo &roomInfo);

    void setupSignals();
    void setupWidgets();

    void restoreWindowPosition();

    void updateChatTabTitle();

    void loadTranslationFile(const QString &locale);

    void setUserNameReadOnlyStatus(bool readOnly);

    void setChatVisibility(bool chatVisible);

    void openUrlInUserBrowser(const QString &url);

    void sendAcceptedChordProgressionToServer(const ChordProgression &progression);

    QString getTranslatedThemeName(const QString &themeName);

    void enableLooperButtonInLocalTracks(bool enable);

    static bool themeCanUseNightModeWorldMaps(const QString &themeName);

    static QString getStripedThemeName(const QString &fullThemeName);

    PerformanceMonitor performanceMonitor; // cpu and memmory usage
    qint64 lastPerformanceMonitorUpdate;
    static const int PERFORMANCE_MONITOR_REFRESH_TIME;

    static const QString NIGHT_MODE_SUFFIX;

};

inline NinjamRoomWindow* MainWindow::getNinjamRomWindow() const
{
    return this->ninjamWindow.data();
}

inline Controller::MainController *MainWindow::getMainController() const
{
    return mainController;
}

inline int MainWindow::getChannelGroupsCount() const
{
    return localGroupChannels.size();
}

inline QString MainWindow::getChannelGroupName(int index) const
{
    return localGroupChannels.at(index)->getGroupName();
}

#endif
