#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_MainWindow.h"
#include "BusyDialog.h"
#include "chords/ChordsPanel.h"
#include "PluginScanDialog.h"
#include "NinjamRoomWindow.h"
#include "MainController.h"
#include "JamRoomViewPanel.h"
#include "LocalTrackGroupView.h"
// #include "performance/PerformanceMonitor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Controller::MainController *mainController, QWidget *parent = 0);
    ~MainWindow();

    virtual void initialize(); //this is overrided in inherited classes, never call virtual methods inside constructor

    void closeEvent(QCloseEvent *) override;
    void changeEvent(QEvent *) override;
    void timerEvent(QTimerEvent *) override;
    void resizeEvent(QResizeEvent *) override;

    void detachMainController();

    virtual Persistence::InputsSettings getInputsSettings() const;

    inline int getChannelGroupsCount() const
    {
        return localGroupChannels.size();
    }

    inline QString getChannelGroupName(int index) const
    {
        return localGroupChannels.at(index)->getGroupName();
    }

    void highlightChannelGroup(int index) const;

    virtual void addChannelsGroup(QString name);
    void removeChannelsGroup(int channelGroupIndex);

    //void refreshTrackInputSelection(int inputTrackIndex);

    void exitFromRoom(bool normalDisconnection);

    bool canCreateSubchannels() const;
    bool canUseFullScreen() const;

    inline bool isRunningInMiniMode() const
    {
        return !fullViewMode;
    }

    inline bool isRunningInFullViewMode() const
    {
        return fullViewMode;
    }

    // EZEE PRESETS NEED THAT
    inline Controller::MainController *getMainController()
    {
        return mainController;
    }

    virtual void loadPresetToTrack();
    void resetGroupChannel(LocalTrackGroupView *group);// should load the defaultPreset.json ?

    bool isTransmiting(int channelID) const;
    void setTransmitingStatus(int channelID, bool xmitStatus);

    QStringList getChannelsNames() const;

public slots:
    void enterInRoom(Login::RoomInfo roomInfo);

protected:
    Controller::MainController *mainController;
    Ui::MainFrameClass ui;

    virtual void initializePluginFinder();


    void centerDialog(QWidget *dialog);

    QList<LocalTrackGroupView *> localGroupChannels;

    virtual NinjamRoomWindow *createNinjamWindow(Login::RoomInfo, Controller::MainController *) = 0;

    virtual void setFullViewStatus(bool fullViewActivated);

    bool eventFilter(QObject *target, QEvent *event);

    LocalTrackGroupView *addLocalChannel(int channelGroupIndex, QString channelName,
                                                 bool createFirstSubchannel,
                                                 bool initializeAsNoInput);


    //this factory method overrided in derived classes to create more specific views
    virtual LocalTrackGroupView* createLocalTrackGroupView(int channelGroupIndex);

    virtual void showPreferencesDialog(int initialTab) = 0;

    void stopCurrentRoomStream();


    //virtual void loadPluginsList(QList<Persistence::Plugin> plugins, StandaloneLocalTrackView* trackView) = 0;

    virtual void loadSubChannel(Persistence::Subchannel subChannel, LocalTrackView* subChannelView) = 0;

    virtual void initializeSubChannel(Persistence::Subchannel subChannel, LocalTrackView* subChannelView) = 0; //TODO the code to loading channels (load preset) and to initialize local inputs is the same, but is almost duplicated at moment.

protected slots:
    void closeTab(int index);
    void changeTab(int index);

    // main menu
    void openPreferencesDialog(QAction *action);

    void showNinjamCommunityWebPage();
    void showNinjamOfficialWebPage();

    void showPrivateServerDialog();

    // view mode menu
    void changeViewMode(QAction *action);

    // ninjam controller
    void startTransmission();
    void prepareTransmission();

    // help menu
    void showJamtabaIssuesWebPage();
    void showJamtabaWikiWebPage();
    void showJamtabaUsersManual();

    // private server
    void connectInPrivateServer(QString server, int serverPort, QString password);

    // login service
    void showNewVersionAvailableMessage();
    void handleIncompatiblity();
    virtual void handleServerConnectionError(QString errorMsg);

    // +++++  ROOM FEATURES ++++++++
    void playPublicRoomStream(Login::RoomInfo roomInfo);
    void stopPublicRoomStream(Login::RoomInfo roomInfo);

    // plugin finder
    void showPluginScanDialog();
    void hidePluginScanDialog(bool finishedWithoutError);
    void addFoundedPlugin(QString name, QString group, QString path);
    void setCurrentScanningPlugin(QString pluginPath);
    void addPluginToBlackList(QString pluginPath);

    // collapse local controls
    void toggleLocalInputsCollapseStatus();

    // channel name changed
    void updateChannelsNames();

    // room streamer
    void handlePublicRoomStreamError(QString msg);

    // master fader
    void setMasterFaderPosition(int);

    // chords progression
    void showChordProgression(ChordProgression chordProgression);
    void sendCurrentChordProgressionToChat();

    void updateBpi(int bpi);
    void updateBpm(int bpm);
    void updateCurrentIntervalBeat(int beat);

    void tryEnterInRoom(Login::RoomInfo roomInfo, QString password = "");

private slots:
    void toggleFullScreen();
    void closePluginScanDialog();
    void showJamtabaCurrentVersion();

    void updateLocalInputChannelsGeometry();

    void refreshPublicRoomsList(QList<Login::RoomInfo> publicRooms);


private:

    BusyDialog busyDialog;

    void showBusyDialog(QString message);
    void showBusyDialog();
    void hideBusyDialog();
    void centerBusyDialog();

    void showMessageBox(QString title, QString text, QMessageBox::Icon icon);

    int timerID;

    QPointF computeLocation() const;

    QMap<long long, JamRoomViewPanel *> roomViewPanels;

    QScopedPointer<PluginScanDialog> pluginScanDialog;

    QScopedPointer<NinjamRoomWindow> ninjamWindow;

    QScopedPointer<Login::RoomInfo> roomToJump;// store the next room reference when jumping from on room to another
    QString passwordToJump;

    void showPluginGui(Audio::Plugin *plugin);

    static bool jamRoomLessThan(Login::RoomInfo r1, Login::RoomInfo r2);

    void initializeWindowState();
    void initializeLoginService();
    void initializeLocalInputChannels();

    void initializeMainTabWidget();
    void initializeViewModeMenu();

    bool fullViewMode;// full view or mini view mode?
    bool fullScreenViewMode;
    void setFullScreenStatus(bool fullScreen);

    void showPeakMetersOnlyInLocalControls(bool showPeakMetersOnly);

    void setInputTracksPreparingStatus(bool preparing);

    ChordsPanel *chordsPanel;
    void hideChordsPanel();
    ChordsPanel *createChordsPanel();

    // PerformanceMonitor performanceMonitor;//cpu and memmory usage
    // qint64 lastPerformanceMonitorUpdate;
    // static const int PERFORMANCE_MONITOR_REFRESH_TIME;

    static const QSize MINI_MODE_MIN_SIZE;
    static const QSize FULL_VIEW_MODE_MIN_SIZE;
};

#endif
