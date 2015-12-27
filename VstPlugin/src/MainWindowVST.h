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

    // overriding
    LocalTrackGroupView *addLocalChannel(int channelGroupIndex, QString channelName,
                                         bool createFirstSubchannel, bool initializeAsNoInput);
};

#endif // MAINWINDOWVST_H
