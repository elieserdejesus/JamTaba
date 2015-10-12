#include "MainWindowStandalone.h"
#include "../StandAloneMainController.h"

MainWindowStandalone::MainWindowStandalone(Controller::MainController* controller)
    :MainWindow(controller){


}

void MainWindowStandalone::initializePluginFinder(){
    MainWindow::initializePluginFinder();

    QStringList vstPaths = mainController->getSettings().getVstPluginsPaths();
    if(vstPaths.empty()){//no vsts in database cache, try scan
        if(mainController->getSettings().getVstScanPaths().isEmpty()){
            (dynamic_cast<Controller::StandaloneMainController*>(mainController))->addDefaultVstScanPath();
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
