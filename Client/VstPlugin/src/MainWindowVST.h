#ifndef MAINWINDOWVST_H
#define MAINWINDOWVST_H

#include "gui/MainWindow.h"

class NinjamRoomWindow;

class MainWindowVST : public MainWindow
{
public:
    MainWindowVST(Controller::MainController *mainController);
protected:
    NinjamRoomWindow *createNinjamWindow(Login::RoomInfo, Controller::MainController *);
    void setFullViewStatus(bool fullViewActivated);
};

#endif // MAINWINDOWVST_H
