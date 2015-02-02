#include "LoginServiceResponse.h"
#include "../model/JamRoom.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include "JsonUtils.h"
#include <QDebug>

using namespace Login;
using namespace Model;


Login::LoginServiceResponse::LoginServiceResponse(QString json)
    :totalOnlineUsers(0)
{
    QJsonDocument document = QJsonDocument::fromJson(QByteArray(json.toStdString().c_str()));
    QJsonObject rootObject = document.object();
    QList<Model::RealTimeRoom*> realTimeRooms = buildRealTimeJamRoomList(rootObject);
    this->totalOnlineUsers = 0;
    foreach(Model::RealTimeRoom* room, realTimeRooms){
        realTimeRoomsMap.insert(room->getId(), room);
        this->totalOnlineUsers += room->getPeersCount();
    }

    QList<Model::NinjamRoom*> ninjamRooms = buildNinjamJamRoomList(rootObject);
    foreach(Model::NinjamRoom* room, ninjamRooms ){
        ninjamServersMap.insert(room->getId(), room);
        this->totalOnlineUsers += room->getPeersCount();
    }

    if(rootObject["peer"] != QJsonValue::Undefined){//not connected?
        QJsonObject jsonPeer = rootObject["peer"].toObject();
        QJsonObject jsonCurrentRoom = jsonPeer["room"].toObject();
        this->currentRoom = JsonUtils::realTimeRoomFromJson(jsonCurrentRoom);
        this->connectedPeer = JsonUtils::peerFromJson(jsonPeer);
    }
    else{
        this->currentRoom = nullptr;
        this->connectedPeer = nullptr;
    }
}

/*
LoginServiceResponse LoginServiceResponse::fromJson(QString jsonString){
    QJsonDocument document = QJsonDocument::fromJson(QByteArray(jsonString.toStdString().c_str()));
    QJsonObject rootObject = document.object();
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    QList<AbstractJamRoom*> jamRooms = buildJamRoomList(rootObject);
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if(rootObject["peer"] == QJsonValue::Undefined){
        return LoginServiceResponse(jamRooms);
    }
    //+++++++++++
    QJsonObject jsonPeer = rootObject["peer"];
    QJsonObject jsonCurrentRoom = (QJsonObject) jsonPeer.get["room"];
    AbstractJamRoom currentRoom = JamRoomUtils.jamRoomFromJSON(jsonCurrentRoom);
    Peer connectedPeer = PeerUtils.peerFromJSON(jsonPeer);
    return LoginServiceResponse(jamRooms, connectedPeer, currentRoom);
}
*/
QList<RealTimeRoom *> LoginServiceResponse::buildRealTimeJamRoomList(QJsonObject rootJsonObject){
    QJsonArray realtimeRomsArray = rootJsonObject["realtimeRooms"].toArray();
    QList<RealTimeRoom*> allRooms;
    foreach (QJsonValue value, realtimeRomsArray) {
        allRooms.append(JsonUtils::realTimeRoomFromJson(value.toObject()));
    }

    return allRooms;
}

QList<Model::NinjamRoom*> LoginServiceResponse::buildNinjamJamRoomList(QJsonObject rootJsonObject)
{
    QList<NinjamRoom*> allRooms;
    QJsonArray ninjamServersArray = rootJsonObject["ninjamServers"].toArray();
    foreach (QJsonValue value, ninjamServersArray) {
        allRooms.append(JsonUtils::ninjamServerFromJson(value.toObject()));
    }
    return allRooms;
}

/*
void LoginServiceResponse::addRoom(AbstractJamRoom room) {
    if (!roomsMap.containsKey(room.getId())) {
        this.roomsMap.put(room.getId(), room);
        totalOnlineUsers += room.getPeersCount();
        if (room.isTheWaitingRoom()) {
            Collection<Peer> peersInWaitingRoom = room.getPeers();
            for (Peer peer : peersInWaitingRoom) {
                if (peer.isPlayingInNinjam()) {
                    totalOnlineUsers--;//avoid count 2 times (one for waiting room and other for ninjam room)
                }
            }
        }
    }
}
*/


QList<RealTimeRoom*> LoginServiceResponse::getRealtimeRooms() const{
    return realTimeRoomsMap.values();
}

QList<NinjamRoom*> LoginServiceResponse::getNinjamRooms() const{
    return ninjamServersMap.values();
}

const Peer *LoginServiceResponse::getConnectedPeer() const{
    return connectedPeer;
}


/**
 * *
 * Usei esse método para setar o server do ninjam como sala atual. Como os
 * servidores do ninjam não estão no servidor do VS estava dando um problema
 * com a sala atual. O DefaultLoginService tinha uma sala atual que era uma
 * instância de NinjamRoom mas o servidor do VS estava retornando a sala
 * atual como sendo a WaitingRoom. Isso acontece porque o servidor só tira o
 * usuário da waitingRoom quando ele entra em uma sala do VS.
 *
 * @param room
 */
/*
void LoginServiceResponse::setCurrentRoom(NinjaMRoom room) {
    currentRoom = room;
    //fiz isso porque quando conectava em um server ninjam eu ainda aparecia na waiting room
    if(waitingRoom != null && waitingRoom.containsPeer(connectedPeer.getId())){
        waitingRoom.removePeer(connectedPeer);
    }
}
*/

const AbstractJamRoom* LoginServiceResponse::getCurrentRoom() const {
    return currentRoom;
}

int LoginServiceResponse::getTotalOnlineUsers() const{
    return totalOnlineUsers;
}
