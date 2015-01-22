#include "LoginService.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonDocument>
#include <QList>
#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkReply>
#include <QEventLoop>

LoginService::LoginService()
{
    this->connected = false;
}
//+++++++++++++++++++++


/*
LoginServiceResponse::LoginServiceResponse(){
    this->totalOnlineUsers = 0;
}

const LoginServiceResponse* LoginServiceResponse::parseFromJSON(QString jsonString){

    QJsonDocument document = QJsonDocument::fromRawData(jsonString.toLocal8Bit().data(), jsonString.size());
    QJsonObject rootObject = document.object();
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    QList<AbstractJamRoom> jamRooms = buildJamRoomList(rootObject);
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    QJsonObject jsonPeer = rootObject["peer"].toObject();
    if (jsonPeer == null) {
        return new LoginServiceResponse(jamRooms);
    }
    //+++++++++++
    QJsonObject jsonCurrentRoom = jsonPeer[.get["room"];
    AbstractJamRoom* currentRoom = JamRoomUtils.jamRoomFromJSON(jsonCurrentRoom);
    Peer connectedPeer = PeerUtils.peerFromJSON(jsonPeer);
    return new LoginServiceResponse(jamRooms, connectedPeer, currentRoom);
}

QList<AbstractJamRoom*> LoginServiceResponse::buildJamRoomList(QJsonObject rootJsonObject){
    QList<AbstractJamRoom*> vsRooms;
    QJsonArray jamRoomsArray = rootJsonObject["rooms"].toArray();
    foreach (const QJsonValue& value, jamRoomsArray) {
        QJsonObject jamRoomJSONObject = value.toObject();
        RealtimeRoom jamRoom = JamRoomUtils.jamRoomFromJSON(jamRoomJSONObject);
        vsRooms.add(jamRoom);
    }

    QList<AbstractJamRoom> ninjaRooms;
//        List<NinjaMServer> publicServers = NinjaMService.getPublicServersInfos();
//        JamRoomFilter jamRoomFilter = FilterFactory.createFilter(FilterModel.getInstance());
//        for (NinjaMServer ninjaMServer : publicServers) {
//            if (ninjaMServer.isActive()) {
//                AbstractJamRoom jamRoom = JamRoomUtils.jamRoomFromNinjaMServer(ninjaMServer);
//                if (jamRoomFilter.acceptRoom(jamRoom)) {
//                    ninjaRooms.add(jamRoom);
//                }
//            }
//        }
    QList<AbstractJamRoom> allRooms;
    allRooms.append( ninjaRooms);
    allRooms.append(vsRooms);
    //Collections.sort(allRooms);
    return allRooms;
}
*/



