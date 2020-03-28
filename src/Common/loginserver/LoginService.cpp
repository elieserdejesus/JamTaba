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

UserInfo::UserInfo(long long id, const QString &name, const QString &ip) :
    id(id),
    name(name),
    ip(ip)
{
    //
}

RoomInfo::RoomInfo(long long id, const QString &roomName, int roomPort,
                   int maxUsers, const QList<UserInfo> &users, int maxChannels, int bpi, int bpm, const QString &streamUrl) :
    id(id),
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
    RoomInfo(-1000, roomName, roomPort, maxUsers, QList<login::UserInfo>(), maxChannels, 0, 0, "")
{
        //
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
}

void LoginService::handleJson(const QString &json)
{
    if (json.isEmpty())
        return;

    QJsonDocument document = QJsonDocument::fromJson(QByteArray(json.toStdString().c_str()));
    QJsonObject root = document.object();

    if (!root.contains("rooms"))
        return;

    QJsonArray allRooms = root["rooms"].toArray();
    QList<RoomInfo> publicRooms;
    for (int i = 0; i < allRooms.size(); ++i) {
        QJsonObject jsonObject = allRooms[i].toObject();
        auto roomInfo = buildRoomInfoFromJson(jsonObject);
        publicRooms.append(roomInfo);

        QString lastChordProgression = "" ; //store an empty chord progression by default
        if (jsonObject.contains("lastChordProgression"))
            lastChordProgression = jsonObject["lastChordProgression"].toString();

    }
    emit roomsListAvailable(publicRooms);
}

RoomInfo LoginService::buildRoomInfoFromJson(const QJsonObject &jsonObject)
{
    long long id = jsonObject.value("id").toVariant().toLongLong();

    QString name = jsonObject["name"].toString();
    int port = jsonObject["port"].toInt();
    int maxUsers = jsonObject["maxUsers"].toInt();
    QString streamLink = jsonObject["streamUrl"].toString();
    int bpi = jsonObject["bpi"].toInt();
    int bpm = jsonObject["bpm"].toInt();

    QJsonArray usersArray = jsonObject["users"].toArray();
    QList<UserInfo> users;
    for (int i = 0; i < usersArray.size(); ++i) {
        QJsonObject userObject = usersArray[i].toObject();
        long long userID = userObject.value("id").toVariant().toLongLong();
        QString userName = userObject.value("name").toString();
        QString userIp = userObject.value("ip").toString();
        users.append(login::UserInfo(userID, userName, userIp));
    }
    return RoomInfo(id, name, port, maxUsers, users, 0, bpi, bpm, streamLink);
}
