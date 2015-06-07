#include "NinjamRoomWindow.h"
#include "ui_NinjamRoomWindow.h"
#include "NinjamTrackView.h"
#include "../MainController.h"
#include "../ninjam/User.h"
#include "../loginserver/JamRoom.h"
#include "../audio/NinjamTrackNode.h"
#include "../audio/MetronomeTrackNode.h"
#include "../NinjamJamRoomController.h"
#include "../audio/core/AudioDriver.h"
#include "../ninjam/Service.h"
#include "NinjamPanel.h"
#include <QMessageBox>

class NinjamTrackInfos{

public:
    NinjamTrackInfos(Ninjam::User* user, Ninjam::UserChannel* channel)
        :user(user), channel(channel), id(ID_GENERATOR++)
    {

    }

    long getID() const{return id;}
    Ninjam::User* getUser() const{return user;}
    Ninjam::UserChannel* getChannel() const{return channel;}

private:
    Ninjam::User* user;
    Ninjam::UserChannel* channel;

    long id;
    static long ID_GENERATOR;// = 0;
};

long NinjamTrackInfos::ID_GENERATOR = 0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

NinjamRoomWindow::NinjamRoomWindow(QWidget *parent, Ninjam::Server *server, Controller::MainController *mainController) :
    QWidget(parent),
    ui(new Ui::NinjamRoomWindow),
    ninjamController(mainController->getNinjamController())
{
    ui->setupUi(this);

    QString roomName = server->getHostName() + ":" + QString::number(server->getPort());
    ui->labelRoomName->setText(roomName);

    ui->topPanel->setBpi(server->getBpi());
    ui->topPanel->setBpm(server->getBpm());

    QObject::connect(ninjamController, SIGNAL(currentBpiChanged(int)), this, SLOT(bpiChanged(int)));
    QObject::connect(ninjamController, SIGNAL(currentBpmChanged(int)), this, SLOT(bpmChanged(int)));
    QObject::connect(ninjamController, SIGNAL(intervalBeatChanged(int)), this, SLOT(intervalBeatChanged(int)));

    QList<Ninjam::User*> users = server->getUsers();
    ui->tracksPanel->layout()->setAlignment(Qt::AlignLeft);
    foreach (Ninjam::User* user, users) {
        if(!user->isBot()){
            Login::NinjamPeer* ninjamPeer = (NinjamPeer*)AbstractPeer::getByIP(user->getIp());
            if(ninjamPeer){
                QSet<Ninjam::UserChannel*> channels = user->getChannels();
                foreach (Ninjam::UserChannel* channel, channels) {
                    NinjamTrackInfos trackInfos(user, channel);
                    mainController->addTrack( trackInfos.getID(), new NinjamTrackNode() );
                    BaseTrackView* trackView = new NinjamTrackView(ui->tracksPanel, mainController, trackInfos.getID(), channel->getName(), ninjamPeer);
                    ui->tracksPanel->layout()->addWidget(trackView);
                }
            }
            else{
                qWarning() << "Não encontrou o ninjamPeer para " << user->getFullName();
            }
        }
    }

    QObject::connect(ui->topPanel->getBpiCombo(), SIGNAL(activated(QString)), this, SLOT(ninjamBpiComboChanged(QString)));
    QObject::connect(ui->topPanel->getBpmCombo(), SIGNAL(activated(QString)), this, SLOT(ninjamBpmComboChanged(QString)));
    QObject::connect(ui->topPanel->getAccentsCombo(), SIGNAL(currentIndexChanged(int)), this, SLOT(ninjamAccentsComboChanged(int)));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::ninjamAccentsComboChanged(int /*index*/){
    //qDebug() << ui->topPanel->getAccentsCombo()->currentData();
    int beatsPerAccent = ui->topPanel->getAccentsCombo()->currentData().toInt();
    ninjamController->setMetronomeBeatsPerAccent(beatsPerAccent);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::ninjamBpiComboChanged(QString newText){
    int currentBpi = ninjamController->getCurrentBpi();
    int newBpi = newText.toInt();
    if(newBpi == currentBpi){
        return;
    }
    QMessageBox::StandardButton reply;
    QString message = "Vote to change the BPI to " + QString::number(newBpi) + "?";
    reply = QMessageBox::question(this, "Changing BPI ...", message,
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        Ninjam::Service::getInstance()->voteToChangeBPI(newBpi);
    }
    else{
        ui->topPanel->getBpiCombo()->blockSignals(true);
        ui->topPanel->getBpiCombo()->setCurrentText(QString::number(currentBpi));
        ui->topPanel->getBpiCombo()->blockSignals(false);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::ninjamBpmComboChanged(QString newText){
    int currentBpm = ninjamController->getCurrentBpm();
    int newBpm = newText.toInt();
    if(newBpm == currentBpm){
        return;
    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Changing BPM ...", "Vote to change the BPM to " + QString::number(newBpm) + "?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        Ninjam::Service::getInstance()->voteToChangeBPM(newBpm);
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
