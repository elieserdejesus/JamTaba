#ifndef SERVER_H
#define SERVER_H

#include <QMap>

class QString;

namespace Ninjam {
class User;
class UserChannel;

class Server
{

public:
    Server(const QString &host, int port, int maxChannels, int maxUsers = 0);

    ~Server();

    void addUserChannel(const QString &userFullName, const UserChannel& newChannel);
    void removeUserChannel(const QString &userFullName, int channelIndex);
    void updateUserChannel(const QString &userFullName, const UserChannel& serverChannel);

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

    inline int getMaxChannels() const
    {
        return maxChannels;
    }

    void setMaxChannels(int max)
    {
        this->maxChannels = max;
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

    inline void setLicence(QString licenceText)
    {
        this->licence = licenceText;
    }

    inline QString getLicence() const
    {
        return licence;
    }

    inline void setMaxUsers(unsigned int maxUsers)
    {
        this->maxUsers = maxUsers;
    }

    inline void setIsActive(bool active)
    {
        this->activeServer = active;
    }

    inline bool containsBot() const
    {
        return containBot;
    }

    bool containsUser(const User &user) const;
    bool containsUser(const QString &userFullName) const;

    void addUser(const User &user);

    inline short getBpi() const
    {
        return bpi;
    }

    inline short getBpm() const
    {
        return bpm;
    }

    inline int getMaxUsers() const
    {
        return maxUsers;
    }

    QList<User> getUsers() const;

    inline bool isActive() const
    {
        return activeServer;
    }

    inline int getPort() const
    {
        return port;
    }

    inline QString getHostName() const
    {
        return host;
    }

    bool containsBotOnly() const;

    QString getUniqueName() const;

    bool setBpm(short bpm);

    bool setBpi(short bpi);

    void refreshUserList(const QSet<QString> &onlineUsers);

    inline QString getTopic() const
    {
        return topic;
    }

    inline void setTopic(const QString &topicText)
    {
        this->topic = topicText;
    }

private:
    int port;
    QString host;
    int maxUsers;
    short bpm;
    short bpi;
    bool activeServer;
    QString streamUrl;
    QString topic;
    QString licence;
    QMap<QString, User> users;
    bool containBot;
    int maxChannels;

    static const int MIN_BPM = 40;
    static const int MAX_BPM = 400;
    static const int MAX_BPI = 64;
    static const int MIN_BPI = 3;
};

QDataStream &operator<<(QDataStream &out, const Ninjam::Server &server);
}// namespace

#endif
