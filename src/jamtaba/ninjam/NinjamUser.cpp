#include "NinjamUser.h"
#include "NinjamService.h"

QMap<QString, NinjamUser*> NinjamUser::users;

NinjamUser::NinjamUser(QString fullName) {
    this->fullName = fullName;
    QStringList fullNameParts = fullName.split("@");
    this->name = fullNameParts.at(0);
    if (fullNameParts.size() > 1) {
        this->ip = fullNameParts.at(1);
    } else {
        this->ip = "";
    }
    //System.out.println("criando user para fullname " + fullName);
    //this->channels = new HashMap<Integer, UserChannel>();
}


NinjamUser* NinjamUser::getUser(QString userFullName) {
    if (userFullName.isNull() || userFullName.trimmed().isEmpty()) {
        return nullptr;
    }
    if (!users.contains(userFullName)) {
        NinjamUser* user = new NinjamUser(userFullName);
        users.insert(userFullName, user);
    }
    return users[userFullName];
}

bool NinjamUser::isBot() const {
    return NinjamService::isBotName(getName());
}

QSet<NinjamUserChannel *> NinjamUser::getChannels() const
{
    return QSet<NinjamUserChannel*>::fromList(channels.values());
}

void NinjamUser::addChannel(NinjamUserChannel* c) {
    if (!channels.contains(c->getIndex())) {
        this->channels.insert(c->getIndex(), c);
    }
}

void NinjamUser::removeChannel(NinjamUserChannel* userChannel) {
    this->channels.remove(userChannel->getIndex());
}


QDataStream &operator<<(QDataStream &out, const NinjamUser &user)
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


QDataStream &operator<<(QDataStream &out, const NinjamUserChannel &ch)
{
    out << "UserChannel{" << "name=" << ch.getName()<< ", active=" << ch.isActive()
        << ", index=" << ch.getIndex() << '}';
    return out;
}
