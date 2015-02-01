#pragma once

#include <QMap>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <memory>

namespace Ninjam {

class User;

class UserChannel {
private:
    User* user;
    QString name;
    bool active;
    int index;
    short volume;//(dB gain, 0=0dB, 10=1dB, -30=-3dB, etc)
    quint8 pan;//Pan [-128, 127]
    quint8 flags;

public:
    UserChannel(User* user, QString name, bool active, int channelIndex, short volume, quint8 pan, quint8 flags);
    ~UserChannel(){
        qDebug() << "NinjamUserChannel destructor";
    }

   // ~NinjamUserChannel();

    inline bool isActive() const{ return active && flags == 0; }

    inline int getIndex() const{ return index; }

    inline QString getName() const{ return name;}

    inline User* getUser() const{ return user;}

    inline void setName(QString name) {this->name = name;}

    inline quint8 getFlags() const{ return flags;}

    inline void setFlags(quint8 flags) { this->flags = flags;}
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class User {

private:
    static std::map<std::string, std::shared_ptr<User>> users;

    QString name;
    QString ip;
    QMap<int, std::shared_ptr<UserChannel>> channels;
    QString fullName;

    User(QString fullName);


public:
    ~User();
    static User* getUser(QString userFullName) ;

    bool isBot() const ;

    inline bool hasChannels() const{ return !channels.isEmpty(); }

    QSet<UserChannel*> getChannels() const;

    inline UserChannel* getChannel(int index) const{ return &*(channels[index]);}

    inline QString getIp() const{ return ip;}

    inline QString getName() const {return name;}

    inline QString getFullName() const{return fullName;}

    void addChannel(UserChannel* c) ;

    void removeChannel(UserChannel* userChannel) ;
};

QDebug &operator<<(QDebug &out, const Ninjam::User &user);
QDebug &operator<<(QDebug &out, const Ninjam::UserChannel &user);

}
