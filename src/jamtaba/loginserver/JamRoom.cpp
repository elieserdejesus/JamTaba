#include "JamRoom.h"
#include <QSet>
#include <QMap>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include "../ninjam/Service.h"
#include "../ninjam/Server.h"
#include <cstring>

using namespace Login;

QMap<long long, std::shared_ptr<AbstractPeer>> AbstractPeer::peers;// = QMap<long long, std::shared_ptr<AbstractPeer>>();

AbstractPeer::AbstractPeer(long long ID)
    :id(ID)//, countryCode("unknown")
{

}

AbstractPeer::~AbstractPeer(){
    //qDebug() << "AbstractPeer destructor " << getName();
}

AbstractPeer* AbstractPeer::getByIP(QString ip){
    foreach (auto peer, AbstractPeer::peers.values()) {
        if(peer->getIP() == ip){
            return &(*peer);
        }
    }
    return nullptr;
}

//+++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++
NinjamPeer::NinjamPeer(long long ID)
    :AbstractPeer(ID)
{

}



void NinjamPeer::updateFromJson(QJsonObject json){
    if(this->name.isNull() || this->name.isEmpty()){
        this->name = json["name"].toString();
        this->ip = json["ip"].toString();
        //this->countryCode = json["countryCode"].toString();
    }
}

bool NinjamPeer::isBot() const
{
    return Ninjam::Service::isBotName(this->name);
}



//+++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++
RealTimePeer::RealTimePeer(long long ID)
    :AbstractPeer(ID), port(0), jamtabaVersion(0), sampleRate(0)
{

}

void RealTimePeer::updateFromJson(QJsonObject /*json*/){

}

//++++++++++++++++++
//++++++++++++++++++
//++++++++++++++++++

AbstractJamRoom::AbstractJamRoom(long long roomID)
    : id(roomID),
      roomName(""), maxUsers(0)
{
    //qDebug() << "AbstractJamRoom constructor ID:" << id;
}


AbstractJamRoom::~AbstractJamRoom(){
    //qDebug() << "AbstractJamRoom destructor!";
}

bool AbstractJamRoom::containsPeer(long long peerId) const{
    return peers.contains(peerId);
}

//void AbstractJamRoom::addPeer(AbstractPeer* peer) {
//    if (!peers.contains(peer->getId())) {
//        peers.insert(peer->getId(); std::shared_ptr<AbstractPeer>(peer));
//        qDebug() << "Adicionou peer " << peer->getName();
//    }
//}

void AbstractJamRoom::removePeer(AbstractPeer *peer) {
    int removed = peers.remove(peer->getId());
    if(removed > 0){
        qDebug() << "Peer removido: " << peer->getName();
    }
}

//void AbstractJamRoom::refreshPeerList(const QMap<long long, AbstractPeer *> &peersInServer) {
//    //verifica peers adicionados
//    QSet<long long> serverKeys = QSet<long long>::fromList( peersInServer.keys());
//    foreach (long long serverKey , serverKeys) {
//        if (!peers.contains(serverKey)) {
//            addPeer(peersInServer[serverKey]);
//        }
//    }
//    //verifica peers que devem ser removidos
//    QList<long long> keysToRemove;
//    foreach (long long key , peers.keys()) {
//        if (!serverKeys.contains(key)) {
//            //se a chave não esta mais na lista de peers do servidor então o peer saiu da sala
//            keysToRemove.append(key);
//        }
//    }
//    foreach (long long key , keysToRemove) {
//        removePeer(peers[key]);
//    }
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool AbstractJamRoom::isFull() const{
    return peers.size() >= getMaxUsers();
}

QString AbstractJamRoom::getName() const{
    return roomName;
}

QList<AbstractPeer*> AbstractJamRoom::getPeers() const{
    QList<AbstractPeer*> list;
    for(std::shared_ptr<AbstractPeer> p : peers.values()){
        list.append(&*p);
    }
    return list;
}


int AbstractJamRoom::getPeersCount() const{
    return peers.size();
}

bool AbstractJamRoom::hasPeers() const{
    return peers.size() > 0;
}





const AbstractPeer *AbstractJamRoom::getPeer(long long peerID) const{
    return peers[peerID].get();
}

bool AbstractJamRoom::isEmpty() const{
    return peers.isEmpty();
}

bool Login::jamRoomLessThan(AbstractJamRoom* r1, AbstractJamRoom* r2)
{
     return r1->getPeersCount() > r2->getPeersCount();
}

//+++++++++++++++++++++++++++++++++++++++++++++++

RealTimeRoom::RealTimeRoom(long long id)
    :AbstractJamRoom(id)
{
    // qDebug() << "RealTimeRoom constructor";
}

RealTimeRoom::~RealTimeRoom()
{
    // qDebug() << "RealTimeRoom destructor!";
}

bool RealTimeRoom::updateFromJson(QJsonObject json){
    if(this->roomName.isNull() || this->roomName.isEmpty()){
        this->roomName = json["name"].toString();
        this->maxUsers = json["maxUsers"].toInt();
    }
    return false;
}


/***
     *
     * @param localPeer - Skip local peer
     * @return the reachable peers list skipping local peer
     */
QList<RealTimePeer*> RealTimeRoom::getReachablePeers(AbstractPeer* localPeer) const{
    QList<AbstractPeer*> allPeers = getPeers();
    QList<RealTimePeer*> reachablePeers;
    if(localPeer == nullptr){//inclui o local peer na lista
        foreach (AbstractPeer* peer , allPeers) {
            if (dynamic_cast<RealTimePeer*>(peer)->isReachable()) {
                reachablePeers.append(dynamic_cast<RealTimePeer*>(peer));
            }
        }
    }
    else{//exclui o peer local da lista
        foreach (AbstractPeer* peer , allPeers) {
            if (dynamic_cast<RealTimePeer*>(peer)->isReachable() && peer != localPeer) {
                reachablePeers.append(dynamic_cast<RealTimePeer*>(peer));
            }
        }
    }
    return reachablePeers;
}

QList<RealTimePeer *> RealTimeRoom::getReachablePeers() const{
    return getReachablePeers(nullptr);
}



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++             NINJAM  ROOM         ++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QMap<QString, NinjamRoom*> NinjamRoom::ninjamRooms;


//bool containBotPeer;
//QString streamLink;
//QString hostName;
//int hostPort;
//int currentBpi;
//int currentBpm;
NinjamRoom::NinjamRoom(long long ID)
    :AbstractJamRoom(ID),
      containBotPeer(false),
      streamLink(""),
      hostName(""),
      hostPort(0),
      currentBpi(0),
      currentBpm(0)
{

}

NinjamRoom::NinjamRoom(QString host, int port, int maxUsers)
    :AbstractJamRoom(1),
      containBotPeer(false),
      streamLink(""),
      hostName(host),
      hostPort(port),
      currentBpi(-1),
      currentBpm(-1)
{
    this->maxUsers = maxUsers;
}

NinjamRoom::NinjamRoom(QString host, int port, int maxUsers, int bpi, int bpm)
    :AbstractJamRoom(1),
      containBotPeer(false),
      streamLink(""),
      hostName(host),
      hostPort(port),
      currentBpi(bpi),
      currentBpm(bpm)
{
    this->maxUsers = maxUsers;
}

NinjamRoom::~NinjamRoom(){
    ninjamRooms.remove(getMapKey());
}

QString NinjamRoom::getMapKey() const{
    return buildMapKey(hostName, hostPort);
}

bool NinjamRoom::updateFromJson(QJsonObject json){
    if(this->hostName.isEmpty() || this->hostName.isNull()){//not initialized
        this->hostName = json["name"].toString();
        this->hostPort = json["port"].toInt();
        this->maxUsers = json["maxUsers"].toInt();
        this->streamLink = json["streamUrl"].toString();

        ninjamRooms.insert(getMapKey(), this);
    }

    //insert new users and removed the olds
    bool changed = false;
    QJsonArray usersArray = json["users"].toArray();
    QSet<long long> idsInServer;
    for (int i = 0; i < usersArray.size(); ++i) {//check for new users
        QJsonObject userObject = usersArray[i].toObject();
        long long userID = userObject.value("id").toVariant().toLongLong();
        idsInServer.insert(userID);
        if(!peers.contains(userID)){
            AbstractPeer* newPeer = dynamic_cast<AbstractPeer*>(new NinjamPeer(userID));
            peers.insert(userID, std::shared_ptr<AbstractPeer>(newPeer));
            newPeer->updateFromJson(userObject);
            if(newPeer->isBot()){
                this->containBotPeer = true;
            }
            changed = true;
        }
    }

    //check for removed users
    for(long long userID : peers.keys()){
        if(!idsInServer.contains(userID)){
            peers.remove(userID);
            changed = true;
        }
    }


    return changed;
}


bool NinjamRoom::hasStreamLink() const{
    return !streamLink.isNull() && !streamLink.isEmpty();
}

QString NinjamRoom::getStreamLink() const{
    if(!hasStreamLink()){
        return "";
    }
    return streamLink;
}

bool NinjamRoom::isEmpty() const{
    if(containBot()){
        return getPeersCount() - 1 <= 0;
    }
    return AbstractJamRoom::isEmpty();
}


QString NinjamRoom::getName() const{
    return hostName + ":" + QString::number(hostPort);
}

QString NinjamRoom::buildMapKey(QString hostName, int hostPort){
    return hostName + ":" + QString::number(hostPort);
}

NinjamRoom* NinjamRoom::getNinjamRoom(const Ninjam::Server* server)
{
    if(server){
        QString key = buildMapKey(server->getHostName(), server->getPort());
        if(ninjamRooms.contains(key)){
            NinjamRoom* room = ninjamRooms[key];
            return room;
        }
    }
    return nullptr;
}

//int NinjamRoom::getPeersCount() const{
//    if(containBot()){
//        return AbstractJamRoom::getPeersCount() - 1;
//    }
//    return AbstractJamRoom::getPeersCount();
//}

