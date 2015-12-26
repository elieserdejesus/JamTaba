#include "MainWindowVST.h"
#include "NinjamRoomWindowVST.h"
#include "MainControllerVST.h"

MainWindowVST::MainWindowVST(Controller::MainController *mainController) :
    MainWindow(mainController)
{
}

NinjamRoomWindow *MainWindowVST::createNinjamWindow(Login::RoomInfo roomInfo,
                                                    Controller::MainController *mainController)
{
    return new NinjamRoomWindowVST(this, roomInfo, mainController);
}

void MainWindowVST::setFullViewStatus(bool fullViewActivated)
{
    MainWindow::setFullViewStatus(fullViewActivated);
    mainController->storeWindowSettings(isMaximized(), fullViewActivated, QPointF());
    dynamic_cast<MainControllerVST *>(mainController)->resizePluginEditor(width(), height());
}
