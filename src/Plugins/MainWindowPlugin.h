#ifndef MAINWINDOWVST_H
#define MAINWINDOWVST_H

#include "gui/MainWindow.h"
#include "NinjamRoomWindowPlugin.h"
#include "MainControllerPlugin.h"

class NinjamRoomWindowPlugin;

class MainWindowPlugin : public MainWindow
{
public:
    MainWindowPlugin(MainControllerPlugin *mainController);

    inline MainControllerPlugin *getMainController() override
    {
        return dynamic_cast<MainControllerPlugin *>(mainController);
    }

protected:
    NinjamRoomWindow *createNinjamWindow(const Login::RoomInfo &, Controller::MainController *) override;

    void setFullViewStatus(bool fullViewActivated);

    void initializeLocalSubChannel(LocalTrackView *subChannelView, const Persistence::Subchannel &subChannel) override;

    void removeAllInputLocalTracks() override;

    PreferencesDialog * createPreferencesDialog() override;
private:
    bool firstChannelIsInitialized;

};

#endif // MAINWINDOWVST_H
