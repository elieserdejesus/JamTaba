#include "LoginService.h"

LoginService::LoginService(LoginServiceListener *listener)
{
    this->connected = false;
    this->listener = listener;
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



