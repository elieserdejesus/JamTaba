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
        UserChannel(const QString &channelName, quint8 channelIndex, quint8 flags, bool active, quint16 volume=0, quint8 pan=0);
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

        inline bool isIntervalicChannel() const {
            return flags == 0;
        }

        inline bool isVoiceChatChannel() const {
            return flags == 2;
        }

        inline quint8 getFlags() const
        {
            return flags;
        }

        inline void setFlags(quint8 flags){
            this->flags = flags;
        }

        inline quint16 getVolume() const
        {
            return volume;
        }

        inline quint8 getPan() const
        {
            return pan;
        }

        QString toString() const {
            return QString("Channel name: %1 \\n Active: %2 \\n Index: %3 \\n Volume: %4 \\n Pan: %5 \\n Flags: %6")
                    .arg(channelName)
                    .arg(active)
                    .arg(index)
                    .arg(volume)
                    .arg(pan)
                    .arg(flags);
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
