#ifndef USER_H
#define USER_H

#include "UserChannel.h"
#include <QMap>

namespace Ninjam {
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class User
{
private:
    QString fullName;
    QString name;
    QString ip;
    QMap<int, UserChannel *> channels;

public:
    explicit User(QString fullName);
    virtual ~User();

    bool isBot() const;

    inline bool hasChannels() const
    {
        return !channels.isEmpty();
    }

    inline bool hasChannel(int channelIndex) const
    {
        return this->channels.contains(channelIndex);
    }

    inline QList<UserChannel *> getChannels() const
    {
        return channels.values();
    }

    inline bool operator<(const User &other) const
    {
        return getFullName() < other.getFullName();
    }

    UserChannel getChannel(int index) const;

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

    void addChannel(UserChannel channel);
    void removeChannel(int channelIndex);
    void setChannelName(int channelIndex, QString name);
    void setChannelFlags(int channelIndex, int flags);
};

QDebug &operator<<(QDebug &out, const Ninjam::User &user);
QDebug &operator<<(QDebug &out, const Ninjam::UserChannel &user);
}

#endif
