#ifndef MAINWINDOWVST_H
#define MAINWINDOWVST_H

#include "MainWindow.h"

class NinjamRoomWindow;

class MainWindowVST : public MainWindow
{
public:
    MainWindowVST(Controller::MainController* mainController);
protected:
    NinjamRoomWindow* createNinjamWindow(Login::RoomInfo, Controller::MainController *);
};

#endif // MAINWINDOWVST_H
