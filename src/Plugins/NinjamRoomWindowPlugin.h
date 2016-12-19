#ifndef NINJAMROOMWINDOWVST_H
#define NINJAMROOMWINDOWVST_H

#include "gui/NinjamRoomWindow.h"

class MainControllerPlugin;

class NinjamRoomWindowPlugin : public NinjamRoomWindow
{
    Q_OBJECT
public:
    NinjamRoomWindowPlugin(MainWindow *parent, const Login::RoomInfo &roomInfo, MainControllerPlugin *mainController);

private slots:
    void setHostSyncState(bool syncWithHost);
    void disableHostSync();
private:
    void showMessageBox(const QString &title, const QString &msg);
    MainControllerPlugin *controller;
};

#endif // NINJAMROOMWINDOWVST_H
