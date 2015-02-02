#pragma once

#include <QMap>
#include <QList>
#include <QString>
#include <QSet>


namespace Ninjam {
    class Server;
}


namespace Model {

class Peer;

class JamRoomListener{
public:
    virtual void peerRemoved(const Peer&) = 0;
    virtual void peerAdded(const Peer&) = 0;
    virtual void nameChanged() = 0;
};

class AbstractJamRoom {

private:
    static const int WAITING_ROOM_ID = 1;
    const long long id;
    QMap<long long, Peer*> peers;
    void refreshPeerList(const QMap<long long, Peer *>& peersInServer);
protected:
    AbstractJamRoom(long long roomID);
    QString roomName;
    //QString originalName;
    int maxUsers;

    QSet<JamRoomListener*> listeners;
    void firePeerRemoved(const Peer &removedPeer) ;
    void firePeerAdded(const Peer& newPeer) ;
    void fireNameChanged();
    virtual ~AbstractJamRoom(){}
public:
    inline bool isTheWaitingRoom() const {return id == WAITING_ROOM_ID; }
    bool containsPeer(long peerId) const;
    void addPeer(Peer *peer) ;
    void removePeer(Peer* peer) ;
    inline int getMaxUsers() const { return maxUsers; }

    void addListener(JamRoomListener &jamRoomListener) ;
    void removeListener(JamRoomListener &l) ;

    void set(QString name, bool isStatic, int maxUsers, const QMap<long long, Peer*>& peers );

    bool isFull() const ;

    inline long long getId() const {
        return id;
    }

    QList<Peer *> getPeers() const;
    
    QSet<Peer *> getPeersAsSet() const;

    /***
     * 
     * @param localPeer - Skip local peer
     * @return the reachable peers list skipping local peer
     */
    QList<Peer*> getReachablePeers(Peer *localPeer) const;
    
    QList<Peer *> getReachablePeers() const;

    virtual int getPeersCount() const;

    bool hasPeers() const;

    inline QString getName() const{
        return roomName;
    }

    //void setName(QString roomName);

    const Peer *getPeer(long long peerID) const;

    bool isEmpty() const;
};

//++++++++++++++++++++++++++++++++=

class RealTimeRoom : public AbstractJamRoom{
public:
    RealTimeRoom(long long id);
    ~RealTimeRoom();
};

//++++++++++++++++++++++++++++++++=

class NinjamRoom : public AbstractJamRoom {

private:
    const Ninjam::Server* ninjamServer;

public:
    NinjamRoom(long long ID, Ninjam::Server* ninjamServer);
    bool containBot() const;
    bool isFull() const;
    QString getTopic() const;
    bool hasStreamLink() const;
    QString getStreamLink() const;
    QString getHostName() const;
    int getHostPort() const;
    bool isEmpty() const;
    QString getName() const;
    int getMaxUsers() const ;
    int getBpm() const;
    int getBpi() const;

    virtual int getPeersCount() const;
};

}
