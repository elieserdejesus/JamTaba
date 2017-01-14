#include "User.h"
#include "UserChannel.h"
#include <QDebug>
#include <QRegularExpression>
#include "log/Logging.h"

using namespace Ninjam;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
User::User(const QString &fullName) :
    fullName(fullName)
{
    QStringList fullNameParts = fullName.split("@");
    this->name = fullNameParts.at(0);
    if (fullNameParts.size() > 1) {
        this->ip = fullNameParts.at(1);
        if (this->ip.contains("x", Qt::CaseInsensitive))
            this->ip = this->ip.replace(QRegularExpression("[xX]+"), "128");// just use 128 as a default ip segment
    } else {
        this->ip = "";
    }
}

User::~User()
{
}

UserChannel User::getChannel(quint8 index) const
{
    if (channels.contains(index))
        return channels[index];

    qCritical() << "invalid channel index (" << QString::number(index) << "), returning an empty channel!";
    return UserChannel();// return a invalid/empty channel
}

void User::addChannel(const UserChannel &channel)
{
    channels.insert(channel.getIndex(), UserChannel(channel));
}

void User::updateChannelName(quint8 channelIndex, const QString &newName){
    if(channels.contains(channelIndex))
        channels[channelIndex].setName(newName);
    else
        qCritical() << "invalid channel index (" << QString::number(channelIndex) << "), can't update the channel!";
}

void User::updateChannelReceiveStatus(quint8 channelIndex, bool receiving)
{
    if(channels.contains(channelIndex))
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
        << "name="
        << user.getName()
        << ", ip="
        << user.getIp()
        << ", fullName="
        << user.getFullName()
        << "}\n";

    if (!user.hasChannels())
        out << "\tNo channels!\n";
    out << "--------------\n";
    return out;
}
