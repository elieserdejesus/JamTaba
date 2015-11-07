#include "../gui/jamroomviewpanel.h"
#include "ui_jamroomviewpanel.h"
#include "../MainController.h"
#include "../ninjam/User.h"
#include <QPainter>
#include <QDebug>


JamRoomViewPanel::JamRoomViewPanel(Login::RoomInfo roomInfo, QWidget* parent, Controller::MainController* mainController)
    :QWidget(parent),
      ui(new Ui::RoomViewPanel),
      mainController(mainController),
      roomInfo(roomInfo)
{
    ui->setupUi(this);
    initialize(roomInfo);

    ui->wavePeakPanel->setEnabled(false);//is enable when user click in listen button
}


void JamRoomViewPanel::refreshUsersList(Login::RoomInfo roomInfo){
    this->roomInfo = roomInfo;

    if(roomInfo.isEmpty() || roomContainsBotsOnly(roomInfo)){
        ui->labelRoomStatus->setText( "Empty room!"  );
    }
    else if(roomInfo.isFull() ){
        ui->labelRoomStatus->setText( "Crowded room!");
    }
    else{
        ui->labelRoomStatus->setText("");
    }

    //remove all users labels from layout
    QList<QLabel*> allUserLabels = ui->usersPanel->findChildren<QLabel*>();
    foreach (QLabel* label, allUserLabels) {
        ui->usersPanel->layout()->removeWidget(label);
        label->deleteLater();
    }

    QList<Login::UserInfo> userInfos = roomInfo.getUsers();
    qSort(userInfos.begin(), userInfos.end(), userInfoLessThan);
    foreach (Login::UserInfo user, userInfos) {
        if(!userIsBot(user)){
            QLabel* label = new QLabel(ui->usersPanel);
            label->setTextFormat(Qt::RichText);
            Geo::Location userLocation = mainController->getGeoLocation(user.getIp());
            QString userString = user.getName();
            QString imageString = "";
            if(!userLocation.isUnknown()){
                userString += " <i>(" + userLocation.getCountryName() + ")</i>";
                QString countryCode = userLocation.getCountryCode().toLower();
                imageString = "<img src=:/flags/flags/" + countryCode +".png> ";
                label->setToolTip("");
            }
            else{
                imageString = "<img src=:/images/warning.png> ";
                label->setToolTip(user.getName() + " location is not available at moment!");
            }
            label->setText(imageString + userString);

            ui->usersPanel->layout()->addWidget(label);
            ui->usersPanel->layout()->setAlignment(Qt::AlignTop);
        }
    }

//    QString gMapURL = "England"; // this is where you want to point
//    gMapURL = "http://maps.google.com.sg/maps?q="+gMapURL+"&oe=utf-8&rls=org.mozilla:en-US:official&client=firefox-a&um=1&ie=UTF-8&hl=en&sa=N&tab=wl";
//    //ui->webView->setUrl(gMapURL);

    ui->buttonListen->setEnabled(roomInfo.hasStream() && !roomInfo.isEmpty());
    if(!roomInfo.hasStream()){
        ui->buttonListen->setIcon(QIcon(":/images/warning.png"));
        ui->buttonListen->setToolTip("The audio stream of this room is not available at moment!");
    }
    else{
        ui->buttonListen->setIcon(QIcon());//remove the icon
        ui->buttonListen->setToolTip("");//clean the tooltip
    }
    ui->buttonEnter->setEnabled(!roomInfo.isFull());
}

bool JamRoomViewPanel::userInfoLessThan(Login::UserInfo u1, Login::UserInfo u2){
    return u1.getName() < u2.getName();
}

void JamRoomViewPanel::addPeak(float peak){
    ui->wavePeakPanel->addPeak(peak);
}

void JamRoomViewPanel::paintEvent( QPaintEvent *e ){
    Q_UNUSED(e)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool JamRoomViewPanel::userIsBot(Login::UserInfo userInfo){
    return mainController->getBotNames().contains(userInfo.getName());
}

bool JamRoomViewPanel::roomContainsBotsOnly(Login::RoomInfo roomInfo){
    QStringList botsNames = mainController->getBotNames();
    foreach (Login::UserInfo user, roomInfo.getUsers()) {
        if(!botsNames.contains(user.getName())){
            return false;
        }
    }
    return true;
}

void JamRoomViewPanel::initialize(Login::RoomInfo roomInfo){
    QString roomName = roomInfo.getName();
    if(roomInfo.getType() == Login::RoomTYPE::NINJAM){
        roomName += " (" + QString::number(roomInfo.getPort()) + ")";
    }
    ui->labelName->setText( roomName );
    //ui->labelRoomType->setText( (roomInfo.getType() == Login::RoomTYPE::NINJAM) ? "Ninjam" : "RealTime");

    refreshUsersList(roomInfo);
}

JamRoomViewPanel::~JamRoomViewPanel()
{
    delete ui;
}

void JamRoomViewPanel::clearPeaks(bool resetListenButton){
    ui->wavePeakPanel->clearPeaks();
    if(resetListenButton){
        ui->buttonListen->setChecked(false);
    }
}

void JamRoomViewPanel::on_buttonListen_clicked(){
    if(ui->buttonListen->isChecked()){
        emit startingListeningTheRoom(roomInfo);
    }
    else{
        emit finishingListeningTheRoom(roomInfo);
    }
    ui->wavePeakPanel->setEnabled(ui->buttonListen->isChecked());
    ui->wavePeakPanel->updateGeometry();
}

void JamRoomViewPanel::on_buttonEnter_clicked(){
    emit enteringInTheRoom(roomInfo);
}
