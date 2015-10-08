#pragma once

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QFutureWatcher>
#include "ui_mainframe.h"
#include "BusyDialog.h"
#include "../ninjam/Server.h"
#include "../loginserver/LoginService.h"
#include "../persistence/Settings.h"
#include "../LocalTrackGroupView.h"

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

namespace Plugin {
class PluginDescriptor;
}

class JamRoomViewPanel;
class PluginGui;
class LocalTrackGroupView;
//class MetronomeTrackView;


//class PluginLoader : public QObject{
//    Q_OBJECT
//public:
//    PluginLoader(Controller::MainController* mainController, Persistence::Plugin plugin, LocalTrackView* trackView);
//    ~PluginLoader();

//    void load();
//private slots:
//    void on_futureWatcherFinished();

//private:
//    Persistence::Plugin plugin;
//    LocalTrackView* trackView;
//    Controller::MainController* mainController;

//    QFutureWatcher<Audio::Plugin*> futureWatcher;

//    Audio::Plugin* loadPlugin();
//};


class MainFrame : public QMainWindow
{
    Q_OBJECT

public:
    MainFrame(Controller::MainController* mainController, QWidget *parent=0);
    ~MainFrame();
    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent*);
    virtual void changeEvent(QEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual void resizeEvent(QResizeEvent*);

    Persistence::InputsSettings getInputsSettings() const;

    inline int getChannelGroupsCount() const{return localChannels.size();}
    inline QString getChannelGroupName(int index) const{return localChannels.at(index)->getGroupName();}
    void highlightChannelGroup(int index) const;

    void addChannelsGroup(QString name);
    void removeChannelsGroup(int channelGroupIndex);

private slots:
    void on_tabCloseRequest(int index);
    void on_tabChanged(int index);


    //themes
    void on_newThemeSelected(QAction*);

    //main menu
    void on_preferencesClicked(QAction *action);
    void on_IOPreferencesChanged(QList<bool>, int audioDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);
    void on_ninjamCommunityMenuItemTriggered();
    void on_ninjamOfficialSiteMenuItemTriggered();
    void on_privateServerMenuItemTriggered();
    void on_reportBugMenuItemTriggered();

    //private server
    void on_privateServerConnectionAccepted(QString server, int serverPort, QString password);

    //login service
    void on_roomsListAvailable(QList<Login::RoomInfo> publicRooms);
    void on_newVersionAvailableForDownload();
    void on_incompatibilityWithServerDetected();
    void on_errorConnectingToServer();

    //+++++  ROOM FEATURES ++++++++
    void on_startingRoomStream(Login::RoomInfo roomInfo);
    void on_stoppingRoomStream(Login::RoomInfo roomInfo);
    void on_enteringInRoom(Login::RoomInfo roomInfo, QString password = "");
    void on_enteredInRoom(Login::RoomInfo roomInfo);
    void on_exitedFromRoom(bool normalDisconnection);

    //plugin finder
    void onScanPluginsStarted();
    void onScanPluginsFinished();
    void onPluginFounded(QString name, QString group, QString path);
    void onScanPluginsStarted(QString pluginPath);

    //input selection
    void on_inputSelectionChanged(int inputTrackIndex);

    //collapse local controls
    void on_localControlsCollapseButtonClicked();

    //channel name changed
    void on_channelNameChanged();

    //xmit
    void on_xmitButtonClicked(bool checked);

    //room streamer
    void on_RoomStreamerError(QString msg);

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

    Controller::MainController* mainController;
    PluginScanDialog* pluginScanDialog;
    Ui::MainFrameClass ui;
    QList<LocalTrackGroupView*> localChannels;

    NinjamRoomWindow* ninjamWindow;

    Login::RoomInfo* roomToJump;//store the next room reference when jumping from on room to another
    QString passwordToJump;

    void showPluginGui(Audio::Plugin* plugin);

    static bool jamRoomLessThan(Login::RoomInfo r1, Login::RoomInfo r2);

    void initializeWindowState();
    void initializeLoginService();
    void initializeLocalInputChannels();
    void restorePluginsList();
    void initializeVstFinderStuff();
    void initializeMainControllerEvents();
    void initializeMainTabWidget();

    QStringList getChannelsNames() const;

    LocalTrackGroupView* addLocalChannel(int channelGroupIndex, QString channelName, bool createFirstSubchannel);

};








