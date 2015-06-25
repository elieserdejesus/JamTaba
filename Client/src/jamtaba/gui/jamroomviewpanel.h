#ifndef JAMROOMVIEWPANEL_H
#define JAMROOMVIEWPANEL_H

#include <QWidget>

namespace Ui { class RoomViewPanel; }

namespace Login{ class AbstractJamRoom; }

namespace Controller {
class MainController;
}

class JamRoomViewPanel : public QWidget
{
    Q_OBJECT

public:
    JamRoomViewPanel(QWidget *parent, Controller::MainController* mainController);
    JamRoomViewPanel(Login::AbstractJamRoom *jamRoom, QWidget *parent, Controller::MainController* mainController);
    ~JamRoomViewPanel();
    void paintEvent( QPaintEvent */*e*/ );
    void addPeak(float peak);
    void clearPeaks();
signals:
    void startingListeningTheRoom(Login::AbstractJamRoom* room);
    void finishingListeningTheRoom(Login::AbstractJamRoom* room);
    void enteringInTheRoom(Login::AbstractJamRoom* room);
private slots:
    void on_buttonListen_clicked();
    void on_buttonEnter_clicked();

private:
    Ui::RoomViewPanel *ui;
    Controller::MainController* mainController;
    Login::AbstractJamRoom* currentRoom;
    void initialize();
};

#endif // JAMROOMVIEWPANEL_H
