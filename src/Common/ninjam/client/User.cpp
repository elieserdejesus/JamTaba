#include "User.h"
#include "UserChannel.h"
#include <QDebug>
#include <QRegularExpression>
#include "log/Logging.h"

using ninjam::client::User;
using ninjam::client::UserChannel;

QString ninjam::client::extractUserName(const QString &fullName)
{
    QChar separator('@');
    if (fullName.contains(separator))
        return fullName.split(separator).at(0);

    return fullName;
}

QString ninjam::client::extractUserIP(const QString &fullName)
{
    QChar separator('@');
    if (fullName.contains(separator))
        return fullName.split(separator).at(1);

    return QString();
}

QString ninjam::client::maskIpInUserFullName(const QString &userFullName)
{
    auto name = extractUserName(userFullName);
    auto ip = extractUserIP(userFullName);

    if (ipIsMasked(ip))
        return userFullName;

    return QString("%1@%2")
           .arg(name)
           .arg(maskIP(ip));
}

QString ninjam::client::maskIP(const QString &ip)
{
    // mask ip if necessary to be compatible with ninjam masked IPs
    if (!ip.isEmpty() && !ip.endsWith(ninjam::client::IP_MASK)) { // not masked?
        auto index = ip.lastIndexOf(".");
        if (index) return ip.left(index) + ninjam::client::IP_MASK; // masking
    }

    return ip;
}

bool ninjam::client::ipIsMasked(const QString &ip)
{
    return ip.endsWith(ninjam::client::IP_MASK);
}

User::User(const QString &fullName) :
    fullName(fullName),
    name(ninjam::client::extractUserName(fullName)),
    ip(ninjam::client::extractUserIP(fullName))
{
}

User::~User()
{
    //
}

UserChannel User::getChannel(quint8 index) const
{
    if (channels.contains(index))
        return channels[index];

    qCritical() << "invalid channel index (" << QString::number(index) << "), returning an empty channel!";

    return UserChannel(); // return a invalid/empty channel
}

void User::addChannel(const UserChannel &channel)
{
    channels.insert(channel.getIndex(), UserChannel(channel));
}

void User::updateChannelName(quint8 channelIndex, const QString &newName)
{
    if (channels.contains(channelIndex))
        channels[channelIndex].setName(newName);
    else
        qCritical() << "invalid channel index (" << QString::number(channelIndex) << "), can't update the channel!";
}

void User::updateChannelReceiveStatus(quint8 channelIndex, bool receiving)
{
    if (channels.contains(channelIndex))
        channels[channelIndex].setActive(receiving);
    else
        qCritical() << "invalid channel index (" << QString::number(channelIndex) << "), can't update the channel!";
}

bool User::hasActiveChannels() const
{
    for (const auto &channel : channels.values()) {
        if (channel.isActive())
            return true;
    }

    return false;
}

void User::removeChannel(quint8 channelIndex)
{
    this->channels.remove(channelIndex);
}

QDebug &ninjam::client::operator<<(QDebug &out, const User &user)
{
    out << "NinjamUser{"
        << "name=" << user.getName()
        << ", ip=" << user.getIp()
        << ", fullName=" << user.getFullName()
        << "}\n";

    if (!user.hasChannels())
        out << "\tNo channels!\n";

    out << "--------------\n";

    return out;
}
