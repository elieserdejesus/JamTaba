#include "JamRoom.h"
#include <QSet>
#include <QMap>
#include <QDebug>
#include "Peer.h"


AbstractJamRoom::AbstractJamRoom(long long roomID)
    : id(roomID),
      roomName("")
{
    //qDebug() << "AbstractJamRoom constructor ID:" << id;
}

bool AbstractJamRoom::containsPeer(long peerId) const{
    return peers.contains(peerId);
}

void AbstractJamRoom::addPeer(Peer* peer) {
    if (!peers.contains(peer->getId())) {
        peers.insert(peer->getId(), peer);
        qDebug() << "Adicionou peer " << peer->getUserName();
        firePeerAdded(*peer);
    }
}

void AbstractJamRoom::removePeer(Peer *peer) {
    int removed = peers.remove(peer->getId());
    if(removed > 0){
        qDebug() << "Peer removido: " << peer->getUserName();
        firePeerRemoved(*peer);
    }
}

void AbstractJamRoom::refreshPeerList(const QMap<long long, Peer *> &peersInServer) {
    //verifica peers adicionados
    QSet<long long> serverKeys = QSet<long long>::fromList( peersInServer.keys());
    foreach (long long serverKey , serverKeys) {
        if (!peers.contains(serverKey)) {
            addPeer(peersInServer[serverKey]);
        }
    }
    //verifica peers que devem ser removidos
    QList<long long> keysToRemove;
    foreach (long long key , peers.keys()) {
        if (!serverKeys.contains(key)) {
            //se a chave não esta mais na lista de peers do servidor então o peer saiu da sala
            keysToRemove.append(key);
        }
    }
    foreach (long long key , keysToRemove) {
        removePeer(peers[key]);
    }
}

void AbstractJamRoom::firePeerRemoved(const Peer& removedPeer) {
    foreach (JamRoomListener* l , listeners) {
        l->peerRemoved(removedPeer);
    }
}

void AbstractJamRoom::firePeerAdded(const Peer &newPeer) {
    foreach (JamRoomListener* l , listeners) {
        l->peerAdded(newPeer);
    }
}

void AbstractJamRoom::fireNameChanged() {
    foreach (JamRoomListener* l , listeners) {
        l->nameChanged();
    }
}
//++++++++++++++++=
void AbstractJamRoom::addListener( JamRoomListener& jamRoomListener) {
    if (!listeners.contains(&jamRoomListener)) {
        listeners.insert(&jamRoomListener);
    }
}

void AbstractJamRoom::removeListener( JamRoomListener& l) {
    listeners.remove(&l);
}

void AbstractJamRoom::set(QString name, bool /*isStatic*/, int maxUsers, const QMap<long long, Peer *> &peers)
{
    if(name != this->roomName){
        roomName = name;
        fireNameChanged();
    }
    this->maxUsers = maxUsers;
    refreshPeerList(peers);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool AbstractJamRoom::isFull() const{
    return peers.size() >= getMaxUsers();
}

QList<Peer*> AbstractJamRoom::getPeers() const{
    return peers.values();
}

QSet<Peer*> AbstractJamRoom::getPeersAsSet() const{
    return QSet<Peer*>::fromList(getPeers());
}

/***
     *
     * @param localPeer - Skip local peer
     * @return the reachable peers list skipping local peer
     */
QList<Peer*> AbstractJamRoom::getReachablePeers(Peer* localPeer) const{
    QList<Peer*> allPeers = getPeers();
    QList<Peer*> reachablePeers;
    if(localPeer == nullptr){//inclui o local peer na lista
        foreach (Peer* peer , allPeers) {
            if (peer->isReachable()) {
                reachablePeers.append(peer);
            }
        }
    }
    else{//exclui o peer local da lista
        foreach (Peer* peer , allPeers) {
            if (peer->isReachable() && peer != localPeer) {
                reachablePeers.append(peer);
            }
        }
    }
    return reachablePeers;
}

QList<Peer *> AbstractJamRoom::getReachablePeers() const{
    return getReachablePeers(nullptr);
}

int AbstractJamRoom::getPeersCount() const{
    return peers.size();
}

bool AbstractJamRoom::hasPeers() const{
    return peers.size() > 0;
}

//void AbstractJamRoom::setName(QString roomName) {
////    if (originalName == NULL) {
////        originalName = roomName;
////        if(roomName == NULL){
////            return;
////        }
////    }

////    //if (this.roomName == NULL || !this.roomName.equals(roomName)) {
////    String roomWord = java.util.ResourceBundle.getBundle("Bundle").getString("Room");
////    this.roomName = roomName.replace("Room", roomWord);
//    this->roomName = roomName;
//    //fireNameChanged();
//    //}
//}




const Peer* AbstractJamRoom::getPeer(long long peerID) const{
    return peers[peerID];
}

bool AbstractJamRoom::isEmpty() const{
    return peers.isEmpty();
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
