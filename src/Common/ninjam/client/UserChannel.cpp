#include "UserChannel.h"

#include <QDebug>

using ninjam::client::UserChannel;

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
    //
}

UserChannel::UserChannel() :
    UserChannel("invalid", "invalid", -1, false, 0, 0, 0)
{
    //
}

UserChannel::~UserChannel()
{
    //
}

