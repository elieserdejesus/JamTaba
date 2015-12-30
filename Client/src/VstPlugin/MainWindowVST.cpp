#include "MainWindowVST.h"
#include "NinjamRoomWindowVST.h"
#include "MainControllerVST.h"
#include "VstPreferencesDialog.h"

MainWindowVST::MainWindowVST(Controller::MainController *mainController) :
    MainWindow(mainController)
{
    this->ui.actionVstPreferences->setVisible(false);
    this->ui.actionAudioPreferences->setVisible(false);
    this->ui.actionMidiPreferences->setVisible(false);
    this->ui.actionQuit->setVisible(false);
    this->ui.actionFullscreenMode->setVisible(false);
}

NinjamRoomWindow *MainWindowVST::createNinjamWindow(Login::RoomInfo roomInfo,
                                                    Controller::MainController *mainController)
{
    return new NinjamRoomWindowVST(this, roomInfo, mainController);
}

// ++++++++++++++++++++++++++++

void MainWindowVST::setFullViewStatus(bool fullViewActivated)
{
    MainWindow::setFullViewStatus(fullViewActivated);
    mainController->storeWindowSettings(isMaximized(), fullViewActivated, QPointF());
    dynamic_cast<MainControllerVST *>(mainController)->resizePluginEditor(width(), height());
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindowVST::showPreferencesDialog(int initialTab)
{
    Q_UNUSED(initialTab)
    VstPreferencesDialog dialog(mainController, this);
    centerDialog(&dialog);
    dialog.exec();
}
