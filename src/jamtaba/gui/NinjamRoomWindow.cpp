#include "NinjamRoomWindow.h"
#include "ui_NinjamRoomWindow.h"
#include "NinjamTrackView.h"
#include "../MainController.h"
#include "../ninjam/User.h"
#include "../loginserver/JamRoom.h"
#include "../audio/NinjamTrackNode.h"
#include "../audio/MetronomeTrackNode.h"

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

NinjamRoomWindow::NinjamRoomWindow(QWidget *parent, Ninjam::Server *server, Controller::MainController* mainController) :
    QWidget(parent),
    ui(new Ui::NinjamRoomWindow)
{
    ui->setupUi(this);

    ui->labelRoomName->setText(server->getHostName() + ":" + QString::number(server->getPort()));

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

    //metronome
    mainController->addTrack(-1, new Audio::MetronomeTrackNode(":/click.wav"));

}

NinjamRoomWindow::~NinjamRoomWindow()
{
    delete ui;
}
