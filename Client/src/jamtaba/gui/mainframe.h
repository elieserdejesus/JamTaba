#pragma once

#include <QList>
#include <QtWidgets/QMainWindow>
#include "ui_mainframe.h"
#include <QMap>

//namespace Controller{
    class MainController;
//}
namespace Ui{
    class MainFrameClass;
    class MainFrame;
}

namespace Login {
    class LoginServiceParser;
    class AbstractJamRoom;
}

class JamRoomViewPanel;

class MainFrame : public QMainWindow
{
    Q_OBJECT

public:
    MainFrame(MainController* mainController, QWidget *parent = 0);
    ~MainFrame();
    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent*);
    virtual void changeEvent(QEvent *);
    virtual void timerEvent(QTimerEvent *);

private slots:
    void on_actionAudio_triggered();
    void on_audioIOPropertiesChanged(int selectedDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);
    void on_connectedInServer(QList<Login::AbstractJamRoom*>);

    //+++++  ROOM STREAMS ++++++++
    void on_startingRoomStream(Login::AbstractJamRoom* room);
    void on_stoppingRoomStream(Login::AbstractJamRoom* room);
private:
    Ui::MainFrameClass ui;
    MainController* mainController;
    int timerID;

    QMap<Login::AbstractJamRoom*, JamRoomViewPanel*> roomViewPanels;

};








