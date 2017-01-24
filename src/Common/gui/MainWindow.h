#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_MainWindow.h"
#include <QMainWindow>
#include <QMessageBox>
#include "BusyDialog.h"
#include "persistence/Settings.h"
#include "LocalTrackGroupView.h"
#include "ScreensaverBlocker.h"

#include <QTranslator>

#include "performance/PerformanceMonitor.h"

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

    inline int getChannelGroupsCount() const
    {
        return localGroupChannels.size();
    }

    inline QString getChannelGroupName(int index) const
    {
        return localGroupChannels.at(index)->getGroupName();
    }

    void highlightChannelGroup(int index) const;

    virtual void addChannelsGroup(const QString &name);
    void removeChannelsGroup(int channelGroupIndex);

    void exitFromRoom(bool normalDisconnection, QString disconnectionMessage = "");

    virtual inline Controller::MainController *getMainController()
    {
        return mainController;
    }

    virtual void loadPreset(const Persistence::Preset &preset);
    void resetLocalChannels();

    bool isTransmiting(int channelID) const;
    void setTransmitingStatus(int channelID, bool xmitStatus);

    QStringList getChannelsNames() const;

    void showMetronomePreferencesDialog();

public slots:
    void enterInRoom(const Login::RoomInfo &roomInfo);

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

    virtual void initializeWindowMinimumSize();

    virtual void doWindowInitialization();

    static const QSize MAIN_WINDOW_MIN_SIZE;

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

    void tryEnterInRoom(const Login::RoomInfo &roomInfo, const QString &password = "");

    void initializeLocalInputChannels();

    QSize getSanitizedMinimumWindowSize(const QSize &prefferedMinimumWindowSize) const;

private slots:

    void showJamtabaCurrentVersion();

    void updateLocalInputChannelsGeometry();

    void refreshPublicRoomsList(const QList<Login::RoomInfo> &publicRooms);

    void hideChordsPanel();

    //preferences dialog (these are just the common slots between Standalone and VST, the other slots are in MainWindowStandalone class)
    void setMultiTrackRecordingStatus(bool recording);
    void setJamRecorderStatus(QString writerId, bool status);
    void setRecordingPath(const QString &newRecordingPath);
    void setBuiltInMetronome(const QString &metronomeAlias);
    void setCustomMetronome(const QString &primaryBeatFile, const QString &secondaryBeatFile);

    void setLanguage(QAction * languageMenuAction);

    void updateUserName();

    void changeTheme(QAction *action);
    void translateThemeMenu();

    void updateNightModeInWorldMaps();

private:

    BusyDialog busyDialog;
    QTranslator jamtabaTranslator; // used to translate jamtaba texts
    QTranslator qtTranslator; // used to translate Qt texts (QMessageBox buttons, context menus, etc.)

    ScreensaverBlocker screensaverBlocker;

    void showBusyDialog(const QString &message);
    void showBusyDialog();
    void hideBusyDialog();
    void centerBusyDialog();

    void initializeWindowSize();

    void showMessageBox(const QString &title, const QString &text, QMessageBox::Icon icon);

    void setTheme(const QString &themeName);

    int timerID; // timer used to refresh the entire GUI: animations, peak meters, etc
    static const quint8 DEFAULT_REFRESH_RATE;
    static const quint8 MAX_REFRESH_RATE;

    QPointF computeLocation() const;

    QMap<long long, JamRoomViewPanel *> roomViewPanels;

    QScopedPointer<NinjamRoomWindow> ninjamWindow;

    QScopedPointer<Login::RoomInfo> roomToJump;// store the next room reference when jumping from on room to another
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

    static bool themeCanUseNightModeWorldMaps(const QString &themeName);

    static QString getStripedThemeName(const QString &fullThemeName);

    PerformanceMonitor performanceMonitor;//cpu and memmory usage
    qint64 lastPerformanceMonitorUpdate;
    static const int PERFORMANCE_MONITOR_REFRESH_TIME;

    static const QString NIGHT_MODE_SUFFIX;

};

#endif
