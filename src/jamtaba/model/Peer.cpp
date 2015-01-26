#include "Peer.h"
#include <QMap>
#include <QDebug>

QMap<long long, std::shared_ptr<Peer>> Peer::peers;


Peer *Peer::getPeer(QString publicIp, int publicPort, QString userName, QString region, QString country, int instrumentID, QString /*channelName*/, int version, QString environment, int sampleRate, long long id)
{
    if(!peers.contains(id)){
        peers.insert(id, std::shared_ptr<Peer>(new Peer(publicIp, publicPort, userName, region, country, instrumentID, version, environment, sampleRate, id)));
    }
    return peers[id].get();
}

Peer::Peer(QString publicIp, int publicPort, QString userName, QString region,
           QString country, int instrumentID, int version, QString environment, int sampleRate, long long id)
    :publicIp(publicIp),
     publicPort(publicPort),
     userName(userName),
     region(region),
     country(country),
     instrumentID(instrumentID ),
     version(version),
     environment(environment),
     sampleRate(sampleRate),
     id(id)

{
    //
}

Peer::~Peer()
{
    qDebug() << "Peer destructor["<<this->userName << "]";
}

