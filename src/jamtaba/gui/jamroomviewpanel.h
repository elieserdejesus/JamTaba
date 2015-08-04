#ifndef JAMROOMVIEWPANEL_H
#define JAMROOMVIEWPANEL_H

#include <QWidget>
#include "../ninjam/Server.h"
#include "../loginserver/LoginService.h"

namespace Ui { class RoomViewPanel; }

namespace Login{ class AbstractJamRoom; }

namespace Controller {
class MainController;
}

class JamRoomViewPanel : public QWidget
{
    Q_OBJECT

public:
    //JamRoomViewPanel(QWidget *parent, Controller::MainController* mainController);
    JamRoomViewPanel(Login::RoomInfo roomInfo, QWidget *parent, Controller::MainController* mainController);
    ~JamRoomViewPanel();
    void addPeak(float peak);
    void clearPeaks();
    void refreshUsersList(Login::RoomInfo roomInfo);
protected:
    void paintEvent( QPaintEvent* e );
signals:
    void startingListeningTheRoom(Login::RoomInfo roomInfo);
    void finishingListeningTheRoom(Login::RoomInfo roomInfo);
    void enteringInTheRoom(Login::RoomInfo roomInfo);
private slots:
    void on_buttonListen_clicked();
    void on_buttonEnter_clicked();

private:
    Ui::RoomViewPanel *ui;
    Controller::MainController* mainController;
    Login::RoomInfo roomInfo;
    void initialize(Login::RoomInfo roomInfo);
    bool roomContainsBotsOnly(Login::RoomInfo roomInfo);
    bool userIsBot(Login::UserInfo userInfo);

};

#endif // JAMROOMVIEWPANEL_H
