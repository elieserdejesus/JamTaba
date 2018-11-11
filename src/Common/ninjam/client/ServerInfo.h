#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H

#include <QMap>
#include <QString>
#include "User.h"

namespace ninjam
{

namespace client
{
    class UserChannel;

    class ServerInfo
    {

    public:
        ServerInfo(const QString &host, quint16 port, quint8 maxChannels, quint8 maxUsers = 0);

        ~ServerInfo();

        void addUserChannel(const QString &userFullName, const UserChannel &newChannel);
        void removeUserChannel(const QString &userFullName, const UserChannel &channel);
        void updateUserChannel(const QString &userFullName, const UserChannel &serverChannel);
        void updateUserChannelReceiveStatus(const QString &userFullName, quint8 channelIndex, bool receive);

        User getUser(const QString &userFullName) const;

        inline void setStreamUrl(const QString &streamUrl)
        {
            this->streamUrl = streamUrl;
        }

        inline QString getStreamUrl() const
        {
            return streamUrl;
        }

        inline bool isLocalHostServer() const
        {
            return host == "localhost";
        }

        inline quint8 getMaxChannels() const
        {
            return maxChannels;
        }

        void setMaxChannels(int max)
        {
            this->maxChannels = (quint8)max;
        }

        inline bool hasStream() const
        {
            // mutantlab server is using a nsv format, I did not find a lib to handle this audio format
            return !streamUrl.isNull() && !host.toLower().contains("mutant");
        }

        inline bool isFull() const
        {
            return users.size() == maxUsers;
        }

        inline void setLicence(const QString &licenceText)
        {
            this->licence = licenceText;
        }

        inline QString getLicence() const
        {
            return licence;
        }

        inline void setMaxUsers(unsigned int maxUsers)
        {
            this->maxUsers = (quint8)maxUsers;
        }

        inline void setIsActive(bool active)
        {
            this->activeServer = active;
        }

        bool containsUser(const User &user) const;
        bool containsUser(const QString &userFullName) const;

        void addUser(const User &user);

        void removeUser(const QString &fullUserName);

        inline quint16 getBpi() const
        {
            return bpi;
        }

        inline quint16 getBpm() const
        {
            return bpm;
        }

        inline quint8 getMaxUsers() const
        {
            return maxUsers;
        }

        QList<User> getUsers() const;

        inline bool isActive() const
        {
            return activeServer;
        }

        inline quint16 getPort() const
        {
            return port;
        }

        inline QString getHostName() const
        {
            return host;
        }

        QString getUniqueName() const;

        bool setBpm(quint16 bpm);

        bool setBpi(quint16 bpi);

        inline QString getTopic() const
        {
            return topic;
        }

        inline void setTopic(const QString &topicText)
        {
            this->topic = topicText;
        }

    private:
        quint16 port;
        QString host;
        quint8 maxUsers;
        quint16 bpm;
        quint16 bpi;
        bool activeServer;
        QString streamUrl;
        QString topic;
        QString licence;
        QMap<QString, User> users;
        quint8 maxChannels;

        static const int MIN_BPM = 40;
        static const int MAX_BPM = 400;
        static const int MAX_BPI = 192;
        static const int MIN_BPI = 2;
    };

} // client ns

} // ninjam ns

#endif
