#include "gui/JamRoomViewPanel.h"
#include "ui_JamRoomViewPanel.h"

#include <QDebug>
#include "MainController.h"
#include "ninjam/User.h"
#include "ninjam/UserChannel.h"
#include "ninjam/Server.h"
#include "MapWidget.h"
#include "MapMarker.h"

JamRoomViewPanel::JamRoomViewPanel(const Login::RoomInfo &roomInfo,
                                   Controller::MainController *mainController) :
    QFrame(nullptr),
    ui(new Ui::RoomViewPanel),
    mainController(mainController),
    roomInfo(roomInfo)
{
    ui->setupUi(this);

    ui->wavePeakPanel->setEnabled(false);// is enable when user click in listen button

    connect(mainController, &Controller::MainController::ipResolved, this, &JamRoomViewPanel::updateUserLocation);
    connect(ui->buttonListen, &QPushButton::clicked, this, &JamRoomViewPanel::toggleRoomListening);
    connect(ui->buttonEnter, &QPushButton::clicked, this, &JamRoomViewPanel::enterInTheRoom);

    createMapWidgets();

    // add wave peak panel as a layer in top of map widget
    ui->content->layout()->removeWidget(ui->wavePeakPanel);
    map->setLayout(new QHBoxLayout());
    map->layout()->addWidget(ui->wavePeakPanel);

    initialize(roomInfo);
}

void JamRoomViewPanel::createMapWidgets()
{
    map = new MapWidget(this);
    ui->content->layout()->addWidget(map);
}

void JamRoomViewPanel::updateUserLocation(const QString &userIP)
{
    qDebug() << "updating user location IP:" << userIP;

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

void JamRoomViewPanel::updateMap()
{
    if (!roomInfo.isEmpty()) {
        QList<Login::UserInfo> userInfos = roomInfo.getUsers();
        qSort(userInfos.begin(), userInfos.end(), userInfoLessThan);
        QList<MapMarker> newMarkers;
        foreach (const Login::UserInfo &user, userInfos) {
            if (!userIsBot(user)) {
                Geo::Location userLocation = mainController->getGeoLocation(user.getIp());
                if (userLocation.isUnknown())
                    continue; // skip invalid locations

                QPointF latLong(userLocation.getLatitude(), userLocation.getLongitude());
                QPixmap flag(":/flags/flags/" + userLocation.getCountryCode().toLower() + ".png");
                MapMarker marker(user.getName(), userLocation.getCountryName(), latLong, flag.toImage());
                newMarkers.append(marker);
            }
        }

        map->setMarkers(newMarkers);
    }

    map->setVisible(!roomInfo.isEmpty());
    map->update();
}

void JamRoomViewPanel::refresh(const Login::RoomInfo &roomInfo)
{
    this->roomInfo = roomInfo;

    ui->labelRoomStatus->setText(buildRoomDescriptionString());

    updateButtonListen();

    ui->buttonEnter->setEnabled(!roomInfo.isFull());

    updateMap();
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
    bool listening = ui->buttonListen->isChecked();
    if (listening)
        emit startingListeningTheRoom(roomInfo);
    else
        emit finishingListeningTheRoom(roomInfo);

    ui->wavePeakPanel->setEnabled(listening);
    ui->wavePeakPanel->updateGeometry();
    map->setMarkersVisibility(!listening);
}

void JamRoomViewPanel::enterInTheRoom()
{
    emit enteringInTheRoom(roomInfo);
}
