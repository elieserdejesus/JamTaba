#ifndef NINJAMROOMWINDOWVST_H
#define NINJAMROOMWINDOWVST_H

#include "gui/NinjamRoomWindow.h"

class MainControllerVST;

class NinjamRoomWindowVST : public NinjamRoomWindow
{
    Q_OBJECT
public:
    NinjamRoomWindowVST(MainWindow *parent, const Login::RoomInfo &roomInfo, MainControllerVST *mainController);

private slots:
    void setHostSyncState(bool syncWithHost);
    void disableHostSync();
private:
    void showMessageBox(const QString &title, const QString &msg);
    MainControllerVST *controller;
};

#endif // NINJAMROOMWINDOWVST_H
