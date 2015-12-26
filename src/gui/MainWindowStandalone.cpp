#include "MainWindowStandalone.h"
#include "../StandAloneMainController.h"
#include <QTimer>
#include "NinjamRoomWindow.h"

MainWindowStandalone::MainWindowStandalone(Controller::MainController* controller)
    :MainWindow(controller){

    initializePluginFinder();
    QTimer::singleShot(50, this, &MainWindowStandalone::restorePluginsList);
}

NinjamRoomWindow* MainWindowStandalone::createNinjamWindow(Login::RoomInfo roomInfo, Controller::MainController* mainController){
    return new NinjamRoomWindow(this, roomInfo, mainController);
}

void MainWindowStandalone::closeEvent(QCloseEvent * e){
    MainWindow::closeEvent(e);
    hide();//hide before stop main controller and disconnect from login server

    foreach (LocalTrackGroupView* trackGroup, localGroupChannels) {
        trackGroup->closePluginsWindows();
    }
}

void MainWindowStandalone::showEvent(QShowEvent *ent){
    Q_UNUSED(ent)
//TODO restorePluginsList(bool fromSettings)
    //wait 50 ms before restore the plugins list to avoid freeze the GUI in hidden state while plugins are loading
    //QTimer::singleShot(50, this, &MainWindowStandalone::restorePluginsList);
}

void MainWindowStandalone::initializePluginFinder()
{
    MainWindow::initializePluginFinder();

    Controller::StandaloneMainController* controller = dynamic_cast<Controller::StandaloneMainController*>(mainController);
    const Persistence::Settings settings = controller->getSettings();

    controller->initializePluginsList(settings.getVstPluginsPaths());//load the cached plugins. The cache can be empty.

    //checking for new plugins...
    if (controller->pluginsScanIsNeeded() )//no vsts in database cache or new plugins detected in scan folders?
    {
        if (settings.getVstScanFolders().isEmpty())
        {
            controller->addDefaultPluginsScanPath();
        }
        controller->saveLastUserSettings(getInputsSettings());//save config file before scan
        controller->scanPlugins(true);
    }
}

void MainWindowStandalone::on_errorConnectingToServer(QString msg){
    MainWindow::on_errorConnectingToServer(msg);
    (dynamic_cast<Controller::StandaloneMainController*>(mainController))->quit();
}
