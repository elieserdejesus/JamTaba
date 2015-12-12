#pragma once

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QFutureWatcher>
#include "ui_MainWindow.h"
#include "BusyDialog.h"
#include "../ninjam/Server.h"
#include "../loginserver/LoginService.h"
#include "../persistence/Settings.h"
#include "../LocalTrackGroupView.h"
#include "../performance/PerformanceMonitor.h"

class PluginScanDialog;
class NinjamRoomWindow;

namespace Controller{
    class MainController;
}
namespace Ui{
    class MainFrameClass;
    class MainFrame;
}

namespace Login {
    class LoginServiceParser;
    //class AbstractJamRoom;
}

namespace Audio {
class Plugin;
class PluginDescriptor;
}

namespace Vst {
class PluginDescriptor;
}

class JamRoomViewPanel;
class PluginGui;
class LocalTrackGroupView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Controller::MainController* mainController, QWidget *parent=0);
    ~MainWindow();
    virtual void closeEvent(QCloseEvent *);
    virtual void changeEvent(QEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual void resizeEvent(QResizeEvent*);

    void detachMainController();

    Persistence::InputsSettings getInputsSettings() const;

    inline int getChannelGroupsCount() const{return localGroupChannels.size();}
    inline QString getChannelGroupName(int index) const{return localGroupChannels.at(index)->getGroupName();}
    void highlightChannelGroup(int index) const;

    void addChannelsGroup(QString name);
    void removeChannelsGroup(int channelGroupIndex);

    void refreshTrackInputSelection(int inputTrackIndex);

    void enterInRoom(Login::RoomInfo roomInfo);
    void exitFromRoom(bool normalDisconnection);

    bool isRunningAsVstPlugin() const;

    inline bool isRunningInMiniMode() const{return !fullViewMode;}
    inline bool isRunningInFullViewMode() const{return fullViewMode;}

    //EZEE PRESETS NEED THAT
    inline Controller::MainController* getMainController(){return mainController ;}
    void loadPresetToTrack();//{initializeLocalInputChannels();}// finally i don't like that name ...
    void resetGroupChannel(LocalTrackGroupView *group);// should load the defaultPreset.json ?


    bool isTransmiting(int channelID) const;
    void setTransmitingStatus(int channelID, bool xmitStatus);


protected:
    Controller::MainController* mainController;

    virtual void initializePluginFinder();
    void restorePluginsList();

    void centerDialog(QWidget* dialog);

    QList<LocalTrackGroupView*> localGroupChannels;

    virtual NinjamRoomWindow* createNinjamWindow(Login::RoomInfo, Controller::MainController*) = 0;

    virtual void setFullViewStatus(bool fullViewActivated);

    bool eventFilter(QObject *target, QEvent *event);

protected slots:
    void on_tabCloseRequest(int index);
    void on_tabChanged(int index);


    void on_localTrackAdded();
    void on_localTrackRemoved();

    //themes
    //void on_newThemeSelected(QAction*);

    //main menu
    void on_preferencesClicked(QAction *action);
    void on_IOPreferencesChanged(QList<bool>, int audioDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);
    void on_ninjamCommunityMenuItemTriggered();
    void on_ninjamOfficialSiteMenuItemTriggered();
    void on_privateServerMenuItemTriggered();

    //view mode menu
    void on_menuViewModeTriggered(QAction* action);

    //ninjam controller
    void ninjamTransmissionStarted();
    void ninjamPreparingToTransmit();

    //help menu
    void on_reportBugMenuItemTriggered();
    void on_wikiMenuItemTriggered();
    void on_UsersManualMenuItemTriggered();

    //private server
    void on_privateServerConnectionAccepted(QString server, int serverPort, QString password);

    //login service
    void on_roomsListAvailable(QList<Login::RoomInfo> publicRooms);
    void on_newVersionAvailableForDownload();
    void on_incompatibilityWithServerDetected();
    virtual void on_errorConnectingToServer(QString errorMsg);

    //+++++  ROOM FEATURES ++++++++
    void on_startingRoomStream(Login::RoomInfo roomInfo);
    void on_stoppingRoomStream(Login::RoomInfo roomInfo);
    void on_enteringInRoom(Login::RoomInfo roomInfo, QString password = "");

    //plugin finder
    void onScanPluginsStarted();
    void onScanPluginsFinished(bool finishedWithoutError);
    void onPluginFounded(QString name, QString group, QString path);
    void onScanPluginsStarted(QString pluginPath);
    void onBadPluginDetected(QString pluginPath);

    //collapse local controls
    void on_localControlsCollapseButtonClicked();

    //channel name changed
    void on_channelNameChanged();


    //room streamer
    void on_RoomStreamerError(QString msg);

    //master fader
    void on_masterFaderMoved(int);

private slots:
    void on_actionFullscreenMode_triggered();
    void on_pluginFinderDialogCanceled();
    void on_currentVersionActionTriggered();
private:

    BusyDialog busyDialog;

    void showBusyDialog(QString message);
    void showBusyDialog();
    void hideBusyDialog();
    void centerBusyDialog();

    void stopCurrentRoomStream();

    void showMessageBox(QString title, QString text, QMessageBox::Icon icon);

    int timerID;

    QPointF computeLocation() const;

    QMap<long long, JamRoomViewPanel*> roomViewPanels;


    QScopedPointer<PluginScanDialog> pluginScanDialog;
    Ui::MainFrameClass ui;


    QScopedPointer<NinjamRoomWindow> ninjamWindow;

    QScopedPointer<Login::RoomInfo> roomToJump;//store the next room reference when jumping from on room to another
    QString passwordToJump;

    void showPluginGui(Audio::Plugin* plugin);

    static bool jamRoomLessThan(Login::RoomInfo r1, Login::RoomInfo r2);

    void initializeWindowState();
    void initializeLoginService();
    void initializeLocalInputChannels();

    //void initializeVstFinderStuff();
    //void initializeMainControllerEvents();
    void initializeMainTabWidget();
    void initializeViewModeMenu();

    QStringList getChannelsNames() const;

    LocalTrackGroupView* addLocalChannel(int channelGroupIndex, QString channelName, bool createFirstSubchannel);

    bool fullViewMode;//full view or mini view mode?
    bool fullScreenViewMode;

    void refreshPublicRoomsList(QList<Login::RoomInfo> publicRooms);

    void showPeakMetersOnlyInLocalControls(bool showPeakMetersOnly);

    void recalculateLeftPanelWidth();

    void setInputTracksPreparingStatus(bool preparing);

    //PerformanceMonitor performanceMonitor;//cpu and memmory usage
    //qint64 lastPerformanceMonitorUpdate;
    //static const int PERFORMANCE_MONITOR_REFRESH_TIME;

};








