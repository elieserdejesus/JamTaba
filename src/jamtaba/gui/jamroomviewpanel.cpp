#include "JamRoomViewPanel.h"
#include "ui_jamroomviewpanel.h"
#include "../loginserver/JamRoom.h"
#include "../MainController.h"

#include <QPainter>
#include <QDebug>
#include <QLocale>


JamRoomViewPanel::JamRoomViewPanel(QWidget *parent, Controller::MainController* mainController)
    :QWidget(parent),
      ui(new Ui::RoomViewPanel),
      mainController(mainController),
      currentRoom(nullptr)
{
    ui->labelRoomStatus->setText("");
}

JamRoomViewPanel::JamRoomViewPanel(Login::AbstractJamRoom* jamRoom, QWidget* parent, Controller::MainController* mainController)
    :QWidget(parent),
      ui(new Ui::RoomViewPanel),
      mainController(mainController),
      currentRoom(jamRoom)
{
    ui->setupUi(this);
    initialize();
}

void JamRoomViewPanel::addPeak(float peak){
    ui->wavePeakPanel->addPeak(peak);
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
            QLabel* label = new QLabel(ui->usersPanel);
            label->setTextFormat(Qt::RichText);
            Geo::Location userLocation = mainController->getLocation(user->getIP());
            QString countryCode = userLocation.getCountryCode().toLower();
            QString countryName = userLocation.getCountryName();
            QString userString = user->getName() + " <i>(" + countryName + ")</i>";
            label->setText("<img src=:/flags/flags/" + countryCode +".png> " + userString);
            ui->usersPanel->layout()->addWidget(label);
        }
    }

    QString gMapURL = "England"; // this is where you want to point
    gMapURL = "http://maps.google.com.sg/maps?q="+gMapURL+"&oe=utf-8&rls=org.mozilla:en-US:official&client=firefox-a&um=1&ie=UTF-8&hl=en&sa=N&tab=wl";
    //ui->webView->setUrl(gMapURL);

    ui->buttonListen->setEnabled(currentRoom->hasStreamLink());
}

JamRoomViewPanel::~JamRoomViewPanel()
{
    delete ui;
}

void JamRoomViewPanel::clearPeaks(){
    ui->wavePeakPanel->clearPeaks();
}

void JamRoomViewPanel::on_buttonListen_clicked()
{
    if(currentRoom->hasStreamLink()){
        if(ui->buttonListen->isChecked()){
            emit startingListeningTheRoom(currentRoom);
        }
        else{
            emit finishingListeningTheRoom(currentRoom);
        }
    }
}

void JamRoomViewPanel::on_buttonEnter_clicked()
{
    if(currentRoom){
        emit enteringInTheRoom(currentRoom);
    }
}
