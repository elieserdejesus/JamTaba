#ifndef JAMROOMVIEWPANEL_H
#define JAMROOMVIEWPANEL_H

#include <QWidget>
#include <map>
#include <string>

//static std::map<std::string, std::string> countriesMap;

namespace Ui { class RoomViewPanel; }

namespace Login{ class AbstractJamRoom; }

class JamRoomViewPanel : public QWidget
{
    Q_OBJECT

public:
    JamRoomViewPanel(QWidget *parent = 0);
    JamRoomViewPanel(Login::AbstractJamRoom *jamRoom, QWidget *parent = 0);
    ~JamRoomViewPanel();
    void paintEvent( QPaintEvent */*e*/ );

private:
    Ui::RoomViewPanel *ui;
    Login::AbstractJamRoom* currentRoom;
    void initialize();
};

#endif // JAMROOMVIEWPANEL_H
