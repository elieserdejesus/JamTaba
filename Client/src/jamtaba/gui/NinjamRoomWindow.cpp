#include "NinjamRoomWindow.h"
#include "ui_NinjamRoomWindow.h"
#include "NinjamTrackView.h"

#include <QComboBox>
#include <QDebug>

#include "../ninjam/User.h"
#include "../ninjam/Server.h"

#include "../loginserver/JamRoom.h"

#include "../audio/NinjamTrackNode.h"
#include "../audio/MetronomeTrackNode.h"
#include "../audio/core/AudioDriver.h"

#include "../NinjamJamRoomController.h"
#include "../MainController.h"
#include "../ninjam/Service.h"

#include "NinjamPanel.h"
#include <QMessageBox>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

NinjamRoomWindow::NinjamRoomWindow(QWidget *parent, const Login::NinjamRoom& room, Controller::MainController *mainController) :
    QWidget(parent),
    ui(new Ui::NinjamRoomWindow),
    mainController(mainController)
{
    ui->setupUi(this);

    ui->licenceButton->setIcon(QIcon(QPixmap(":/images/licence.png")));

    //QString roomName = room.getHostName() + ":" + QString::number(room.getHostPort());
    ui->labelRoomName->setText(room.getName());

    ui->topPanel->setBpi(room.getCurrentBpi());
    ui->topPanel->setBpm(room.getCurrentBpm());

    ui->tracksPanel->layout()->setAlignment(Qt::AlignLeft);//tracks are left aligned

    Controller::NinjamJamRoomController* ninjamController = mainController->getNinjamController();
    QObject::connect(ninjamController, SIGNAL(currentBpiChanged(int)), this, SLOT(bpiChanged(int)));
    QObject::connect(ninjamController, SIGNAL(currentBpmChanged(int)), this, SLOT(bpmChanged(int)));
    QObject::connect(ninjamController, SIGNAL(intervalBeatChanged(int)), this, SLOT(intervalBeatChanged(int)));
    QObject::connect(ninjamController, SIGNAL(channelAdded(Ninjam::User,   Ninjam::UserChannel, long)), this, SLOT(channelAdded(  Ninjam::User, Ninjam::UserChannel, long)));
    QObject::connect(ninjamController, SIGNAL(channelRemoved(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(channelRemoved(Ninjam::User, Ninjam::UserChannel, long)));
    QObject::connect(ninjamController, SIGNAL(channelChanged(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(channelChanged(Ninjam::User, Ninjam::UserChannel, long)));

    QObject::connect(ui->topPanel->getBpiCombo(), SIGNAL(activated(QString)), this, SLOT(ninjamBpiComboChanged(QString)));
    QObject::connect(ui->topPanel->getBpmCombo(), SIGNAL(activated(QString)), this, SLOT(ninjamBpmComboChanged(QString)));
    QObject::connect(ui->topPanel->getAccentsCombo(), SIGNAL(currentIndexChanged(int)), this, SLOT(ninjamAccentsComboChanged(int)));

    QString serverLicence = Ninjam::Service::getInstance()->getCurrentServerLicence();
    ui->licenceButton->setVisible(!serverLicence.isEmpty());

    //testing many tracks
//    for (int t = 0; t < 16; ++t) {
//        BaseTrackView* trackView = new NinjamTrackView(ui->tracksPanel, this->mainController, t, "User", QString::number(t), "BR", "BR" );
//        ui->tracksPanel->layout()->addWidget(trackView);
//    }


}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::updatePeaks(){
    foreach (NinjamTrackView* view, tracks) {
        if(view){
            Audio::AudioPeak peak = mainController->getTrackPeak(view->getTrackID());
            view->setPeaks(peak.getLeft(), peak.getRight());
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::channelRemoved(Ninjam::User, Ninjam::UserChannel /*channel*/, long channelID){
    BaseTrackView* trackView = NinjamTrackView::getTrackViewByID(channelID);
    if(trackView){
        ui->tracksPanel->layout()->removeWidget(trackView);
        tracks.removeOne(dynamic_cast<NinjamTrackView*>(trackView));
        trackView->deleteLater();
    }
}

void NinjamRoomWindow::channelChanged(Ninjam::User, Ninjam::UserChannel channel, long channelID){
    NinjamTrackView* trackView = static_cast<NinjamTrackView*>(NinjamTrackView::getTrackViewByID(channelID));
    if(trackView){
        trackView->setChannelName(channel.getName());
        //trackView->setUserName(channel->getUser()->getName());
    }
}

void NinjamRoomWindow::channelAdded(Ninjam::User user, Ninjam::UserChannel channel, long channelID){
    QString userName = user.getName();
    QString channelName = channel.getName();
    Geo::Location userLocation = mainController->getLocation(user.getIp());
    QString countryName = userLocation.getCountryName();
    QString countryCode = userLocation.getCountryCode().toLower();
    NinjamTrackView* trackView = new NinjamTrackView(ui->tracksPanel, this->mainController, channelID, userName, channelName, countryName, countryCode );
    ui->tracksPanel->layout()->addWidget(trackView);
    tracks.append(trackView);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::ninjamAccentsComboChanged(int /*index*/){
    //qDebug() << ui->topPanel->getAccentsCombo()->currentData();
    int beatsPerAccent = ui->topPanel->getAccentsCombo()->currentData().toInt();
    mainController->getNinjamController()->setMetronomeBeatsPerAccent(beatsPerAccent);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::ninjamBpiComboChanged(QString newText){
    int currentBpi = mainController->getNinjamController()->getCurrentBpi();
    int newBpi = newText.toInt();
    if(newBpi == currentBpi){
        return;
    }
    QMessageBox::StandardButton reply;
    QString message = "Vote to change the BPI to " + QString::number(newBpi) + "?";
    reply = QMessageBox::question(this, "Changing BPI ...", message,
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        mainController->getNinjamController()->voteBpi(newBpi);
    }
    else{
        ui->topPanel->getBpiCombo()->blockSignals(true);
        ui->topPanel->getBpiCombo()->setCurrentText(QString::number(currentBpi));
        ui->topPanel->getBpiCombo()->blockSignals(false);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::ninjamBpmComboChanged(QString newText){
    int currentBpm = mainController->getNinjamController()->getCurrentBpm();
    int newBpm = newText.toInt();
    if(newBpm == currentBpm){
        return;
    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Changing BPM ...", "Vote to change the BPM to " + QString::number(newBpm) + "?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        mainController->getNinjamController()->voteBpm(newBpm);
    }
    else{
        ui->topPanel->getBpmCombo()->blockSignals(true);
        ui->topPanel->getBpmCombo()->setCurrentText(QString::number(currentBpm));
        ui->topPanel->getBpmCombo()->blockSignals(false);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NinjamRoomWindow::~NinjamRoomWindow()
{
    delete ui;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//ninjam controller events
void NinjamRoomWindow::bpiChanged(int bpi){
    ui->topPanel->setBpi(bpi);
}

void NinjamRoomWindow::bpmChanged(int bpm){
    ui->topPanel->setBpm(bpm);
}

void NinjamRoomWindow::intervalBeatChanged(int beat){
    ui->topPanel->setCurrentBeat(beat);
}

void NinjamRoomWindow::on_licenceButton_clicked()
{
    QString licence = Ninjam::Service::getInstance()->getCurrentServerLicence();
    QMessageBox::information(this, ui->labelRoomName->text(), licence, QMessageBox::NoButton, QMessageBox::NoButton);
}
