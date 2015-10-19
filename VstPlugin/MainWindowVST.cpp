#include "MainWindowVST.h"
#include "NinjamRoomWindowVST.h"

MainWindowVST::MainWindowVST(Controller::MainController *mainController)
    :MainWindow(mainController){

}

NinjamRoomWindow* MainWindowVST::createNinjamWindow(Login::RoomInfo roomInfo, Controller::MainController *mainController){
    return new NinjamRoomWindowVST(this, roomInfo, mainController);
}
