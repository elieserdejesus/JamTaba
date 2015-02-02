#include "User.h"
#include "Service.h"
#include <QDebug>
#include <memory>

using namespace Ninjam;

std::map<std::string, std::shared_ptr<User>> User::users;

UserChannel::UserChannel(User *user, QString name, bool active, int channelIndex, short volume, quint8 pan, quint8 flags)
    : user(user), name(name), active(active), index(channelIndex), volume(volume), pan(pan), flags(flags)
{
    //qDebug() << "NinjamUserChannel constructor";
}


User::User(QString fullName) {
    this->fullName = fullName;
    QStringList fullNameParts = fullName.split("@");
    this->name = fullNameParts.at(0);
    if (fullNameParts.size() > 1) {
        this->ip = fullNameParts.at(1);
    } else {
        this->ip = "";
    }
   // qDebug() << "NinjamUser constructor " << fullName;
}

User::~User(){
    //qDebug() << "Destrutor NinjamUser";
}

User* User::getUser(QString userFullName) {
    if (userFullName.isNull() || userFullName.trimmed().isEmpty()) {
        return nullptr;
    }
    std::string key = userFullName.toStdString();
    if (users.count(key) == 0) {//not constains user
        users[key] = std::shared_ptr<User>(new User(userFullName));
    }
    return &(*(users[key]));
}

bool User::isBot() const {
    return Service::isBotName(getName());
}

QSet<UserChannel *> User::getChannels() const
{
    QSet<UserChannel*> channels;
    for (auto &l : this->channels.values()) {
        channels.insert(&*l);
    }

    return channels;
}

void User::addChannel(UserChannel* c) {
    this->channels[c->getIndex()].reset(c);
}

void User::removeChannel(UserChannel* userChannel) {
    this->channels.remove(userChannel->getIndex());
}


QDebug& Ninjam::operator<<(QDebug &out, const User &user)
{
    out << "NinjamUser{" << "name=" << user.getName() << ", ip=" << user.getIp()
        << ", fullName=" << user.getFullName() << "}\n";
    if (!user.hasChannels()) {
        out << "\tNenhum canal!\n";
    }
    else{
        QSet<UserChannel*> chs = user.getChannels();
        foreach (UserChannel* userChannel , chs) {
            out << "\t" << userChannel << "\n";
        }
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
