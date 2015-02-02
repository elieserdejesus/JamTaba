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
    QList<Model::AbstractJamRoom*> jamRooms = buildJamRoomList(rootObject);
    this->totalOnlineUsers = 0;
    foreach(Model::AbstractJamRoom* room, jamRooms){
        roomsMap.insert(room->getId(), room);
        this->totalOnlineUsers += room->getPeersCount();
    }

    if(rootObject["peer"] != QJsonValue::Undefined){//not connected?
        QJsonObject jsonPeer = rootObject["peer"].toObject();
        QJsonObject jsonCurrentRoom = jsonPeer["room"].toObject();
        this->currentRoom = JsonUtils::jamRoomFromJson(jsonCurrentRoom);
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
QList<Model::AbstractJamRoom*> LoginServiceResponse::buildJamRoomList(QJsonObject rootJsonObject){
    QJsonArray jamRoomsArray = rootJsonObject["rooms"].toArray();
    QList<AbstractJamRoom*> realTimeRooms;
    foreach (QJsonValue value, jamRoomsArray) {
        AbstractJamRoom* jamRoom = JsonUtils::jamRoomFromJson(value.toObject());
        realTimeRooms.append(jamRoom);
    }

    QList<AbstractJamRoom*> ninjaRooms;
    QList<AbstractJamRoom*> allRooms;
    allRooms.append( ninjaRooms);
    allRooms.append(realTimeRooms);

    //Collections.sort(allRooms);
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

const AbstractJamRoom* LoginServiceResponse::getRoom(long id) const {
    return roomsMap[id];
}

QList<AbstractJamRoom *> LoginServiceResponse::getRooms() const{
    //qDebug() << "roomsMap.values().size:" << roomsMap.values().size();
    QList<AbstractJamRoom*> sortedRooms(roomsMap.values());// = new ArrayList<AbstractJamRoom>(roomsMap.values());
    //Collections.sort(sortedRooms);
    return sortedRooms;
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
