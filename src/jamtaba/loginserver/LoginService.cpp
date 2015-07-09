#include "LoginService.h"
#include "natmap.h"
#include "JsonUtils.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include "../ninjam/Server.h"

using namespace Login;

const QString LoginService::SERVER = "https://jamtaba2.appspot.com/vs";
//const QString LoginService::SERVER = "http://localhost:8080/vs";

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UserInfo::UserInfo(long long id, QString name, QString ip)
    :id(id), name(name), ip(ip){

}

RoomInfo::RoomInfo(long long id, QString roomName, int roomPort, RoomTYPE roomType, int maxUsers, QList<UserInfo> users, QString streamUrl)
    :id(id), name(roomName), port(roomPort), type(roomType) , maxUsers(maxUsers),
      users(users), streamUrl(streamUrl)
{

}

RoomInfo::RoomInfo(QString roomName, int roomPort, RoomTYPE roomType, int maxUsers)
    :id(-1000), name(roomName), port(roomPort), type(roomType) , maxUsers(maxUsers),
      users(QList<Login::UserInfo>()), streamUrl("")
{

}


bool RoomInfo::isEmpty() const{
    return users.isEmpty();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class HttpParamsFactory{
public:
    static QUrlQuery parametersToConnect(QString userName, int instrumentID, QString channelName, NatMap localPeerMap, int version, QString environment, int sampleRate) {
        QUrlQuery query;
        query.addQueryItem("cmd", "CONNECT");
        query.addQueryItem("userName", userName);
        query.addQueryItem("instrumentID", QString::number(instrumentID));
        query.addQueryItem("channelName", channelName);
        query.addQueryItem("publicIp", localPeerMap.getPublicIp());
        query.addQueryItem("publicPort", QString::number(localPeerMap.getPublicPort()));
        query.addQueryItem("environment", environment );
        query.addQueryItem("version", QString::number(version));
        query.addQueryItem("sampleRate", QString::number(sampleRate));
        return query;
    }

    static QUrlQuery parametersToDisconnect() {
        QUrlQuery query;
        query.addQueryItem("cmd", "DISCONNECT");
        return query;
    }
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++

LoginService::LoginService(QObject* parent)
     : QObject(parent), pendingReply(nullptr), connected(false)
{

}

LoginService::~LoginService()
{
    //disconnect();

}

void LoginService::connectInServer(QString userName, int instrumentID, QString channelName, const NatMap &localPeerMap, int version, QString environment, int sampleRate)
{
    QUrlQuery query = HttpParamsFactory::parametersToConnect(userName, instrumentID, channelName, localPeerMap, version, environment, sampleRate);
    pendingReply = sendCommandToServer(query);
    connectNetworkReplySlots(pendingReply, LoginService::Command::CONNECT);

}

void LoginService::disconnect()
{
    if(isConnected()){
        qDebug() << "DISCONNECTING from server...";
        QUrlQuery query = HttpParamsFactory::parametersToDisconnect();
        pendingReply = sendCommandToServer(query);
        connectNetworkReplySlots(pendingReply, LoginService::Command::DISCONNECT);
    }
}

QNetworkReply* LoginService::sendCommandToServer(const QUrlQuery &query)
{
    if(pendingReply != NULL){
        pendingReply->deleteLater();
    }
    QUrl url(SERVER);
    QByteArray postData(query.toString(QUrl::EncodeUnicode).toStdString().c_str());
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded; charset=utf-8"));
    return httpClient.post(request, postData);
}


void LoginService::disconnectedSlot(){
    this->connected = false;
    emit disconnectedFromServer();
}

void LoginService::sslErrorsSlot(QList<QSslError> errorList){
    foreach (const QSslError& error, errorList) {
        qDebug() << error;
    }
}

void LoginService::errorSlot(QNetworkReply::NetworkError /*error*/){
    if(pendingReply != NULL){
        qCritical() << pendingReply->errorString();
        //qFatal(pendingReply->errorString().toStdString().c_str());
    }
}

void LoginService::connectNetworkReplySlots(QNetworkReply* reply, Command command)
{
    switch (command) {
    case LoginService::Command::CONNECT: reply->connect(reply, SIGNAL(finished()), this, SLOT(connectedSlot()));  break;
    case LoginService::Command::DISCONNECT: reply->connect(reply, SIGNAL(finished()), this, SLOT(disconnectedSlot()));  break;
    default:
        break;
    }
    reply->connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),  this, SLOT(errorSlot(QNetworkReply::NetworkError)));
    reply->connect(reply, SIGNAL(sslErrors(QList<QSslError>)),  this, SLOT(sslErrorsSlot(QList<QSslError>)));
}

void LoginService::connectedSlot(){
    connected = true;
    if(!LOCAL_HOST_MODE){
        QString json = QString( pendingReply->readAll());
        handleJson(json);
    }
    else{//local host - test mode
        QList<Login::RoomInfo> publicRooms;
        publicRooms.append(RoomInfo(1, "localhost", 2049, Login::RoomTYPE::NINJAM, 16, QList<UserInfo>()));
        emit roomsListAvailable(publicRooms);
    }
}

void LoginService::handleJson(QString json){
    QJsonDocument document = QJsonDocument::fromJson(QByteArray(json.toStdString().c_str()));
    QJsonObject root = document.object();
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

    QJsonArray usersArray = jsonObject["users"].toArray();
    QList<UserInfo> users;
    for (int i = 0; i < usersArray.size(); ++i) {
        QJsonObject userObject = usersArray[i].toObject();
        long long userID = userObject.value("id").toVariant().toLongLong();
        QString userName = userObject.value("name").toString();
        QString userIp =  userObject.value("ip").toString();
        users.append(Login::UserInfo(userID, userName, userIp));
    }
    return Login::RoomInfo(id, name, port, type, maxUsers, users, streamLink);
}




