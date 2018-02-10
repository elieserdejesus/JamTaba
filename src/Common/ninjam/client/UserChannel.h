#ifndef USERCHANNEL_H
#define USERCHANNEL_H

#include <QtGlobal>
#include <QString>

namespace ninjam
{

namespace client
{

    class UserChannel
    {

    public:
        UserChannel(const QString &channelName, quint8 channelIndex,
                    bool active=true, quint16 volume=0, quint8 pan=0, quint8 flags=0);
        UserChannel();

        ~UserChannel();

        inline bool operator<(const UserChannel &other)
        {
            return getIndex() < other.getIndex();
        }

        inline bool isActive() const
        {
            return active;
        }

        inline quint8 getIndex() const
        {
            return index;
        }

        inline QString getName() const
        {
            return channelName;
        }

        inline void setName(const QString &name)
        {
            this->channelName = name;
        }

        inline void setActive(bool active)
        {
            this->active = active;
        }

        inline quint8 getFlags() const
        {
            return flags;
        }

        inline quint16 getVolume() const
        {
            return volume;
        }

        inline quint8 getPan() const
        {
            return pan;
        }

    private:
        QString channelName;
        bool active;
        quint8 index;
        quint16 volume; // (dB gain, 0=0dB, 10=1dB, -30=-3dB, etc)
        quint8 pan; // Pan [-128, 127]
        quint8 flags; // received from server. Possible values: 0 - ninjam interval based channel, 2 - voice chat channel, 4 - session mode
    };

} // ns
} // ns

#endif // USERCHANNEL_H
