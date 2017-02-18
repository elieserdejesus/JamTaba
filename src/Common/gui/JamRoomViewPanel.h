#ifndef JAMROOMVIEWPANEL_H
#define JAMROOMVIEWPANEL_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include "ninjam/Server.h"
#include "loginserver/LoginService.h"
#include "WavePeakPanel.h"

class MapWidget;

namespace Ui {
class RoomViewPanel;
}

namespace Geo {
class Location;
}

namespace Login {
class AbstractJamRoom;
class RoomInfo;
}

namespace Controller {
class MainController;
}

class JamRoomViewPanel : public QFrame
{
    Q_OBJECT

public:
    JamRoomViewPanel(const Login::RoomInfo &roomInfo, Controller::MainController *mainController);
    ~JamRoomViewPanel();
    void addPeak(float peak);
    void clear(bool resetListenButton);
    void refresh(const Login::RoomInfo &roomInfo);

    void setShowBufferingState(bool showBuffering);
    void setBufferingPercentage(int percentage);
    void setWaveDrawingMode(WavePeakPanel::WaveDrawingMode mode);

    inline Login::RoomInfo getRoomInfo() const
    {
        return roomInfo;
    }

signals:
    void startingListeningTheRoom(const Login::RoomInfo &roomInfo);
    void finishingListeningTheRoom(const Login::RoomInfo &roomInfo);
    void enteringInTheRoom(const Login::RoomInfo &roomInfo);

protected:
    void changeEvent(QEvent *) override;

private slots:
    void toggleRoomListening();
    void enterInTheRoom();
    void updateUserLocation(const QString &userIP);

private:
    Ui::RoomViewPanel *ui;
    Controller::MainController *mainController;
    Login::RoomInfo roomInfo;
    MapWidget *map;

    QLayout *waveDrawingButtonsLayout;
    QMap<WavePeakPanel::WaveDrawingMode, QPushButton*> waveDrawingButtons;
    void createWaveDrawingButtonsLayout(QLayout *layout);
    void setWaveDrawingButtonsVisibility(bool showButtons);

     void initialize(const Login::RoomInfo &roomInfo);
     bool roomContainsBotsOnly(const Login::RoomInfo &roomInfo);
     bool userIsBot(const Login::UserInfo &userInfo);
     void updateButtonListen();

     static bool userInfoLessThan(const Login::UserInfo &u1, const Login::UserInfo &u2);
     QString buildRoomDescriptionString();

     void translateUi();
     void updateStyleSheet();
     void createMapWidgets();
     void updateMap();

     bool static canShowNinjamServerPort(const QString &serverName);
 };

 #endif // JAMROOMVIEWPANEL_H
