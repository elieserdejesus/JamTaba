#include "Server.h"
#include "ServerMessages.h"
#include "User.h"
#include "UserChannel.h"
#include "log/Logging.h"

#include <QSet>

using namespace ninjam::client;

Server::Server(const QString &host, quint16 port, quint8 maxChannels, quint8 maxUsers) :
    port(port),
    host(host),
    maxUsers(maxUsers),
    bpm(120),
    bpi(16),
    activeServer(true),
    streamUrl(""),
    topic(""),
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

void Server::addUser(const User &user)
{
    if (!users.contains(user.getFullName())) {
        users.insert(user.getFullName(), User(user.getFullName()));
    }
}

void Server::updateUserChannel(const UserChannel &serverChannel)
{
    QString userFullName = serverChannel.getUserFullName();
    if (users.contains(userFullName)) {
        quint8 channelIndex = serverChannel.getIndex();
        users[userFullName].updateChannelName(channelIndex, serverChannel.getName());
        users[userFullName].updateChannelReceiveStatus(channelIndex, serverChannel.isActive());
    }
}

void Server::updateUserChannelReceiveStatus(const QString &userFullName, quint8 channelIndex, bool receive)
{
    if (users.contains(userFullName)) {
        users[userFullName].updateChannelReceiveStatus(channelIndex, receive);
    }
}

void Server::removeUserChannel(const UserChannel &channel)
{
    QString userFullName = channel.getUserFullName();
    if (users.contains(userFullName)) {
        users[userFullName].removeChannel(channel.getIndex());
    }
}

void Server::removeUser(const QString &fullUserName)
{
    users.remove(fullUserName);
}

void Server::addUserChannel(const UserChannel &newChannel)
{
    QString userFullName = newChannel.getUserFullName();
    if (users.contains(userFullName)) {
        int userChannelsCount = users[userFullName].getChannelsCount();
        if (userChannelsCount < maxChannels)
            users[userFullName].addChannel(newChannel);
        else
            qCritical() << "Can't add more channels for "
                        << userFullName << "(using"
                        << QString::number(userChannelsCount)
                        << " channels). The server maxChannels is "
                        << QString::number(maxChannels);
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

QString Server::getUniqueName() const
{
    return host + ":" + QString::number(port);
}

bool Server::setBpm(quint16 bpm)
{
    if (bpm == this->bpm)
        return false;

    if (bpm >= Server::MIN_BPM && bpm <= Server::MAX_BPM) {
        this->bpm = bpm;
        return true;
    }

    return false;
}

bool Server::setBpi(quint16 bpi)
{
    if (bpi == this->bpi)
        return false;

    if (bpi >= Server::MIN_BPI && bpi <= Server::MAX_BPI) {
        this->bpi = bpi;
        return true;
    }

    return false;
}
