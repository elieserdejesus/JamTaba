#include "gui/JamRoomViewPanel.h"
#include "ui_JamRoomViewPanel.h"

#include "MainController.h"
#include "ninjam/User.h"
#include "ninjam/UserChannel.h"
#include "PlayerLabel.h"
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

    connect(mainController, &Controller::MainController::ipResolved, this, &JamRoomViewPanel::updateUserLocation);
    connect(ui->buttonListen, &QPushButton::clicked, this, &JamRoomViewPanel::toggleRoomListening);
    connect(ui->buttonEnter, &QPushButton::clicked, this, &JamRoomViewPanel::enterInTheRoom);
}

void JamRoomViewPanel::updateUserLocation(const QString &userIP)
{
    QList<PlayerLabel *> playerLabels = ui->usersPanel->findChildren<PlayerLabel *>();
    foreach (PlayerLabel *label, playerLabels) {
        if (label->getUserIP() == userIP) {
            Geo::Location userLocation = mainController->getGeoLocation(userIP);
            label->setLocation(userLocation);
            break;
        }
    }
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
            Geo::Location userLocation = mainController->getGeoLocation(user.getIp());
            QLabel *label = new PlayerLabel(ui->usersPanel, user, userLocation);
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

void JamRoomViewPanel::toggleRoomListening()
{
    if (ui->buttonListen->isChecked())
        emit startingListeningTheRoom(roomInfo);
    else
        emit finishingListeningTheRoom(roomInfo);
    ui->wavePeakPanel->setEnabled(ui->buttonListen->isChecked());
    ui->wavePeakPanel->updateGeometry();
}

void JamRoomViewPanel::enterInTheRoom()
{
    emit enteringInTheRoom(roomInfo);
}
