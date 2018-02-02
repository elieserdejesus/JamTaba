#include "LoginService.h"
#include "natmap.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QTimer>
#include <QDebug>
#include <QApplication>
#include "ninjam/client/Server.h"
#include "ninjam/client/Service.h"
#include "log/Logging.h"

using namespace login;

const QString LoginService::LOGIN_SERVER_URL = "http://jamtaba2.appspot.com/vs";
//const QString LoginService::LOGIN_SERVER_URL = "http://localhost:8080/vs";

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

UserInfo::UserInfo(long long id, const QString &name, const QString &ip) :
    id(id),
    name(name),
    ip(ip)
{
    //
}

RoomInfo::RoomInfo(long long id, const QString &roomName, int roomPort, RoomTYPE roomType,
                   int maxUsers, const QList<UserInfo> &users, int maxChannels, int bpi, int bpm, const QString &streamUrl) :
    id(id),
    name(roomName),
    port(roomPort),
    type(roomType),
    maxUsers(maxUsers),
    maxChannels(maxChannels),
    users(users),
    bpi(bpi),
    bpm(bpm),
    streamUrl(streamUrl)
{
    //
}

RoomInfo::RoomInfo(const QString &roomName, int roomPort, RoomTYPE roomType, int maxUsers, int maxChannels) :
    RoomInfo(-1000, roomName, roomPort, roomType, maxUsers, QList<login::UserInfo>(), maxChannels, 0, 0, "")
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

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class HttpParamsFactory
{

public:
    static QUrlQuery createParametersToConnect(QString userName, int instrumentID,
                                               QString channelName, NatMap localPeerMap,
                                               QString version, QString environment, int sampleRate)
    {
        QUrlQuery query;
        query.addQueryItem("cmd", "CONNECT");
        query.addQueryItem("userName", userName);
        query.addQueryItem("instrumentID", QString::number(instrumentID));// used in real time rooms
        query.addQueryItem("channelName", channelName);// use in real time room, need update to allow more channels
        query.addQueryItem("publicIp", localPeerMap.getPublicIp());// used in real time rooms
        query.addQueryItem("publicPort", QString::number(localPeerMap.getPublicPort())); // real time rooms only
        query.addQueryItem("environment", environment);
        query.addQueryItem("version", version);
        query.addQueryItem("sampleRate", QString::number(sampleRate));
        return query;
    }

    static QUrlQuery createParametersToUpdateLastChordProgression(const QString &userName,
                                               const QString &serverName, quint32 serverPort, const QString &chordsProgression)
    {
        QUrlQuery query;
        query.addQueryItem("cmd", "UPDATE_CHORDS");
        query.addQueryItem("userName", userName);
        query.addQueryItem("serverName", serverName);
        query.addQueryItem("serverPort", QString::number(serverPort));
        query.addQueryItem("chords", chordsProgression);
        return query;
    }

    static QUrlQuery createParametersToDisconnect()
    {
        QUrlQuery query;
        query.addQueryItem("cmd", "DISCONNECT");
        return query;
    }

    static QUrlQuery createParametersToRefreshRoomsList()
    {
        QUrlQuery query;
        query.addQueryItem("cmd", "KEEP_ALIVE");
        return query;
    }
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++

LoginService::LoginService(QObject *parent) :
    QObject(parent),
    httpClient(this),
    pendingReply(nullptr),
    connected(false),
    refreshTimer(new QTimer(this))
{
    connect(refreshTimer, &QTimer::timeout, this, &LoginService::refreshTimerSlot);

    // QObject::connect(&httpClient, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrorsSlot(QNetworkReply*,QList<QSslError>)));
}

LoginService::~LoginService()
{
    // disconnect();

    qCDebug(jtLoginService) << "LoginService Destructor";

    if (pendingReply)
        pendingReply->deleteLater();
}

void LoginService::connectInServer(const QString &userName, int instrumentID,
                                   const QString &channelName, const NatMap &localPeerMap,
                                   const QString &version, const QString &environment, int sampleRate)
{
    QUrlQuery query = HttpParamsFactory::createParametersToConnect(userName, instrumentID,
                                                                   channelName, localPeerMap,
                                                                   version, environment,
                                                                   sampleRate);
    qCDebug(jtLoginService) << "Connecting in server ...";

    pendingReply = sendCommandToServer(query);

    if (pendingReply) {
        qDebug(jtLoginService) << "Waiting for server reply...";
        connectNetworkReplySlots(pendingReply, LoginService::Command::CONNECT);
    }
    else {
        qCritical(jtLoginService) << "Pending reply is null!";
    }
}

void LoginService::refreshTimerSlot()
{
    QUrlQuery query = HttpParamsFactory::createParametersToRefreshRoomsList();

    pendingReply = sendCommandToServer(query);

    if (pendingReply)
        connectNetworkReplySlots(pendingReply, LoginService::Command::REFRESH_ROOMS_LIST);
}

void LoginService::disconnectFromServer()
{
    if (isConnected()) {
        qDebug(jtLoginService) << "DISCONNECTING from server...";
        refreshTimer->stop();

        if (pendingReply)
            pendingReply->deleteLater();

        QUrlQuery query = HttpParamsFactory::createParametersToDisconnect();
        qDebug(jtLoginService) << "sending disconnect command to server...";
        pendingReply = sendCommandToServer(query, true);
        qDebug(jtLoginService) << "disconnected";

        if (pendingReply) {
            pendingReply->readAll();
            pendingReply->deleteLater();
            qDebug(jtLoginService) << "reply content readed and discarded!";
            // delete pendingReply;
            // qWarning() << "reply deleted!";
        }
    }

    this->connected = false;

    qDebug(jtLoginService) << "disconnected from login server!";
}

QNetworkReply *LoginService::sendCommandToServer(const QUrlQuery &query, bool synchronous)
{
    if (pendingReply) {
        pendingReply->deleteLater();
    }

    QUrl url(LOGIN_SERVER_URL);
    QByteArray postData(query.toString(QUrl::EncodeUnicode).toStdString().c_str());
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);// disable cache
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded; charset=utf-8"));
    pendingReply = httpClient.post(request, postData);

    if (synchronous && pendingReply) {
        QEventLoop loop;
        connect(pendingReply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
    }
    return pendingReply;
}

QString getNetworkErrorMsg(QNetworkReply::NetworkError error)
{
    switch (error) {
    case QNetworkReply::ConnectionRefusedError:
        return "Connection refused!";
    case QNetworkReply::RemoteHostClosedError:
        return "Remote host closed!";
    case QNetworkReply::HostNotFoundError:
        return "Host not found!";
    case QNetworkReply::TimeoutError:
        return "Time out!";
    case QNetworkReply::OperationCanceledError:
        return "Operation canceled!";
    case QNetworkReply::SslHandshakeFailedError:
        return "Ssl handshake failed!";
    case QNetworkReply::TemporaryNetworkFailureError:
        return "Temporary network failure!";
    case QNetworkReply::NetworkSessionFailedError:
        return "Network Session Failed";
    case QNetworkReply::BackgroundRequestNotAllowedError:
        return "background request not allowed!";
    case QNetworkReply::UnknownNetworkError:
        return "Unknown network error!";
    default:
        return "no error description!";
    }
}

void LoginService::errorSlot(QNetworkReply::NetworkError error)
{
    QString errorMsg;

    if (pendingReply)
        errorMsg = pendingReply->errorString();
    else
        errorMsg = getNetworkErrorMsg(error);

    emit errorWhenConnectingToServer(errorMsg);
}

void LoginService::connectNetworkReplySlots(QNetworkReply *reply, Command command)
{
    if (!reply)
        return;

    switch (command) {
    case LoginService::Command::CONNECT:
        connect(reply, &QNetworkReply::finished, this, &LoginService::connectedSlot);
        break;
    case LoginService::Command::REFRESH_ROOMS_LIST:
        connect(reply, &QNetworkReply::finished, this, &LoginService::roomsListReceivedSlot);
        break;
    default:
        break;
    }

    reply->connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(errorSlot(QNetworkReply::NetworkError)));

    // reply->connect(reply, SIGNAL(sslErrors(QList<QSslError>)),  this, SLOT(sslErrorsSlot(QList<QSslError>)));
}

void LoginService::connectedSlot()
{
    // emit errorWhenConnectingToServer("teste");
    qDebug(jtLoginService) << "CONNECTED in server!";
    refreshTimer->start(REFRESH_PERIOD);
    roomsListReceivedSlot();
    connected = true;
}

void LoginService::roomsListReceivedSlot()
{
    QString json = QString(pendingReply->readAll());
    handleJson(json);
}

void LoginService::handleJson(const QString &json)
{
    if (json.isEmpty())
        return;

    QJsonDocument document = QJsonDocument::fromJson(QByteArray(json.toStdString().c_str()));
    QJsonObject root = document.object();
    if (!connected) { // first time handling json?
        bool clientIsServerCompatible = root.contains("clientCompatibility") && root["clientCompatibility"].toBool();
        bool newVersionAvailable = root.contains("newVersionAvailable") && root["newVersionAvailable"].toBool();

        if (!clientIsServerCompatible) {
            refreshTimer->stop();
            connected = false;
            emit incompatibilityWithServerDetected();
            return;
        }

        if (newVersionAvailable) {
            QString latestVersionDetails("");
            if (root.contains("latestVersionDetails"))
                latestVersionDetails = root["latestVersionDetails"].toString();

            emit newVersionAvailableForDownload(latestVersionDetails);
        }
    }

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

        QString key = getRoomInfoUniqueName(roomInfo);
        lastChordProgressions.insert(key, lastChordProgression);
    }
    emit roomsListAvailable(publicRooms);
}

QString LoginService::getChordProgressionFor(const RoomInfo &roomInfo) const
{
    QString key = getRoomInfoUniqueName(roomInfo);
    if (lastChordProgressions.contains(key))
        return lastChordProgressions[key];

    return ""; // no chord progression available for this roomInfo
}

void LoginService::sendChordProgressionToServer(const QString &userName, const QString &serverName, quint32 serverPort, const QString &chordProgression)
{
    QUrlQuery query = HttpParamsFactory::createParametersToUpdateLastChordProgression(userName, serverName, serverPort, chordProgression);
    sendCommandToServer(query);
}

QString LoginService::getRoomInfoUniqueName(const login::RoomInfo &roomInfo)
{
    return roomInfo.getName() + ":" + roomInfo.getPort();
}

RoomInfo LoginService::buildRoomInfoFromJson(const QJsonObject &jsonObject)
{
    long long id = jsonObject.value("id").toVariant().toLongLong();
    QString typeString = jsonObject["type"].toString();// ninjam OR realtime

    login::RoomTYPE type = (typeString == "ninjam") ? login::RoomTYPE::NINJAM : login::RoomTYPE::REALTIME;
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
    return login::RoomInfo(id, name, port, type, maxUsers, users, 0, bpi, bpm, streamLink);
}
