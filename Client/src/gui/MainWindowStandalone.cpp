#include "MainWindowStandalone.h"
#include "../StandAloneMainController.h"
#include <QTimer>
#include "NinjamRoomWindow.h"

MainWindowStandalone::MainWindowStandalone(Controller::MainController* controller)
    :MainWindow(controller){

    initializePluginFinder();
}

NinjamRoomWindow* MainWindowStandalone::createNinjamWindow(Login::RoomInfo roomInfo, Controller::MainController* mainController){
    return new NinjamRoomWindow(this, roomInfo, mainController);
}

void MainWindowStandalone::closeEvent(QCloseEvent * e){
    MainWindow::closeEvent(e);
    hide();//hide before stop main controller and disconnect from login server to

    foreach (LocalTrackGroupView* trackGroup, localChannels) {
        trackGroup->closePluginsWindows();
    }
}

void MainWindowStandalone::showEvent(QShowEvent *){
    //wait 50 ms before restore the plugins list to avoid freeze the GUI in hidden state while plugins are loaded
    QTimer::singleShot(50, this, &MainWindowStandalone::restorePluginsList);
}

void MainWindowStandalone::initializePluginFinder(){
    MainWindow::initializePluginFinder();

    QStringList vstPaths = mainController->getSettings().getVstPluginsPaths();
    if(vstPaths.empty()){//no vsts in database cache, try scan
        if(mainController->getSettings().getVstScanPaths().isEmpty()){
            (dynamic_cast<Controller::StandaloneMainController*>(mainController))->addDefaultPluginsScanPath();
        }
        mainController->saveLastUserSettings(getInputsSettings());//save config file before scan
        (dynamic_cast<Controller::StandaloneMainController*>(mainController))->scanPlugins();
    }
    else{//use vsts stored in settings file
        (dynamic_cast<Controller::StandaloneMainController*>(mainController))->initializePluginsList(vstPaths);
        onScanPluginsFinished();
    }
}

void MainWindowStandalone::on_errorConnectingToServer(QString msg){
    MainWindow::on_errorConnectingToServer(msg);
    (dynamic_cast<Controller::StandaloneMainController*>(mainController))->quit();
}
