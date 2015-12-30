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

    void showPreferencesDialog(int initialTab) override;

};

#endif // MAINWINDOWVST_H
