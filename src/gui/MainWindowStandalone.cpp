#include "MainWindowStandalone.h"
#include "../StandAloneMainController.h"
#include <QTimer>

MainWindowStandalone::MainWindowStandalone(Controller::MainController* controller)
    :MainWindow(controller){


}

void MainWindowStandalone::showEvent(QShowEvent *){

    if(!mainController->isStarted()){//first show?
        int availableDevices = mainController->getAudioDriver()->getDevicesCount();
        if(availableDevices > 0){

            //mainController->start();

            //wait 50 ms before restore the plugins list to avoid freeze the GUI in hidden state while plugins are loaded
            QTimer::singleShot(50, this, &MainWindowStandalone::restorePluginsList);
        }
        else{
            QMessageBox::critical(this, "ERROR", "No audio device!");
            close();
        }
    }
}

void MainWindowStandalone::initializePluginFinder(){
    MainWindow::initializePluginFinder();

    QStringList vstPaths = mainController->getSettings().getVstPluginsPaths();
    if(vstPaths.empty()){//no vsts in database cache, try scan
        if(mainController->getSettings().getVstScanPaths().isEmpty()){
            (dynamic_cast<Controller::StandaloneMainController*>(mainController))->addDefaultPluginsScanPath();
        }
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
