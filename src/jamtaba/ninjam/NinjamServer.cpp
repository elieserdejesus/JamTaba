#include "NinjamServer.h"
#include "NinjamUser.h"
#include <QDebug>

QMap<QString, std::shared_ptr<NinjamServer>> NinjamServer::servers;

NinjamServer::NinjamServer(QString host, int port)
    :port(port), host(host),
      maxUsers(0), bpm(120), bpi(16),
      activeServer(true),
      streamUrl(""),
      topic(""),
      containBot(false)
{

    //qDebug() << "criou server " << host <<":" << port;
}


QString NinjamServer::getUniqueName(QString host, int port) {
    return host + ":" + port;
}

bool NinjamServer::containsUser(const NinjamUser &user) const
{
    return users.contains(user.getFullName());
}


NinjamServer* NinjamServer::getServer(QString host, int port) {
    QString key = getUniqueName(host, port);
    if (!servers.contains(key)) {
        servers.insert(key, std::shared_ptr<NinjamServer>(new NinjamServer(host, port)));
    }
    return servers[key].get();
}

void NinjamServer::addUser(NinjamUser* user) {
    if (!users.contains(user->getFullName())) {
        users.insert(user->getFullName(), user);
        if (user->isBot()) {
            containBot = true;
        }
    }
}

QList<NinjamUser*> NinjamServer::getUsers() const{
    return users.values();
}

bool NinjamServer::containsBotOnly() const{
    if (users.size() == 1 && containBot) {
        return true;
    }
    return false;
}

bool NinjamServer::setBpm(short bpm) {
    if (bpm == this->bpm) {
        return false;
    }

    if (bpm >= NinjamServer::MIN_BPM && bpm <= NinjamServer::MAX_BPM) {
        this->bpm = bpm;
        return true;
    }
    return false;
}

bool NinjamServer::setBpi(short bpi) {
    if (bpi == this->bpi) {
        return false;
    }

    if (bpi >= NinjamServer::MIN_BPI && bpi <= NinjamServer::MAX_BPI) {
        this->bpi = bpi;
        return true;
    }
    return false;
}

void NinjamServer::refreshUserList(QSet<NinjamUser*> onlineUsers) {
    QList<NinjamUser*> toRemove;

    foreach (NinjamUser* onlineUser , onlineUsers) {
        addUser(onlineUser);
    }

    QList<NinjamUser*> currentUsers= users.values();
    foreach (NinjamUser* user , currentUsers) {
        if (!onlineUsers.contains(user)) {
            toRemove.append(user);
        }
    }

    foreach (NinjamUser* ninjaMUser , toRemove) {
        users.remove(ninjaMUser->getFullName());
    }

}

QDataStream &operator<<(QDataStream &out, const NinjamServer &server){
    out << "NinjamServer{" << "port="  <<  server.getPort()  <<  ", host="
        <<  server.getHostName() <<  ", stream="  <<  server.getStreamUrl()
        <<" maxUsers="  <<  server.getMaxUsers() <<  ", bpm="
        <<  server.getBpm()  <<  ", bpi="  <<  server.getBpi()
        <<  ", isActive="  <<  server.isActive() <<  "}\n";
    for (NinjamUser* user : server.getUsers()) {
        out << "\t" << user->getName()  << "\n";
    }
    return out;
}
