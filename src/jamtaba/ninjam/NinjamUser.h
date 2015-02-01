#pragma once

#include <QMap>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <memory>

class NinjamUser;

class NinjamUserChannel {
private:
    NinjamUser* user;
    QString name;
    bool active;
    int index;
    short volume;//(dB gain, 0=0dB, 10=1dB, -30=-3dB, etc)
    quint8 pan;//Pan [-128, 127]
    quint8 flags;

public:
    NinjamUserChannel(NinjamUser* user, QString name, bool active, int channelIndex, short volume, quint8 pan, quint8 flags);
    ~NinjamUserChannel(){
        qDebug() << "NinjamUserChannel destructor";
    }

   // ~NinjamUserChannel();

    inline bool isActive() const{ return active && flags == 0; }

    inline int getIndex() const{ return index; }

    inline QString getName() const{ return name;}

    inline NinjamUser* getUser() const{ return user;}

    inline void setName(QString name) {this->name = name;}

    inline quint8 getFlags() const{ return flags;}

    inline void setFlags(quint8 flags) { this->flags = flags;}
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class NinjamUser {

private:
    static std::map<std::string, std::shared_ptr<NinjamUser>> users;

    QString name;
    QString ip;
    QMap<int, std::shared_ptr<NinjamUserChannel>> channels;
    QString fullName;

    NinjamUser(QString fullName);


public:
    ~NinjamUser();
    static NinjamUser* getUser(QString userFullName) ;

    bool isBot() const ;

    inline bool hasChannels() const{ return !channels.isEmpty(); }

    QSet<NinjamUserChannel*> getChannels() const;

    inline NinjamUserChannel* getChannel(int index) const{ return &*(channels[index]);}

    inline QString getIp() const{ return ip;}

    inline QString getName() const {return name;}

    inline QString getFullName() const{return fullName;}

    void addChannel(NinjamUserChannel* c) ;

    void removeChannel(NinjamUserChannel* userChannel) ;
};

QDebug &operator<<(QDebug &out, const NinjamUser &user);
QDebug &operator<<(QDebug &out, const NinjamUserChannel &user);

