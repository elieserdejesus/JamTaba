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

    LocalTrackGroupView *addLocalChannel(int channelGroupIndex, QString channelName,
                                         bool createFirstSubchannel, bool initializeAsNoInput) override;

    void showPreferencesDialog(int initialTab) override;

    void initializeSubChannel(Persistence::Subchannel subChannel, LocalTrackView* subChannelView) override; //TODO the code to loading channels (load preset) and to initialize local inputs is the same, but is almost duplicated at moment.
};

#endif // MAINWINDOWVST_H
