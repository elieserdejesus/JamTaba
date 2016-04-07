#include "gui/JamRoomViewPanel.h"
#include "ui_JamRoomViewPanel.h"

#include "MainController.h"
#include "ninjam/User.h"
#include "ninjam/UserChannel.h"

#include <QDebug>

JamRoomViewPanel::JamRoomViewPanel(const Login::RoomInfo &roomInfo,
                                   Controller::MainController *mainController) :
    QFrame(nullptr),
    ui(new Ui::RoomViewPanel),
    mainController(mainController),
    roomInfo(roomInfo)
{
    ui->setupUi(this);
    initialize(roomInfo);
    ui->wavePeakPanel->setEnabled(false);// is enable when user click in listen button
}

void JamRoomViewPanel::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange){
        translateUi();
    }
    QFrame::changeEvent(e);
}

void JamRoomViewPanel::translateUi()
{
    ui->labelRoomStatus->setText(buildRoomDescriptionString());
    ui->buttonEnter->setText(tr("enter"));
    ui->buttonListen->setText(tr("listen"));

    updateButtonListen();
}

QString JamRoomViewPanel::buildRoomDescriptionString()
{
    int botsCount = roomInfo.getUsers().count() - roomInfo.getNonBotUsersCount();
    int maxUsers = roomInfo.getMaxUsers() - botsCount;
    int users = roomInfo.getNonBotUsersCount();
    QString roomDescription = tr("%1 / %2 players ").arg(
                QString::number(users),
                QString::number(maxUsers));

    if (roomInfo.getBpm() > 0)
        roomDescription += "  " + QString::number(roomInfo.getBpm()) + " BPM ";
    if (roomInfo.getType() == Login::RoomTYPE::NINJAM && roomInfo.getBpi() > 0)
        roomDescription += "  " + QString::number(roomInfo.getBpi()) + " BPI";
    return roomDescription;
}

void JamRoomViewPanel::refresh(const Login::RoomInfo &roomInfo)
{
    this->roomInfo = roomInfo;

    ui->labelRoomStatus->setText(buildRoomDescriptionString());

    // remove all users labels from layout
    QList<QLabel *> allUserLabels = ui->usersPanel->findChildren<QLabel *>();
    foreach (QLabel *label, allUserLabels) {
        ui->usersPanel->layout()->removeWidget(label);
        label->deleteLater();
    }

    QList<Login::UserInfo> userInfos = roomInfo.getUsers();
    qSort(userInfos.begin(), userInfos.end(), userInfoLessThan);
    foreach (const Login::UserInfo &user, userInfos) {
        if (!userIsBot(user)) {
            QLabel *label = new QLabel(ui->usersPanel);
            label->setTextFormat(Qt::RichText);
            Geo::Location userLocation = mainController->getGeoLocation(user.getIp());
            QString userString = user.getName();
            QString imageString = "";
            if (!userLocation.isUnknown()) {
                userString += " <i>(" + userLocation.getCountryName() + ")</i>";
                QString countryCode = userLocation.getCountryCode().toLower();
                imageString = "<img src=:/flags/flags/" + countryCode +".png> ";
                label->setToolTip("");
            } else {
                imageString = "<img src=:/images/warning.png> ";
                QString toolTip = tr("%1  location is not available at moment!").arg(user.getName());
                label->setToolTip(toolTip);
            }
            label->setText(imageString + userString);

            ui->usersPanel->layout()->addWidget(label);
            ui->usersPanel->layout()->setAlignment(Qt::AlignTop);
        }
    }

    updateButtonListen();

    ui->buttonEnter->setEnabled(!roomInfo.isFull());
}

void JamRoomViewPanel::updateButtonListen()
{
    ui->buttonListen->setEnabled(roomInfo.hasStream() && !roomInfo.isEmpty());

    if (!roomInfo.hasStream()) {
        ui->buttonListen->setIcon(QIcon(":/images/warning.png"));
        ui->buttonListen->setToolTip(tr("The audio stream of this room is not available at moment!"));
    } else {
        ui->buttonListen->setIcon(QIcon());// remove the icon
        ui->buttonListen->setToolTip("");// clean the tooltip
    }

    style()->unpolish(ui->buttonListen);
    style()->polish(ui->buttonListen);
}

bool JamRoomViewPanel::userInfoLessThan(const Login::UserInfo &u1, const Login::UserInfo &u2)
{
    return u1.getName() < u2.getName();
}

void JamRoomViewPanel::addPeak(float peak)
{
    ui->wavePeakPanel->addPeak(peak);
}

void JamRoomViewPanel::setShowBufferingState(bool showBuffering)
{
    ui->wavePeakPanel->setShowBuffering(showBuffering);
}

void JamRoomViewPanel::setBufferingPercentage(int percentage)
{
    ui->wavePeakPanel->setBufferingPercentage(percentage);
}

bool JamRoomViewPanel::userIsBot(const Login::UserInfo &userInfo)
{
    return mainController->getBotNames().contains(userInfo.getName());
}

bool JamRoomViewPanel::roomContainsBotsOnly(const Login::RoomInfo &roomInfo)
{
    QStringList botsNames = mainController->getBotNames();
    foreach (const Login::UserInfo &user, roomInfo.getUsers()) {
        if (!botsNames.contains(user.getName()))
            return false;
    }
    return true;
}

void JamRoomViewPanel::initialize(const Login::RoomInfo &roomInfo)
{
    QString roomName = roomInfo.getName();
    if (roomInfo.getType() == Login::RoomTYPE::NINJAM)
        roomName += " (" + QString::number(roomInfo.getPort()) + ")";
    ui->labelName->setText(roomName);
    refresh(roomInfo);
}

JamRoomViewPanel::~JamRoomViewPanel()
{
    delete ui;
}

void JamRoomViewPanel::clear(bool resetListenButton)
{
    ui->wavePeakPanel->clearPeaks();
    ui->wavePeakPanel->setShowBuffering(false);
    if (resetListenButton)
        ui->buttonListen->setChecked(false);
    updateButtonListen();
}

void JamRoomViewPanel::on_buttonListen_clicked()
{
    if (ui->buttonListen->isChecked())
        emit startingListeningTheRoom(roomInfo);
    else
        emit finishingListeningTheRoom(roomInfo);
    ui->wavePeakPanel->setEnabled(ui->buttonListen->isChecked());
    ui->wavePeakPanel->updateGeometry();
}

void JamRoomViewPanel::on_buttonEnter_clicked()
{
    emit enteringInTheRoom(roomInfo);
}
