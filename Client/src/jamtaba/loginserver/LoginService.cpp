#include "LoginService.h"
#include "JamRoom.h"
#include "natmap.h"
#include "JsonUtils.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>

using namespace Login;

const QString LoginService::SERVER = "https://jamtaba2.appspot.com/vs";
//const QString LoginService::SERVER = "http://localhost:8080/vs";
QMap<long long, std::shared_ptr<AbstractJamRoom>> LoginService::rooms;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//class LoginServiceParser {//this class handle json data

//private:

//    static QList<RealTimeRoom *> buildRealTimeJamRoomList(QJsonObject rootJsonObject){
//        QJsonArray realtimeRomsArray = rootJsonObject["realtimeRooms"].toArray();
//        QList<RealTimeRoom*> allRooms;
//        foreach (QJsonValue value, realtimeRomsArray) {
//            allRooms.append(JsonUtils::realTimeRoomFromJson(value.toObject()));
//        }

//        return allRooms;
//    }

//    static QList<NinjamRoom*> buildNinjamJamRoomList(QJsonObject rootJsonObject)
//    {
//        QList<NinjamRoom*> allRooms;
//        QJsonArray ninjamServersArray = rootJsonObject["ninjamServers"].toArray();
//        foreach (QJsonValue value, ninjamServersArray) {
//            allRooms.append(JsonUtils::ninjamServerFromJson(value.toObject()));
//        }
//        return allRooms;
//    }

//public:

//    static QList<AbstractJamRoom*> getRooms(QString json) {
//        QJsonDocument document = QJsonDocument::fromJson(QByteArray(json.toStdString().c_str()));
//        QJsonObject rootObject = document.object();

//        QList<AbstractJamRoom*> rooms;

//        QList<NinjamRoom*> ninjamRooms = buildNinjamJamRoomList(rootObject);
//        foreach(NinjamRoom* room, ninjamRooms ){
//            rooms.append(room);
//        }

//        QList<RealTimeRoom*> realTimeRooms = buildRealTimeJamRoomList(rootObject);
//        foreach(RealTimeRoom* room, realTimeRooms){
//            rooms.append(room);
//        }

////        if(rootObject["peer"] != QJsonValue::Undefined){//not connected?
////            QJsonObject jsonPeer = rootObject["peer"].toObject();
////            QJsonObject jsonCurrentRoom = jsonPeer["room"].toObject();
////            this->currentRoom = JsonUtils::realTimeRoomFromJson(jsonCurrentRoom);
////            this->connectedPeer = JsonUtils::peerFromJson(jsonPeer);
////        }
////        else{
////            this->currentRoom = nullptr;
////            this->connectedPeer = nullptr;
////        }
//        return rooms;
//    }
//};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++


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
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++

LoginService::LoginService(QObject* parent)
     : QObject(parent), connected(false), pendingReply(nullptr)
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
        qFatal(pendingReply->errorString().toStdString().c_str());
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
    //qDebug() << "connected slot!" << pendingReply->readAll();
    connected = true;
    QString json = QString( pendingReply->readAll());
    updateFromJson(json);
    QList<AbstractJamRoom*> rooms = buildTheRoomsList();

    emit connectedInServer(rooms);
}

void LoginService::updateFromJson(QString json){
    QJsonDocument document = QJsonDocument::fromJson(QByteArray(json.toStdString().c_str()));
    QJsonObject root = document.object();
    qDebug() << "room no map antes de atualizar: " << rooms.size();
    QJsonArray allRooms = root["rooms"].toArray();
    for (int i = 0; i < allRooms.size(); ++i) {
        QJsonObject jsonObject = allRooms[i].toObject();
        long long id = jsonObject.value("id").toVariant().toLongLong();
        QString roomType = jsonObject["type"].toString();//NINJAM OR REALTIME
        AbstractJamRoom* room;
        if(!rooms.contains(id)){
            if(roomType == "ninjam"){
                room = dynamic_cast<AbstractJamRoom*>(new NinjamRoom(id));
            }
            else{
                room = dynamic_cast<AbstractJamRoom*>(new RealTimeRoom(id));
            }
            rooms.insert(id,  std::shared_ptr<AbstractJamRoom>(room));
        }
        bool changed = room->updateFromJson(jsonObject);
        if(changed){
            emit roomChanged(*room);
        }
    }
    qDebug() << "rooms recebidas: " << allRooms.size();
    qDebug() << "room inseridas no map: " << rooms.size();

}

QList<AbstractJamRoom*> LoginService::buildTheRoomsList(){
    QList<AbstractJamRoom*> list;
    for(std::shared_ptr<AbstractJamRoom> &l : rooms.values()){
        AbstractJamRoom* room = l.get();
        list.append(room);
    }
    qSort(list.begin(), list.end(), Login::jamRoomLessThan);
    return list;
}





