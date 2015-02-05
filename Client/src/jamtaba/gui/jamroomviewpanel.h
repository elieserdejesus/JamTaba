#ifndef JAMROOMVIEWPANEL_H
#define JAMROOMVIEWPANEL_H

#include <QWidget>

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
