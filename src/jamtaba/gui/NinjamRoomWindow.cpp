#include "NinjamRoomWindow.h"
#include "ui_NinjamRoomWindow.h"
#include "NinjamTrackView.h"
#include "../MainController.h"
#include "../ninjam/User.h"
#include "../loginserver/JamRoom.h"
#include "../audio/NinjamTrackNode.h"
#include "../audio/MetronomeTrackNode.h"
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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

NinjamRoomWindow::NinjamRoomWindow(QWidget *parent, Ninjam::Server *server, Controller::MainController* mainController) :
    QWidget(parent),
    ui(new Ui::NinjamRoomWindow),
    server(server),
    mainController(mainController),
    metronomeTrackNode(new Audio::MetronomeTrackNode(":/click.wav")),
    newBpi(-1), newBpm(-1)

{
    ui->setupUi(this);

    QString roomName = server->getHostName() + ":" + QString::number(server->getPort());
    ui->labelRoomName->setText(roomName);

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

    this->samplesInInterval = computeTotalSamplesInInterval();

    //metronome
    this->metronomeTrackNode->setSamplesPerBeat(getSamplesPerBeat());
    mainController->addTrack(-1, this->metronomeTrackNode);


    Ninjam::Service* ninjamService = Ninjam::Service::getInstance();
    QObject::connect(ninjamService, SIGNAL(serverBpmChanged(short)), this, SLOT(ninjamServerBpmChanged(short)));
    QObject::connect(ninjamService, SIGNAL(serverBpiChanged(short,short)), this, SLOT(ninjamServerBpiChanged(short,short)));

    QObject::connect(mainController, SIGNAL(audioSamplesProcessed(int)), this, SLOT(audioSamplesProcessed(int)));

    QObject::connect(ui->topPanel->getBpiCombo(), SIGNAL(currentIndexChanged(int)), this, SLOT(ninjamBpiComboChanged(int)));
    QObject::connect(ui->topPanel->getBpmCombo(), SIGNAL(currentIndexChanged(int)), this, SLOT(ninjamBpmComboChanged(int)));
}

void NinjamRoomWindow::ninjamBpiComboChanged(int /*index*/){
    int newBpi = ui->topPanel->getBpiCombo()->currentData().toInt();
    QMessageBox::StandardButton reply;
    QString message = "Vote to change the BPI to " + QString::number(newBpi) + "?";
    reply = QMessageBox::question(this, "Changing BPI ...", message,
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        Ninjam::Service::getInstance()->voteToChangeBPI(newBpi);
    }
}

void NinjamRoomWindow::ninjamBpmComboChanged(int /*index*/){
    int newBpm = ui->topPanel->getBpmCombo()->currentData().toInt();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Changing BPM ...", "Vote to change the BPM to " + QString::number(newBpm) + "?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        Ninjam::Service::getInstance()->voteToChangeBPM(newBpm);
    }
}

NinjamRoomWindow::~NinjamRoomWindow()
{
    delete ui;
    delete metronomeTrackNode;
}

void NinjamRoomWindow::audioSamplesProcessed(int samplesProcessed){
    bool startingNewInterval = this->intervalPosition + samplesProcessed > samplesInInterval;
    if(startingNewInterval){
        processScheduledChanges();
    }
    this->intervalPosition = (this->intervalPosition + samplesProcessed) % samplesInInterval;
}

void NinjamRoomWindow::processScheduledChanges(){
    if(newBpi > 0){
        this->samplesInInterval = computeTotalSamplesInInterval();
        newBpi = -1;
    }
    if(newBpm > 0){
        this->metronomeTrackNode->setSamplesPerBeat(getSamplesPerBeat());
        newBpm = -1;
    }
}

long NinjamRoomWindow::getSamplesPerBeat(){
    return computeTotalSamplesInInterval()/server->getBpi();
}

long NinjamRoomWindow::computeTotalSamplesInInterval(){
    double intervalPeriod =  60000.0 / server->getBpm() * server->getBpi();
    int sampleRate = mainController->getAudioDriver()->getSampleRate();
    return (long)(sampleRate * intervalPeriod / 1000.0);
}


//ninjam events

void NinjamRoomWindow::ninjamServerBpiChanged(short /*oldBpi*/, short newBpi){
    //this->samplesInInterval = computeTotalSamplesInInterval();
    this->newBpi = newBpi;
}

void NinjamRoomWindow::ninjamServerBpmChanged(short newBpm){
    //this->metronomeTrackNode->setSamplesPerBeat(getSamplesPerBeat());
    this->newBpm = newBpm;
}
