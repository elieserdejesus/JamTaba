#include "Server.h"
#include "User.h"
#include "UserChannel.h"

using namespace Ninjam;

Server::Server(const QString &host, int port, int maxChannels, int maxUsers) :
    port(port),
    host(host),
    maxUsers(maxUsers),
    bpm(120),
    bpi(16),
    activeServer(true),
    streamUrl(""),
    topic(""),
    containBot(false),
    maxChannels(maxChannels)
{
}

Server::~Server()
{
}

bool Server::containsUser(const QString &userFullName) const
{
    return users.contains(userFullName);
}

bool Server::containsUser(const User &user) const
{
    return containsUser(user.getFullName());
}

// Server* Server::getServer(QString host, int port) {
// QString key = getUniqueName(host, port);
// if (!servers.contains(key)) {
// servers.insert(key, std::shared_ptr<Server>(new Server(host, port)));
// }
// return servers[key].get();
// }

void Server::addUser(const User &user)
{
    if (!users.contains(user.getFullName())) {
        users.insert(user.getFullName(), User(user.getFullName()));
        if (user.isBot())
            containBot = true;
    }
}

void Server::updateUserChannel(const QString &userFullName, const UserChannel &serverChannel)
{
    if(users.contains(userFullName)){
        users[userFullName].updateChannelName(serverChannel.getIndex(), serverChannel.getName());
    }
}

void Server::removeUserChannel(const QString &userFullName, int channelIndex)
{
    if(users.contains(userFullName)){
        users[userFullName].removeChannel(channelIndex);
    }
}

void Server::addUserChannel(const QString &userFullName, const UserChannel &newChannel)
{
    if(users.contains(userFullName)){
        users[userFullName].addChannel(newChannel);
    }
}

User Server::getUser(const QString &userFullName) const
{
    if (users.contains(userFullName))
        return users[userFullName];
    return User("");
}

QList<User> Server::getUsers() const
{
    return users.values();
}

bool Server::containsBotOnly() const
{
    if (users.size() == 1 && containBot)
        return true;
    return false;
}

QString Server::getUniqueName() const
{
    return host + ":" + QString::number(port);
}

bool Server::setBpm(short bpm)
{
    if (bpm == this->bpm)
        return false;

    if (bpm >= Server::MIN_BPM && bpm <= Server::MAX_BPM) {
        this->bpm = bpm;
        return true;
    }
    return false;
}

bool Server::setBpi(short bpi)
{
    if (bpi == this->bpi)
        return false;

    if (bpi >= Server::MIN_BPI && bpi <= Server::MAX_BPI) {
        this->bpi = bpi;
        return true;
    }
    return false;
}

void Server::refreshUserList(const QSet<QString> &onlineUsers)
{
    QList<QString> toRemove;

    foreach (const QString &onlineUserName, onlineUsers)
        addUser(User(onlineUserName));

    QList<User> currentUsers = users.values();
    foreach (const User &user, currentUsers) {
        if (!onlineUsers.contains(user.getFullName()))
            toRemove.append(user.getFullName());
    }

    foreach (const QString &fullName, toRemove)
        users.remove(fullName);

}

QDataStream &Ninjam::operator<<(QDataStream &out, const Server &server)
{
    out << "NinjamServer{" << "port="  <<  server.getPort()  <<  ", host="
        <<  server.getHostName() <<  ", stream="  <<  server.getStreamUrl()
        <<" maxUsers="  <<  server.getMaxUsers() <<  ", bpm="
        <<  server.getBpm()  <<  ", bpi="  <<  server.getBpi()
        <<  ", isActive="  <<  server.isActive() <<  "}\n";
    for (const User &user : server.getUsers())
        out << "\t" << user.getName()  << "\n";
    return out;
}
