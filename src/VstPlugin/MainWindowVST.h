#ifndef MAINWINDOWVST_H
#define MAINWINDOWVST_H

#include "gui/MainWindow.h"
#include "NinjamRoomWindowVST.h"
#include "MainControllerVST.h"

class NinjamRoomWindow;

class MainWindowVST : public MainWindow
{
public:
    MainWindowVST(MainControllerVST *mainController);

    inline MainControllerVST *getMainController() override
    {
        return dynamic_cast<MainControllerVST *>(mainController);
    }

protected:
    NinjamRoomWindowVST *createNinjamWindow(const Login::RoomInfo &, Controller::MainController *) override;
    void setFullViewStatus(bool fullViewActivated);

    void showPreferencesDialog(int initialTab) override;

    void initializeLocalSubChannel(LocalTrackView *subChannelView, const Persistence::Subchannel &subChannel) override;

    void removeAllInputLocalTracks() override;

private:
    bool firstChannelIsInitialized;

};

#endif // MAINWINDOWVST_H
