#include "LoginService.h"
#include "natmap.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QDebug>
#include <QApplication>
#include "ninjam/client/ServerInfo.h"
#include "ninjam/client/Service.h"
#include "log/Logging.h"

using login::LoginService;
using login::UserInfo;
using login::RoomInfo;

const QString LoginService::LOGIN_SERVER_URL = "http://ninbot.com/app/servers.php";

UserInfo::UserInfo(const QString &name, const QString &ip) :
    name(name),
    ip(ip)
{
    //
}

RoomInfo::RoomInfo(const QString &roomName, int roomPort,
                   int maxUsers, const QList<UserInfo> &users, int maxChannels, int bpi, int bpm, const QString &streamUrl) :
    name(roomName),
    port(roomPort),
    maxUsers(maxUsers),
    maxChannels(maxChannels),
    users(users),
    bpi(bpi),
    bpm(bpm),
    streamUrl(streamUrl)
{
    //
}

RoomInfo::RoomInfo(const QString &roomName, int roomPort, int maxUsers, int maxChannels) :
    RoomInfo(roomName, roomPort, maxUsers, QList<login::UserInfo>(), maxChannels, 0, 0, "")
{
        //
}

QString RoomInfo::getUniqueName() const {
    return QString("%1:%2")
            .arg(getName())
            .arg(QString::number(getPort()));
}

int RoomInfo::getNonBotUsersCount() const
{
    int nonBots = 0;
    for (const UserInfo &userInfo : users) {
        if (!ninjam::client::Service::isBotName(userInfo.getName()))
            nonBots++;
    }
    return nonBots;
}

bool RoomInfo::isEmpty() const
{
    return getNonBotUsersCount() == 0;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++

LoginService::LoginService(QObject *parent) :
    QObject(parent),
    httpClient(this),
    refreshTimer(new QTimer(this))
{
    connect(refreshTimer, &QTimer::timeout, this, [=](){
        httpClient.get(QNetworkRequest(QUrl(LOGIN_SERVER_URL)));
    });

    connect(&httpClient, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){
        handleJson(reply->readAll());
    });

    refreshTimer->start(REFRESH_PERIOD);

    // the first public servers list query
    httpClient.get(QNetworkRequest(QUrl(LOGIN_SERVER_URL)));
}

void LoginService::handleJson(const QString &json)
{
    if (json.isEmpty())
        return;

    QJsonDocument document = QJsonDocument::fromJson(QByteArray(json.toStdString().c_str()));
    QJsonObject root = document.object();

    if (!root.contains("servers"))
        return;

    QJsonArray allRooms = root["servers"].toArray();
    QList<RoomInfo> publicRooms;
    for (int i = 0; i < allRooms.size(); ++i) {
        QJsonObject jsonObject = allRooms[i].toObject();
        auto roomInfo = buildRoomInfoFromJson(jsonObject);
        publicRooms.append(roomInfo);
    }
    emit roomsListAvailable(publicRooms);
}

int getServerPort(const QString &serverName) {
    auto port = 2049;
    auto index = serverName.lastIndexOf(":");

    if (index) {
        return serverName.right(serverName.size() - (index + 1)).toInt();
    }

    return port;
}

QString getServerName(const QString &serverText) {
    auto index = serverText.lastIndexOf(":");

    if (index) {
        return serverText.left(index);
    }

    return serverText;
}

int getServerGuessedMaxUsers(const QString &serverName, int serverPort) {
    if (serverName.contains("discordonlinejammingcentral"))
        return 10;

    if (serverName.contains("ninbot") && serverPort == 2051)
        return 16;

    if (serverName.contains("musicorner") || serverName.contains("cockos"))
        return 4;

    return 8;
}

RoomInfo LoginService::buildRoomInfoFromJson(const QJsonObject &jsonObject)
{
    auto serverNameText = jsonObject.contains("name") ? jsonObject["name"].toString() : QString("Error");
    QString name =  getServerName(serverNameText);
    int port = getServerPort(serverNameText);
    int maxUsers = jsonObject.contains("maxUsers") ? jsonObject["maxUsers"].toInt() : getServerGuessedMaxUsers(name, port);
    QString streamLink = jsonObject.contains("stream") ? jsonObject["stream"].toString() : QString("");
    int bpi = jsonObject.contains("bpi") ? jsonObject["bpi"].toInt() : 16;
    int bpm = jsonObject.contains("bpm") ? jsonObject["bpm"].toInt() : 120;

    QJsonArray usersArray = jsonObject.contains("users") ? jsonObject["users"].toArray() : QJsonArray();
    QList<UserInfo> users;
    for (int i = 0; i < usersArray.size(); ++i) {
        QJsonObject userObject = usersArray[i].toObject();
        QString userName = userObject.contains("name") ? userObject.value("name").toString() : QString("Error");
        QString userIp = userObject.contains("ip") ? userObject.value("ip").toString() : QString("Error");
        users.append(login::UserInfo(userName, userIp));
    }
    return RoomInfo(name, port, maxUsers, users, 0, bpi, bpm, streamLink);
}
