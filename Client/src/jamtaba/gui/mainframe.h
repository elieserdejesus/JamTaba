#pragma once

#include <QList>
#include <QtWidgets/QMainWindow>
#include "ui_mainframe.h"
#include <QMap>
#include "pluginscandialog.h"

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

private slots:

    void on_preferences_triggered();
    void on_IOPropertiesChanged(int midiDevice, int audioDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);
    void on_connectedInServer(QList<Login::AbstractJamRoom*>);

    //+++++  ROOM STREAMS ++++++++
    void on_startingRoomStream(Login::AbstractJamRoom* room);
    void on_stoppingRoomStream(Login::AbstractJamRoom* room);

    //fx MENU
    void on_fxMenuActionTriggered(QAction*);
    void on_editingPlugin(Audio::Plugin* plugin);//emited by LocalTrackView
    void on_removingPlugin(Audio::Plugin* plugin);//emit by LocalTrackView

    //plugin finder
    void onPluginScanStarted();
    void onPluginScanFinished();
    void onPluginFounded(Audio::PluginDescriptor* descriptor);
    void on_pushButton_clicked();

private:

    int timerID;

    QMap<Login::AbstractJamRoom*, JamRoomViewPanel*> roomViewPanels;

    QMenu* createFxMenu();

    PluginScanDialog* pluginScanDialog;

    LocalTrackView* localTrackView;

    QMenu* fxMenu;
    Controller::MainController* mainController;

    Ui::MainFrameClass ui;

    //PluginGui* createPluginView(Plugin::PluginDescriptor *, Audio::Plugin *plugin) ;

    void showPluginGui(Audio::Plugin* plugin);
};








