#ifndef USERCHANNEL_H
#define USERCHANNEL_H

#include <QString>
//#include <QDebug>

namespace Ninjam {

//class User;

class UserChannel{

private:
    QString userFullName;
    QString channelName;
    bool active;
    int index;
    short volume;//(dB gain, 0=0dB, 10=1dB, -30=-3dB, etc)
    quint8 pan;//Pan [-128, 127]
    quint8 flags;

public:
    UserChannel(QString userFullName, QString channelName, bool active, int channelIndex, short volume, quint8 pan, quint8 flags);
    UserChannel();
    UserChannel(const UserChannel& c);
    ~UserChannel();

   // ~NinjamUserChannel();

    inline bool operator< (const UserChannel& other){ return getIndex() < other.getIndex(); }

    inline bool isActive() const{ return active && flags == 0; }

    inline int getIndex() const{ return index; }

    inline QString getName() const{ return channelName;}

    inline QString getUserFullName() const{ return userFullName;}

    inline void setName(QString name) {this->channelName = name;}

    inline quint8 getFlags() const{ return flags;}

    inline void setFlags(quint8 flags) { this->flags = flags;}
};
}

#endif // USERCHANNEL_H
