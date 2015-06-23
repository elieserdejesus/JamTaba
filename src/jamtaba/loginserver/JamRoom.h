#pragma once

#include <QMap>
#include <QList>
#include <QString>
#include <QSet>
#include <memory>
#include <QJsonObject>

namespace Ninjam {
    class Server;
}

namespace Login {


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class AbstractPeer{
public:
    explicit AbstractPeer(long long ID);
    virtual ~AbstractPeer();

    virtual void updateFromJson(QJsonObject json) = 0;

    inline long long getId() const {return id;}
    inline QString getName() const {return name;}
    inline QString getIP() const {return ip;}
    virtual bool isBot() const = 0;
    virtual QString getCountryCode() const {return countryCode;}
    virtual QString getCountryName() const {return countriesMap[getCountryCode().toUpper()];}

    static AbstractPeer* getByIP(QString ip);
protected:
    //all created instances
    static QMap<long long, std::shared_ptr<AbstractPeer>> peers;
    //static QMap<QString, QString> countriesMap;
    //static const std::pair<std::string, std::string> countriesMap[];
    static const QMap<QString, QString> countriesMap;
    long long id;
    QString ip;
    QString name;
    QString countryCode;

};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
class RealTimePeer : public AbstractPeer
{
public:
    bool isReachable() const {return true;}
    ~RealTimePeer(){}
    virtual void updateFromJson(QJsonObject json);
    virtual bool isBot() const { return false; }
protected:
    explicit RealTimePeer(long long ID);

private:
    int port;
    int jamtabaVersion;
    QString environment;
    int sampleRate;

};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class NinjamPeer : public AbstractPeer{
public:
    explicit NinjamPeer(long long ID);
    ~NinjamPeer(){}
    virtual void updateFromJson(QJsonObject json);
    virtual bool isBot() const;

};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class AbstractJamRoom {

private:
    static const int WAITING_ROOM_ID = 1;
    const long long id;
    AbstractJamRoom(): id(0){}//private constructor to avoid QMap create ghost instances
protected:
    QString roomName;
    int maxUsers;
    QMap<long long, std::shared_ptr<AbstractPeer>> peers;

public:
    explicit AbstractJamRoom(long long id);
    virtual ~AbstractJamRoom();
    enum class Type{NINJAM, REALTIME};


    virtual bool hasStreamLink() const = 0;

    virtual QString getStreamLink() const= 0;

    virtual Type getRoomType() const = 0;

    inline bool isTheWaitingRoom() const {return id == WAITING_ROOM_ID; }
    virtual bool containsPeer(long long peerId) const;
    //virtual void addPeer(AbstractPeer *peer) ;
    virtual void removePeer(AbstractPeer* peer) ;
    inline int getMaxUsers() const { return maxUsers; }

    virtual bool updateFromJson(QJsonObject json) = 0;

    virtual bool isFull() const ;

    inline long long getId() const {
        return id;
    }

    virtual QList<AbstractPeer *> getPeers() const;

    virtual int getPeersCount() const;

    virtual bool hasPeers() const;

    virtual QString getName() const;

    const AbstractPeer *getPeer(long long peerID) const;

    virtual bool isEmpty() const;
};

bool jamRoomLessThan(AbstractJamRoom* r1, AbstractJamRoom* r2);

//++++++++++++++++++++++++++++++++=

class RealTimeRoom : public AbstractJamRoom{
public:
    explicit RealTimeRoom(long long id);
    ~RealTimeRoom();
    virtual bool updateFromJson(QJsonObject json);
    inline bool hasStreamLink() const{return false;}
    inline QString getStreamLink() const{ return "";}
    inline AbstractJamRoom::Type getRoomType() const {return AbstractJamRoom::Type::REALTIME;}
    /***
     * @param localPeer - Skip local peer
     * @return the reachable peers list skipping local peer
     */
    QList<RealTimePeer*> getReachablePeers(AbstractPeer *localPeer) const;
    QList<RealTimePeer *> getReachablePeers() const;

};

//++++++++++++++++++++++++++++++++=

class NinjamRoom : public AbstractJamRoom {

private:
    bool containBotPeer;
    QString streamLink;
    QString hostName;
    int hostPort;
    static QMap<QString, NinjamRoom*> ninjamRooms;
    QString getMapKey() const;
    static QString buildMapKey(QString hostName, int hostPort);
    int currentBpi;
    int currentBpm;
public:
    explicit NinjamRoom(long long ID);
    NinjamRoom(QString host, int port, int maxUsers);
    NinjamRoom(QString host, int port, int maxUsers, int initialBpi, int intialBpm);
    ~NinjamRoom();
    inline int getCurrentBpi() const{return currentBpi;}
    inline int getCurrentBpm() const{return currentBpm;}
    virtual bool updateFromJson(QJsonObject json) ;
    inline AbstractJamRoom::Type getRoomType() const {return AbstractJamRoom::Type::NINJAM; }
    inline bool containBot() const{return containBotPeer;}
    //bool isFull() const;
    bool hasStreamLink() const;
    QString getStreamLink() const;
    inline QString getHostName() const {return hostName;}
    inline int getHostPort() const{return hostPort;}
    bool isEmpty() const;
    virtual QString getName() const;//hostName:port = ninbot.com:2049
    inline int getMaxUsers() const {return maxUsers;}
    //inline void setNinjamServer(Ninjam::Server* server){this->ninjamServer = server;}
    //inline Ninjam::Server* getNinjamServer() const{return this->ninjamServer;}
    //virtual int getPeersCount() const;

    static NinjamRoom* getNinjamRoom(const Ninjam::Server *server );
};

}//end of namespace
