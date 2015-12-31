#include "UserChannel.h"

#include <QDebug>

using namespace Ninjam;

UserChannel::UserChannel(const QString &userFullName, const QString &channelName, quint8 channelIndex, bool active,
                         quint16 volume, quint8 pan, quint8 flags) :
    userFullName(userFullName),
    channelName(channelName),
    active(active),
    index(channelIndex),
    volume(volume),
    pan(pan),
    flags(flags)
{
}

UserChannel::UserChannel() :
    userFullName("invalid"),
    channelName("invalid"),
    active(false),
    index(-1),
    volume(0),
    pan(0),
    flags(0)
{
}

UserChannel::UserChannel(const UserChannel &c) :
    userFullName(c.getUserFullName()),
    channelName(c.getName()),
    active(c.isActive()),
    index(c.getIndex()),
    volume(c.volume),
    pan(c.pan),
    flags(c.flags)
{
}

UserChannel::~UserChannel()
{
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
