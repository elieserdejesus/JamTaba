#pragma once

#include <QString>
#include <QMap>
#include <memory>

class Peer
{
public:

    static Peer* getPeer(QString publicIp, int publicPort, QString userName,
                         QString region, QString country, int instrumentID,
                         QString channelName, int version, QString environment,
                         int sampleRate, long long id) ;

    inline long long getId() const {return id;}
    inline QString getUserName() const {return userName;}
    bool isReachable() const {return true;}
~Peer();

protected:
    Peer(QString publicIp, int publicPort, QString userName, QString region,
    QString country, int instrumentID, int version, QString environment,
    int sampleRate, long long id);

private:
    QString publicIp;
    int publicPort;
    QString userName;
    QString region, country;
    int instrumentID, version;
    QString environment;
    int sampleRate;
    long long id;

    //all created instances
    static QMap<long long, std::shared_ptr<Peer>> peers;
};

