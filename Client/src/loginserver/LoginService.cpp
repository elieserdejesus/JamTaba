#include "LoginService.h"
#include "natmap.h"
#include "JsonUtils.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QTimer>
#include <QDebug>
#include <QApplication>
#include "../ninjam/Server.h"
#include "../ninjam/Service.h"
#include "../log/logging.h"

using namespace Login;

const QString LoginService::SERVER = "http://jamtaba2.appspot.com/vs";
//const QString LoginService::SERVER = "http://localhost:8080/vs";

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UserInfo::UserInfo(long long id, QString name, QString ip)
    :id(id), name(name), ip(ip){

}


RoomInfo::RoomInfo(long long id, QString roomName, int roomPort, RoomTYPE roomType, int maxUsers, QList<UserInfo> users, int maxChannels, int bpi, int bpm, QString streamUrl)
    :id(id), name(roomName), port(roomPort), type(roomType) , maxUsers(maxUsers), maxChannels(maxChannels),
      users(users), bpi(bpi), bpm(bpm), streamUrl(streamUrl)
{

}

//RoomInfo::RoomInfo(long long id, QString roomName, int roomPort, RoomTYPE roomType, int maxUsers, QList<UserInfo> users, int maxChannels, QString streamUrl)
//    :id(id), name(roomName), port(roomPort), type(roomType) , maxUsers(maxUsers), maxChannels(maxChannels),
//      users(users), bpi(0), bpm(0), streamUrl(streamUrl)
//{

//}

RoomInfo::RoomInfo(QString roomName, int roomPort, RoomTYPE roomType, int maxUsers, int maxChannels)
    :id(-1000), name(roomName), port(roomPort), type(roomType) , maxUsers(maxUsers), maxChannels(maxChannels),
      users(QList<Login::UserInfo>()), bpi(0), bpm(0), streamUrl("")
{

}

//RoomInfo::RoomInfo(const RoomInfo &other)
//    :id(other.id), name(other.name), port(other.port), type(other.type),
//        maxUsers(other.maxUsers), maxChannels(other.maxChannels), users(other.users), streamUrl(other.streamUrl){

//}

int RoomInfo::getNonBotUsersCount() const{
    int nonBots = 0;
    foreach (const UserInfo& userInfo, users) {
        if(!Ninjam::Service::isBotName(userInfo.getName())){
            nonBots++;
        }
    }
    return nonBots;
}

bool RoomInfo::isEmpty() const{
    foreach (const UserInfo& userInfo, users) {
        if(!Ninjam::Service::isBotName(userInfo.getName())){
            return false;
        }
    }
    return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class HttpParamsFactory{
public:
    static QUrlQuery createParametersToConnect(QString userName, int instrumentID, QString channelName, NatMap localPeerMap, QString version, QString environment, int sampleRate) {
        QUrlQuery query;
        query.addQueryItem("cmd", "CONNECT");
        query.addQueryItem("userName", userName);
        query.addQueryItem("instrumentID", QString::number(instrumentID));//used in real time rooms
        query.addQueryItem("channelName", channelName);//use in real time room, need update to allow more channels
        query.addQueryItem("publicIp", localPeerMap.getPublicIp());//used in real time rooms
        query.addQueryItem("publicPort", QString::number(localPeerMap.getPublicPort())); //real time rooms only
        query.addQueryItem("environment", environment );
        query.addQueryItem("version", version);
        query.addQueryItem("sampleRate", QString::number(sampleRate));
        return query;
    }

    static QUrlQuery createParametersToDisconnect() {
        QUrlQuery query;
        query.addQueryItem("cmd", "DISCONNECT");
        return query;
    }

    static QUrlQuery createParametersToRefreshRoomsList(){
        QUrlQuery query;
        query.addQueryItem("cmd", "KEEP_ALIVE");
        return query;
    }
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++

LoginService::LoginService(QObject* parent)
     : QObject(parent), pendingReply(nullptr), connected(false)
{
    refreshTimer = new QTimer(this);
    QObject::connect( refreshTimer, SIGNAL(timeout()), this, SLOT(refreshTimerSlot()));
    //QObject::connect(&httpClient, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrorsSlot(QNetworkReply*,QList<QSslError>)));
}

LoginService::~LoginService()
{
    //disconnect();
    qCDebug(jtLoginService) << "LoginService Destructor";
    if(pendingReply){
        pendingReply->deleteLater();
    }

}

void LoginService::connectInServer(QString userName, int instrumentID, QString channelName, const NatMap &localPeerMap, QString version, QString environment, int sampleRate)
{

    QUrlQuery query = HttpParamsFactory::createParametersToConnect(userName, instrumentID, channelName, localPeerMap, version, environment, sampleRate);
    qCDebug(jtLoginService) << "Connecting in server ..." ;
    pendingReply = sendCommandToServer(query);
    if(pendingReply){
        qDebug(jtLoginService) << "Waiting for server reply..." ;
        connectNetworkReplySlots(pendingReply, LoginService::Command::CONNECT);
    }
    else{
        qCritical(jtLoginService) << "Pending reply is null!";
    }
}

void LoginService::refreshTimerSlot(){
    QUrlQuery query = HttpParamsFactory::createParametersToRefreshRoomsList();
    pendingReply = sendCommandToServer(query);
    if(pendingReply){
        connectNetworkReplySlots(pendingReply, LoginService::Command::REFRESH_ROOMS_LIST);
    }
}

void LoginService::disconnectFromServer()
{
    if(isConnected()){
        qDebug(jtLoginService) << "DISCONNECTING from server...";
        refreshTimer->stop();
        if(pendingReply){
            pendingReply->deleteLater();
        }
        QUrlQuery query = HttpParamsFactory::createParametersToDisconnect();
        qDebug(jtLoginService) << "sending disconnect command to server...";
        pendingReply = sendCommandToServer(query, true);
        qDebug(jtLoginService) << "disconnected";
        if(pendingReply){
            pendingReply->readAll();
            pendingReply->deleteLater();
            qDebug(jtLoginService) << "reply content readed and discarded!";
            //delete pendingReply;
            //qWarning() << "reply deleted!";
        }
    }
    this->connected = false;

    qDebug(jtLoginService) << "disconnected from login server!";
}

QNetworkReply* LoginService::sendCommandToServer(const QUrlQuery &query, bool synchronous )
{
    if(pendingReply){
        pendingReply->deleteLater();

    }

    QUrl url(SERVER);
    QByteArray postData(query.toString(QUrl::EncodeUnicode).toStdString().c_str());
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded; charset=utf-8"));
    pendingReply = httpClient.post(request, postData);
    if(synchronous && pendingReply){
        QEventLoop loop;
        connect(pendingReply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
    }
    return pendingReply;
}


//void LoginService::sslErrorsSlot(QNetworkReply* reply, QList<QSslError> errors){
//    Q_UNUSED(errors);
//    reply->ignoreSslErrors();
//}

QString getNetworkErrorMsg(QNetworkReply::NetworkError error){
    switch(error){
    case QNetworkReply::ConnectionRefusedError: return "Connection refused!";
    case QNetworkReply::RemoteHostClosedError: return "Remote host closed!";
    case QNetworkReply::HostNotFoundError: return "Host not found!";
    case QNetworkReply::TimeoutError: return "Time out!";
    case QNetworkReply::OperationCanceledError: return "Operation canceled!";
    case QNetworkReply::SslHandshakeFailedError: return "Ssl handshake failed!";
    case QNetworkReply::TemporaryNetworkFailureError: return "Temporary network failure!";
    case QNetworkReply::NetworkSessionFailedError: return "Network Session Failed";
    case QNetworkReply::BackgroundRequestNotAllowedError: return "background request not allowed!";
    case QNetworkReply::UnknownNetworkError: return "Unknown network error!";
    default: return "no error description!";
    }
}

void LoginService::errorSlot(QNetworkReply::NetworkError error){
    QString errorMsg;
    if(pendingReply)
        errorMsg = pendingReply->errorString();
    else
        errorMsg = getNetworkErrorMsg(error);
    emit errorWhenConnectingToServer(errorMsg);
}

void LoginService::connectNetworkReplySlots(QNetworkReply* reply, Command command)
{
    if(!reply){
        return;
    }
    switch (command) {
    case LoginService::Command::CONNECT: QObject::connect(reply, SIGNAL(finished()), this, SLOT(connectedSlot()));  break;
    //case LoginService::Command::DISCONNECT: QObject::connect(reply, SIGNAL(finished()), this, SLOT(disconnectedSlot()));  break;
    case LoginService::Command::REFRESH_ROOMS_LIST: QObject::connect(reply, SIGNAL(finished()), this, SLOT(roomsListReceivedSlot())); break;
    default:
        break;
    }

    reply->connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),  this, SLOT(errorSlot(QNetworkReply::NetworkError)));
    //reply->connect(reply, SIGNAL(sslErrors(QList<QSslError>)),  this, SLOT(sslErrorsSlot(QList<QSslError>)));
}

void LoginService::connectedSlot(){
    //emit errorWhenConnectingToServer("teste");
    qDebug(jtLoginService) << "CONNECTED in server!" ;
    refreshTimer->start(REFRESH_PERIOD);
    roomsListReceivedSlot();
    connected = true;
}





void LoginService::roomsListReceivedSlot(){
    QString json = QString( pendingReply->readAll());
    handleJson(json);
}

void LoginService::handleJson(QString json){
    if(json.isEmpty()){
        return;
    }
    QJsonDocument document = QJsonDocument::fromJson(QByteArray(json.toStdString().c_str()));
    QJsonObject root = document.object();
    if(!connected){//first time handling json?
        bool clientIsServerCompatible = root["clientCompatibility"].toBool();
        bool newVersionAvailable = root["newVersionAvailable"].toBool();

        if(!clientIsServerCompatible){
            refreshTimer->stop();
            connected = false;
            emit incompatibilityWithServerDetected();
            return;
        }

        if(newVersionAvailable){
            emit newVersionAvailableForDownload();
        }
    }

    QJsonArray allRooms = root["rooms"].toArray();
    QList<RoomInfo> publicRooms;
    for (int i = 0; i < allRooms.size(); ++i) {
        QJsonObject jsonObject = allRooms[i].toObject();
        publicRooms.append(buildRoomInfoFromJson(jsonObject));
    }
    emit roomsListAvailable(publicRooms);
}

RoomInfo LoginService::buildRoomInfoFromJson(QJsonObject jsonObject){
    long long id = jsonObject.value("id").toVariant().toLongLong();
    QString typeString = jsonObject["type"].toString();//ninjam OR realtime

    Login::RoomTYPE type = (typeString == "ninjam") ? Login::RoomTYPE::NINJAM : Login::RoomTYPE::REALTIME;
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
        QString userIp =  userObject.value("ip").toString();
        users.append(Login::UserInfo(userID, userName, userIp));
    }
    return Login::RoomInfo(id, name, port, type, maxUsers, users, 0, bpi, bpm, streamLink);
}




