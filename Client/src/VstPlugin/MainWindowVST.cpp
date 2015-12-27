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

//implementing the MainWindow methods
bool MainWindow::canCreateSubchannels() const{
    return false;
}

bool MainWindow::canUseFullScreen() const{
    return false;
}
//++++++++++++++++++++++++++++

void MainWindowVST::setFullViewStatus(bool fullViewActivated)
{
    MainWindow::setFullViewStatus(fullViewActivated);
    mainController->storeWindowSettings(isMaximized(), fullViewActivated, QPointF());
    dynamic_cast<MainControllerVST *>(mainController)->resizePluginEditor(width(), height());
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LocalTrackGroupView *MainWindowVST::addLocalChannel(int channelGroupIndex, QString channelName,
                                                    bool createFirstSubchannel,
                                                    bool initializeAsNoInput)
{
    Q_UNUSED(initializeAsNoInput)

    LocalTrackGroupView *newLocalChannel = MainWindow::addLocalChannel(channelGroupIndex,
                                                                       channelName,
                                                                       createFirstSubchannel,
                                                                       false);

    newLocalChannel->removeFxPanel(); // no fxPanel (plugins) in Vst Plugin
    newLocalChannel->removeInputSelectionControls(); // input selection (audio or midi) in Vst plugin

    return newLocalChannel;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindowVST::showPreferencesDialog(int initialTab){
    Q_UNUSED(initialTab)
    VstPreferencesDialog dialog(mainController, this);
    centerDialog(&dialog);
    dialog.exec();

}
