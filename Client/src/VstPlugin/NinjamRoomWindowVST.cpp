#include "NinjamRoomWindowVST.h"
#include "ui_NinjamRoomWindow.h"
#include "MainControllerVST.h"
#include "NinjamControllerVST.h"

// +++++++++++++++++++++++++++++++++++++++++++++
NinjamRoomWindowVST::NinjamRoomWindowVST(MainWindow *parent, Login::RoomInfo roomInfo,
                                         MainControllerVST *mainController) :
    NinjamRoomWindow(parent, roomInfo, mainController),
    controller(mainController)
{
    QString hostName = mainController->getHostName();
    if (ninjamPanel) {
        ninjamPanel->createHostSyncButton("Sync with " + hostName);
        QObject::connect(ninjamPanel, SIGNAL(hostSyncButtonClicked()), this,
                         SLOT(ninjamHostSyncButtonClicked()));
    }
}

void NinjamRoomWindowVST::ninjamHostSyncButtonClicked()
{
    int ninjamBpm = controller->getNinjamController()->getCurrentBpm();
    bool canSync = controller->getHostBpm() == ninjamBpm;
    QString hostName = controller->getHostName();
    if (canSync) {
        // stop ninjam streams and wait until user press play/start in host
        NinjamControllerVST* ninjamController = controller->getNinjamController();
        Q_ASSERT(ninjamController);
        ninjamController->waitForHostSync();
        if (ninjamPanel) {
            ninjamPanel->setCurrentBeat(0);
            showMessageBox("Synchronizing...",
                           "Press play/start in " + hostName + " to sync with Jamtaba!");
        }
    } else {
        showMessageBox("Trying to sync ...", "Change " + hostName + " BPM to " + QString::number(
                           ninjamBpm) + " and try sync again!");
    }
}

void NinjamRoomWindowVST::showMessageBox(QString title, QString msg)
{
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setWindowTitle(title);
    msgBox->setText(msg);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->show();

    QPoint globalPosition = mapToGlobal(this->pos());
    int x = globalPosition.x() + width()/2 - msgBox->width()/2;
    int y = globalPosition.y() + height()/2 - msgBox->height()/2;
    msgBox->move(x, y);
}
