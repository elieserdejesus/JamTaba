#ifndef NINJAMROOMWINDOW_H
#define NINJAMROOMWINDOW_H

#include <QWidget>
#include "../loginserver/JamRoom.h"
#include "../MainController.h"

#include <QSpacerItem>

using namespace Login;

namespace Ui {
class NinjamRoomWindow;
}

class NinjamRoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NinjamRoomWindow(QWidget *parent, Login::NinjamRoom* room, Controller::MainController *mainController);
    ~NinjamRoomWindow();

private:
    Ui::NinjamRoomWindow *ui;
};

#endif // NINJAMROOMWINDOW_H
