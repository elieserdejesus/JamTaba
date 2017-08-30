#ifndef USER_H
#define USER_H

#include <QMap>
#include "UserChannel.h"

namespace Ninjam {

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class User
{

public:
    explicit User(const QString &fullName = "");
    virtual ~User();

    inline bool hasChannels() const
    {
        return !channels.isEmpty();
    }

    inline bool hasChannel(int channelIndex) const
    {
        return this->channels.contains(channelIndex);
    }

    inline QList<UserChannel> getChannels() const
    {
        return channels.values();
    }

    inline bool operator<(const User &other) const
    {
        return getFullName() < other.getFullName();
    }

    UserChannel getChannel(quint8 index) const;

    inline QString getIp() const
    {
        return ip;
    }

    inline QString getName() const
    {
        return name;
    }

    inline QString getFullName() const
    {
        return fullName;
    }

    void updateChannelName(quint8 channelIndex, const QString &newName);
    void updateChannelReceiveStatus(quint8 channelIndex, bool receiving);

    void addChannel(const UserChannel &channel);
    void removeChannel(quint8 channelIndex);

    inline int getChannelsCount() const
    {
        return channels.size();
    }

private:
    QString fullName;
    QString name;
    QString ip;
    QMap<int, UserChannel> channels;
};

QDebug &operator<<(QDebug &out, const Ninjam::User &user);

}

#endif
