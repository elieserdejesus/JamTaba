#pragma once

#include <QMap>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <memory>
#include "UserChannel.h"

namespace Ninjam {

class UserChannel;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class User {

private:
    QString fullName;
    QString name;
    QString ip;
    QMap<int, UserChannel> channels;

public:
    User(QString fullName);
    User(const User& u);
    ~User();
    //static User getUser(QString userFullName) ;

    bool isBot() const ;

    inline bool hasChannels() const{ return !channels.isEmpty(); }
    inline bool hasChannel(int channelIndex) const{ return this->channels.contains(channelIndex); }
    inline QList<UserChannel> getChannels() const{return channels.values();}
    inline bool operator< (const User& other) const{ return getFullName() < other.getFullName(); }

    //QSet<UserChannel> getChannels() const;



    inline UserChannel getChannel(int index) const{ return channels[index];}

    inline QString getIp() const{ return ip;}

    inline QString getName() const {return name;}

    inline QString getFullName() const{return fullName;}

    //void addChannel(QString channelName, bool active, int channelIndex, short volume, quint8 pan, quint8 flags) ;
    void addChannel(UserChannel channel);


    void removeChannel(int channelIndex) ;
};

QDebug &operator<<(QDebug &out, const Ninjam::User &user);
QDebug &operator<<(QDebug &out, const Ninjam::UserChannel &user);

}
