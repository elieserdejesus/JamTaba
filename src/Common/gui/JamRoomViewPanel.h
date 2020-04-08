#ifndef JAMROOMVIEWPANEL_H
#define JAMROOMVIEWPANEL_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>

#include "gui/widgets/WavePeakPanel.h"

#include "ninjam/client/ServerInfo.h"
#include "loginserver/LoginService.h"

class MapWidget;

namespace Ui {
class JamRoomViewPanel;
}

namespace geo {
class Location;
}

namespace login {
class AbstractJamRoom;
class RoomInfo;
}

namespace controller {
class MainController;
}

class JamRoomViewPanel : public QFrame
{
    Q_OBJECT

public:
    JamRoomViewPanel(const login::RoomInfo &roomInfo, controller::MainController *mainController);
    ~JamRoomViewPanel();
    void addPeak(float peak);
    void clear(bool resetListenButton);
    void refresh(const login::RoomInfo &roomInfo);

    void setShowBufferingState(bool showBuffering);
    void setBufferingPercentage(int percentage);
    void setWaveDrawingMode(WavePeakPanel::WaveDrawingMode mode);

    login::RoomInfo getRoomInfo() const;

signals:
    void startingListeningTheRoom(const login::RoomInfo &roomInfo);
    void finishingListeningTheRoom(const login::RoomInfo &roomInfo);
    void enteringInTheRoom(const login::RoomInfo &roomInfo);

protected:
    void changeEvent(QEvent *) override;

private slots:
    void toggleRoomListening();
    void enterInTheRoom();
    void updateUserLocation(const QString &userIP);

private:
    Ui::JamRoomViewPanel *ui;
    controller::MainController *mainController;
    login::RoomInfo roomInfo;
    MapWidget *map;

    QLayout *waveDrawingButtonsLayout;
    QMap<WavePeakPanel::WaveDrawingMode, QPushButton*> waveDrawingButtons;
    void createWaveDrawingButtonsLayout(QLayout *layout);
    void setWaveDrawingButtonsVisibility(bool showButtons);

    void initialize(const login::RoomInfo &roomInfo);
    bool roomContainsBotsOnly(const login::RoomInfo &roomInfo);
    bool userIsBot(const login::UserInfo &userInfo);
    void updateButtonListen();

    static bool userInfoLessThan(const login::UserInfo &u1, const login::UserInfo &u2);
    QString buildRoomDescriptionString();

    void translateUi();
    void updateStyleSheet();
    void createMapWidgets();
    void updateMap();
};

inline login::RoomInfo JamRoomViewPanel::getRoomInfo() const
{
    return roomInfo;
}

 #endif // JAMROOMVIEWPANEL_H
