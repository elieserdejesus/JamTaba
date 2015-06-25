#include "User.h"
#include "Service.h"
#include <QDebug>
#include <memory>

using namespace Ninjam;

//QMap<QString, User> User::users;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
User::User(QString fullName)
    :  fullName(fullName)
{
    QStringList fullNameParts = fullName.split("@");
    this->name = fullNameParts.at(0);
    if (fullNameParts.size() > 1) {
        this->ip = fullNameParts.at(1);
        if(this->ip.contains("x", Qt::CaseInsensitive)){
            this->ip = this->ip.replace("[xX]", "128");//just use 128 as a default ip segment
        }
    } else {
        this->ip = "";
    }
   // qDebug() << "NinjamUser constructor " << fullName;
}

//User::User(const User &u)
//    :
//      fullName(u.fullName),
//      name(u.name),
//      ip(u.ip)

//{

//}

User::~User(){
    //qDebug() << "Destrutor NinjamUser";
}

//User User::getUser(QString userFullName) {
//    if (userFullName.isNull() || userFullName.trimmed().isEmpty()) {
//        return nullptr;
//    }
//    if (users.contains(userFullName)) {//not constains user
//        users.insert(userFullName, User(userFullName));
//    }
//    return users[userFullName];
//}

bool User::isBot() const {
    return Service::isBotName(getName());
}

//QSet<UserChannel> User::getChannels() const
//{
//    return QSet<UserChannel>::fromList( channels.values());
//}

//void User::addChannel(QString channelName, bool active, int channelIndex, short volume, quint8 pan, quint8 flags) {
//    this->channels.insert(channelIndex, UserChannel(this->getFullName(), channelName, active, channelIndex, volume, pan, flags));
//}

void User::addChannel(UserChannel c){
    //addChannel(c.getName(), c.isActive(), c.getIndex(), c.volume, c.pan, c.getFlags());
    channels.insert(c.getIndex(), new UserChannel(c));
}

void User::removeChannel(int channelIndex) {
    this->channels.remove(channelIndex);
}

void User::setChannelName(int channelIndex, QString name){
    if(channels.contains(channelIndex)){
        channels[channelIndex]->setName(name);
    }
}

void User::setChannelFlags(int channelIndex, int flags){
    if(channels.contains(channelIndex)){
        channels[channelIndex]->setFlags(flags);
    }
}

QDebug& Ninjam::operator<<(QDebug &out, const User &user)
{
    out << "NinjamUser{" << "name=" << user.getName() << ", ip=" << user.getIp()
        << ", fullName=" << user.getFullName() << "}\n";
    if (!user.hasChannels()) {
        out << "\tNenhum canal!\n";
    }
    else{
//        QSet<UserChannel> chs = user.getChannels();
//        foreach (UserChannel userChannel , chs) {
//            out << "\t" << userChannel << "\n";
//        }
    }
    out << "--------------\n";
    return out;
}


QDebug& Ninjam::operator<<(QDebug &out, const UserChannel &ch)
{
    out << "UserChannel{" << "name=" << ch.getName()<< ", active=" << ch.isActive()
        << ", index=" << ch.getIndex() << '}';
    return out;
}
