#ifndef JAMROOMVIEWPANEL_H
#define JAMROOMVIEWPANEL_H

#include <QWidget>
#include "ninjam/Server.h"
#include "loginserver/LoginService.h"

namespace Ui {
class RoomViewPanel;
}

namespace Login {
class AbstractJamRoom;
}

namespace Controller {
class MainController;
}

class JamRoomViewPanel : public QWidget
{
    Q_OBJECT

public:
    JamRoomViewPanel(const Login::RoomInfo &roomInfo, Controller::MainController *mainController);
    ~JamRoomViewPanel();
    void addPeak(float peak);
    void clearPeaks(bool resetListenButton);
    void refresh(const Login::RoomInfo &roomInfo);

    inline Login::RoomInfo getRoomInfo() const
    {
        return roomInfo;
    }

protected:
    void paintEvent(QPaintEvent *e);
signals:
    void startingListeningTheRoom(const Login::RoomInfo &roomInfo);
    void finishingListeningTheRoom(const Login::RoomInfo &roomInfo);
    void enteringInTheRoom(const Login::RoomInfo &roomInfo);
private slots:
    void on_buttonListen_clicked();
    void on_buttonEnter_clicked();
private:
    Ui::RoomViewPanel *ui;
    Controller::MainController *mainController;
    Login::RoomInfo roomInfo;
    void initialize(const Login::RoomInfo &roomInfo);
    bool roomContainsBotsOnly(const Login::RoomInfo &roomInfo);
    bool userIsBot(const Login::UserInfo &userInfo);

    static bool userInfoLessThan(const Login::UserInfo &u1, const Login::UserInfo &u2);
    QString buildRoomDescriptionString(const Login::RoomInfo &roomInfo);
};

#endif // JAMROOMVIEWPANEL_H
