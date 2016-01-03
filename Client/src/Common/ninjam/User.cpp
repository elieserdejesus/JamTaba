#include "User.h"
#include "UserChannel.h"
#include "Service.h"
#include <QDebug>
#include <QRegularExpression>

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

bool User::isBot() const
{
    return Service::isBotName(getName());
}

UserChannel User::getChannel(int index) const
{
    if (channels.contains(index))
        return channels[index];
    return UserChannel();// return a invalid/empty channel
}

void User::addChannel(const UserChannel &channel)
{
    channels.insert(channel.getIndex(), UserChannel(channel));
}

void User::updateChannelName(int channelIndex, const QString &newName){
    if(channels.contains(channelIndex)){
        channels[channelIndex].setName(newName);
    }
}

void User::removeChannel(int channelIndex)
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

QDebug &Ninjam::operator<<(QDebug &out, const UserChannel &ch)
{
    out << "UserChannel{"
        << "name="
        << ch.getName()
        << ", active="
        << ch.isActive()
        << ", index="
        << ch.getIndex() << '}';
    return out;
}
