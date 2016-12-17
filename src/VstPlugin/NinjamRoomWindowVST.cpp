#include "NinjamRoomWindowVST.h"
#include "ui_NinjamRoomWindow.h"
#include "MainControllerVST.h"
#include "NinjamControllerVST.h"
#include <QToolTip>

using namespace Controller;

// +++++++++++++++++++++++++++++++++++++++++++++
NinjamRoomWindowVST::NinjamRoomWindowVST(MainWindow *parent, const Login::RoomInfo &roomInfo,
                                         MainControllerVST *mainController) :
    NinjamRoomWindow(parent, roomInfo, mainController),
    controller(mainController)
{
    QString hostName = mainController->getHostName();
    if (ninjamPanel) {
        //: The '%1' marker will be replaced by the host name when Jamtaba is running.
        QString text = tr("Sync with %1").arg(hostName);
        ninjamPanel->createHostSyncButton(text);
        connect(ninjamPanel, &NinjamPanel::hostSyncStateChanged, this, &NinjamRoomWindowVST::setHostSyncState);
    }

    // if server bpm change the 'sync with host' is disabled
    connect(controller->getNinjamController(), &NinjamController::currentBpmChanged, this, &NinjamRoomWindowVST::disableHostSync);
}

void NinjamRoomWindowVST::disableHostSync()
{
    if (ninjamPanel->hostSyncButtonIsChecked()) {
        setHostSyncState(false);
        ninjamPanel->uncheckHostSyncButton();

        QString hostName = controller->getHostName();
        QString newBpm = QString::number(controller->getNinjamController()->getCurrentBpm());

        //: This is the title of MessageDialot showed when user click in "Sync with host" button
        QString title = tr("Host sync");

        QString message = tr("The BPM has changed! Please stop %1 and change BPM to %2!").arg(hostName, newBpm);
        showMessageBox(title, message);
    }
}

// activate/deactivate sync with host
void NinjamRoomWindowVST::setHostSyncState(bool syncWithHost)
{
    Q_ASSERT(ninjamPanel);
    NinjamControllerVST *ninjamController = controller->getNinjamController();
    if (syncWithHost) {
        int ninjamBpm = ninjamController->getCurrentBpm();
        int hostBpm = controller->getHostBpm();
        QString hostName = controller->getHostName();
        if (hostBpm == ninjamBpm) {
            ninjamController->stopAndWaitForHostSync();// stop ninjam streams and wait until user press play/start in host
            ninjamPanel->setCurrentBeat(0);
            QString title = tr("Synchronizing...");
            QString message = tr("Press play/start in %1 to sync with Jamtaba!").arg(hostName);
            showMessageBox(title, message);
        } else {
            QString title = tr("Trying to sync ...");
            QString message = tr("Change %1 BPM to %2 and try sync again!").arg(hostName, ninjamBpm);
            showMessageBox(title, message);
            ninjamPanel->uncheckHostSyncButton();//the button is unchecked, so user can try again
        }
    } else {
        ninjamController->disableHostSync();
    }
}

void NinjamRoomWindowVST::showMessageBox(const QString &title, const QString &msg)
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
