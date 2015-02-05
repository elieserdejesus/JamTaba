#include "jamroomviewpanel.h"
#include "ui_jamroomviewpanel.h"
#include "../loginserver/JamRoom.h"

#include <QPainter>
#include <QDebug>

JamRoomViewPanel::JamRoomViewPanel(QWidget *parent)
    :QWidget(parent)
{
    ui->labelRoomStatus->setText("");
}

JamRoomViewPanel::JamRoomViewPanel(Login::AbstractJamRoom* jamRoom, QWidget* parent)
    :QWidget(parent),
      ui(new Ui::RoomViewPanel),
      currentRoom(jamRoom)
{
    ui->setupUi(this);
    initialize();
}

void JamRoomViewPanel::paintEvent( QPaintEvent */*e*/ ){
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void JamRoomViewPanel::initialize(){
    ui->labelName->setText(currentRoom->getName());
    ui->labelRoomType->setText( currentRoom->getRoomType() == Login::AbstractJamRoom::Type::NINJAM ? "Ninjam" : "Realtime" );
    if(currentRoom->isEmpty()){
        ui->labelRoomStatus->setText( "Empty room!"  );
    }
    else if(currentRoom->isFull()){
        ui->labelRoomStatus->setText( "Crowded room!");
    }
    else{
        ui->labelRoomStatus->setText("");
    }

    foreach (Login::AbstractPeer* user, currentRoom->getPeers()) {
        if(!user->isBot()){
            QLabel* label = new QLabel(user->getName(), ui->usersPanel);
            label->setTextFormat(Qt::RichText);
            qDebug() << user->getCountryCode();
            QString country = user->getCountryCode().toLower();
            label->setText("<img src=:/flags/flags/" + country +".png> " + user->getName());
            ui->usersPanel->layout()->addWidget(label);
        }
    }


}

JamRoomViewPanel::~JamRoomViewPanel()
{
    delete ui;
}
