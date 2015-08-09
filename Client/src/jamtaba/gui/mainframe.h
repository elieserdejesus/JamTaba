#pragma once

#include <QtWidgets/QMainWindow>
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
private slots:
    void on_tabCloseRequest(int index);
    void on_preferencesClicked(QAction *action);
    void on_IOPreferencesChanged(QList<bool>, int audioDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);
    void on_roomsListAvailable(QList<Login::RoomInfo> publicRooms);

    //+++++  ROOM FEATURES ++++++++
    void on_startingRoomStream(Login::RoomInfo roomInfo);
    void on_stoppingRoomStream(Login::RoomInfo roomInfo);
    void on_enteringInRoom(Login::RoomInfo roomInfo);
    void on_enteredInRoom(Login::RoomInfo roomInfo);
    void on_exitedFromRoom(bool normalDisconnection);
    //

    //plugin finder
    void onPluginScanStarted();
    void onPluginScanFinished();
    void onPluginFounded(QString name, QString group, QString path);

    //input selection
    void on_inputSelectionChanged(int inputTrackIndex);

    //add/remove channels
    void on_toolButtonClicked();//show the channels menu
    void on_addChannelClicked();//add a new channel (a group of subchannels)
    void on_toolButtonMenuActionTriggered(QAction*);
    void on_toolButtonMenuActionHovered(QAction*);//highlight the hovered channel

    //channel name changed
    void on_channelNameChanged();

    //xmit
    void on_xmitButtonClicked(bool checked);

private:

    BusyDialog busyDialog;

    void showBusyDialog(QString message);
    void showBusyDialog();
    void hideBusyDialog();
    void centerBusyDialog();

    int timerID;

    QPointF computeLocation() const;

    QMap<long long, JamRoomViewPanel*> roomViewPanels;

    Controller::MainController* mainController;
    PluginScanDialog* pluginScanDialog;
    Ui::MainFrameClass ui;
    QList<LocalTrackGroupView*> localChannels;
    //MetronomeTrackView* metronomeTrackView;
    NinjamRoomWindow* ninjamWindow;



    void showPluginGui(Audio::Plugin* plugin);

    static bool jamRoomLessThan(Login::RoomInfo r1, Login::RoomInfo r2);

    void initializeWindowState();
    void initializeLoginService();
    void initializeLocalInputChannels();
    void initializeVstFinderStuff();
    void initializeMainControllerEvents();
    void initializeMainTabWidget();

    QStringList getChannelsNames() const;

    LocalTrackGroupView* addLocalChannel(int channelGroupIndex, QString channelName, bool createFirstSubchannel);
    void removeLocalChannel(int channelGroupIndex);
};








