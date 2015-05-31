#include "NinjamRoomWindow.h"
#include "ui_NinjamRoomWindow.h"
#include "NinjamTrackView.h"
#include "../MainController.h"

NinjamRoomWindow::NinjamRoomWindow(QWidget *parent, Login::NinjamRoom* room, Controller::MainController* mainController) :
    QWidget(parent),
    ui(new Ui::NinjamRoomWindow)
{
    ui->setupUi(this);

    ui->labelRoomName->setText(room->getName());
    QList<AbstractPeer*> peers = room->getPeers();
    foreach (AbstractPeer* peer, peers) {
        int trackID = mainController->addTrack(*peer);
        BaseTrackView* trackView = new NinjamTrackView(ui->tracksPanel, mainController, trackID);
        ui->tracksPanel->layout()->addWidget(trackView);
    }
    ui->tracksPanel->layout()->setAlignment(ui->tracksPanel->layout(), Qt::AlignLeft);
}

NinjamRoomWindow::~NinjamRoomWindow()
{
    delete ui;
}
