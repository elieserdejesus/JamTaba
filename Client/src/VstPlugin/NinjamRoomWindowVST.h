#ifndef NINJAMROOMWINDOWVST_H
#define NINJAMROOMWINDOWVST_H

#include "gui/NinjamRoomWindow.h"

class MainControllerVST;

class NinjamRoomWindowVST : public NinjamRoomWindow
{
    Q_OBJECT
public:
    NinjamRoomWindowVST(MainWindow *parent, Login::RoomInfo roomInfo, MainControllerVST *mainController);

private slots:
    void ninjamHostSyncButtonClicked();

private:
    void showMessageBox(QString title, QString msg);
};

#endif // NINJAMROOMWINDOWVST_H
