#include "NinjamRoomWindowVST.h"
#include "ui_NinjamRoomWindow.h"
#include "MainControllerVST.h"


//+++++++++++++++++++++++++++++++++++++++++++++
NinjamRoomWindowVST::NinjamRoomWindowVST(MainWindow *parent, Login::RoomInfo roomInfo, Controller::MainController *mainController)
    :NinjamRoomWindow(parent, roomInfo, mainController){

    if(mainController->isRunningAsVstPlugin()){//just in case
        ui->topPanel->createHostSyncButton("Sync with host");
        QObject::connect(ui->topPanel, SIGNAL(hostSyncButtonClicked()), this, SLOT(ninjamHostSyncButtonClicked()));
    }
}

void NinjamRoomWindowVST::ninjamHostSyncButtonClicked(){
    if(mainController->isRunningAsVstPlugin()){//just in case...
        MainControllerVST* controller = dynamic_cast<MainControllerVST*>(mainController);
        int ninjamBpm = controller->getNinjamController()->getCurrentBpm();
        bool canSync = controller->getHostBpm() == ninjamBpm;
        if(canSync){
            dynamic_cast<NinjamControllerVST*>(controller->getNinjamController())->waitForHostSync();//stop ninjam streams and wait until user press play/start in host
            ui->topPanel->setCurrentBeat(0);
            showMessageBox("Synchronizing...", "Press play/start in your host to sync with Jamtaba!");
        }
        else{
            showMessageBox("Trying to sync ...", "Change your host BPM to " + QString::number(ninjamBpm) + " and try sync again!");
        }
    }
}

void NinjamRoomWindowVST::showMessageBox(QString title, QString msg){
    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setWindowTitle(title);
    msgBox->setText(msg);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->show();

    QPoint globalPosition = mapToGlobal( this->pos());
    int x = globalPosition.x() + width()/2 - msgBox->width()/2;
    int y = globalPosition.y() + height()/2 - msgBox->height()/2;
    msgBox->move(x,y);
}



