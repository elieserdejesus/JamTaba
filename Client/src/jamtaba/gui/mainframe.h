#pragma once
#include <QList>
#include <QtWidgets/QMainWindow>
#include "ui_mainframe.h"
#include <QMap>
#include "pluginscandialog.h"
#include "BusyDialog.h"

namespace Controller{
    class MainController;
}
namespace Ui{
    class MainFrameClass;
    class MainFrame;
}

namespace Login {
    class LoginServiceParser;
    class AbstractJamRoom;
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
class LocalTrackView;
class MetronomeTrackView;

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

private slots:

    void on_preferencesClicked();
    void on_IOPropertiesChanged(int midiDevice, int audioDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);
    void on_connectedInServer(QList<Login::AbstractJamRoom*>);

    //+++++  ROOM FEATURES ++++++++
    void on_startingRoomStream(Login::AbstractJamRoom* room);
    void on_stoppingRoomStream(Login::AbstractJamRoom* room);
    void on_enteringInRoom(Login::AbstractJamRoom* room);
    void on_enteredInRoom(Login::AbstractJamRoom* room);
    void on_exitedFromRoom(bool normalDisconnection);
    //

    //fx MENU
    void on_fxMenuActionTriggered(QAction*);
    void on_editingPlugin(Audio::Plugin* plugin);//emited by LocalTrackView
    void on_removingPlugin(Audio::Plugin* plugin);//emit by LocalTrackView

    //plugin finder
    void onPluginScanStarted();
    void onPluginScanFinished();
    void onPluginFounded(Audio::PluginDescriptor* descriptor);


private:

    BusyDialog busyDialog;

    void showBusyDialog(QString message);
    void showBusyDialog();
    void hideBusyDialog();
    void centerBusyDialog();

    int timerID;

    QMap<Login::AbstractJamRoom*, JamRoomViewPanel*> roomViewPanels;

    QMenu* createFxMenu();
    QMenu* fxMenu;
    Controller::MainController* mainController;
    PluginScanDialog* pluginScanDialog;
    Ui::MainFrameClass ui;
    LocalTrackView* localTrackView;
    MetronomeTrackView* metronomeTrackView;

    //PluginGui* createPluginView(Plugin::PluginDescriptor *, Audio::Plugin *plugin) ;

    void showPluginGui(Audio::Plugin* plugin);
};








