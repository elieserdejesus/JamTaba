#include "NinjamUser.h"
#include "NinjamService.h"
#include <QDebug>
#include <memory>



std::map<std::string, std::shared_ptr<NinjamUser>> NinjamUser::users;

NinjamUserChannel::NinjamUserChannel(NinjamUser *user, QString name, bool active, int channelIndex, short volume, quint8 pan, quint8 flags)
    : user(user), name(name), active(active), index(channelIndex), volume(volume), pan(pan), flags(flags)
{
    //qDebug() << "NinjamUserChannel constructor";
}


NinjamUser::NinjamUser(QString fullName) {
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

NinjamUser::~NinjamUser(){
    //qDebug() << "Destrutor NinjamUser";
}

NinjamUser* NinjamUser::getUser(QString userFullName) {
    if (userFullName.isNull() || userFullName.trimmed().isEmpty()) {
        return nullptr;
    }
    std::string key = userFullName.toStdString();
    if (users.count(key) == 0) {//not constains user
        users[key] = std::shared_ptr<NinjamUser>(new NinjamUser(userFullName));
    }
    return &(*(users[key]));
}

bool NinjamUser::isBot() const {
    return NinjamService::isBotName(getName());
}

QSet<NinjamUserChannel *> NinjamUser::getChannels() const
{
    QSet<NinjamUserChannel*> channels;
    for (auto &l : this->channels.values()) {
        channels.insert(&*l);
    }

    return channels;
}

void NinjamUser::addChannel(NinjamUserChannel* c) {
    this->channels[c->getIndex()].reset(c);
}

void NinjamUser::removeChannel(NinjamUserChannel* userChannel) {
    this->channels.remove(userChannel->getIndex());
}


QDebug &operator<<(QDebug &out, const NinjamUser &user)
{
    out << "NinjamUser{" << "name=" << user.getName() << ", ip=" << user.getIp()
        << ", fullName=" << user.getFullName() << "}\n";
    if (!user.hasChannels()) {
        out << "\tNenhum canal!\n";
    }
    else{
        QSet<NinjamUserChannel*> chs = user.getChannels();
        foreach (NinjamUserChannel* userChannel , chs) {
            out << "\t" << userChannel << "\n";
        }
    }
    out << "--------------\n";
    return out;
}


QDebug &operator<<(QDebug &out, const NinjamUserChannel &ch)
{
    out << "UserChannel{" << "name=" << ch.getName()<< ", active=" << ch.isActive()
        << ", index=" << ch.getIndex() << '}';
    return out;
}
