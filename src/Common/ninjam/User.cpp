#include "User.h"
#include "UserChannel.h"
#include <QDebug>
#include <QRegularExpression>
#include "log/Logging.h"

using namespace Ninjam;

QString Ninjam::extractUserName(const QString &fullName)
{
    QChar separator('@');
    if (fullName.contains(separator))
        return fullName.split(separator).at(0);

    return fullName;
}

QString Ninjam::extractUserIP(const QString &fullName)
{
    QChar separator('@');
    if (fullName.contains(separator))
        return fullName.split(separator).at(1);

    return QString();
}


User::User(const QString &fullName) :
    fullName(fullName),
    name(Ninjam::extractUserName(fullName)),
    ip(Ninjam::extractUserIP(fullName))

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

void User::removeChannel(quint8 channelIndex)
{
    this->channels.remove(channelIndex);
}

QDebug &Ninjam::operator<<(QDebug &out, const User &user)
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
