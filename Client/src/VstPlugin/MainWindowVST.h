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

    void initializeLocalSubChannel(LocalTrackView *subChannelView, Persistence::Subchannel subChannel) override;

    void removeAllInputLocalTracks() override;

private:
    bool firstChannelIsInitialized;

};

#endif // MAINWINDOWVST_H
