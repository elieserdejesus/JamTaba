#ifndef LOGIN_SERVICE_H
#define LOGIN_SERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "ninjam/client/ServerInfo.h"

class NatMap;
class QTimer;

namespace login {

class UserInfo
{

public:
    UserInfo(const QString &name, const QString &ip);
    inline QString getIp() const { return ip; }
    inline QString getName() const { return name; }

private:
    QString name;
    QString ip;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++

enum class RoomTYPE {
    NINJAM, REALTIME
};


class RoomInfo
{

public:
    RoomInfo(const QString &roomName, int roomPort, int maxUsers,
             const QList<UserInfo> &users, int maxChannels, int bpi, int bpm, const QString &streamUrl);

    RoomInfo(const QString &roomName, int roomPort, int maxUsers, int maxChannels = 0);

    QString getName() const;

    bool isEmpty() const;
    bool isFull() const;

    int getPort() const;

    QList<UserInfo> getUsers() const;

    bool hasStream() const;
    QString getStreamUrl() const;

    int getMaxChannels() const;

    int getNonBotUsersCount() const;
    int getMaxUsers() const;

    int getBpm() const;
    int getBpi() const;

    QString getUniqueName() const;

protected:

    QString name;

    int port;

    int maxUsers;
    int maxChannels;

    QList<UserInfo> users;

    QString streamUrl;

    int bpi;
    int bpm;
};

inline bool RoomInfo::isFull() const
{
    return users.size() >= maxUsers;
}

inline int RoomInfo::getPort() const
{
    return port;
}

inline QList<UserInfo> RoomInfo::getUsers() const
{
    return users;
}

inline bool RoomInfo::hasStream() const
{
    return !streamUrl.isEmpty();
}

inline QString RoomInfo::getStreamUrl() const
{
    return streamUrl;
}

inline int RoomInfo::getMaxChannels() const
{
    return maxChannels;
}

inline int RoomInfo::getMaxUsers() const
{
    return maxUsers;
}

inline int RoomInfo::getBpm() const
{
    return bpm;
}

inline int RoomInfo::getBpi() const
{
    return bpi;
}

inline QString RoomInfo::getName() const
{
    return name;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++

class LoginService : public QObject
{
    Q_OBJECT

public:

    explicit LoginService(QObject *parent = 0);
    //virtual ~LoginService();

signals:
    void roomsListAvailable(const QList<login::RoomInfo> &publicRooms);

private:

    QNetworkAccessManager httpClient;
    static const QString LOGIN_SERVER_URL;

    static const int REFRESH_PERIOD = 60000;
    QTimer *refreshTimer;

    void handleJson(const QString &json);

    RoomInfo buildRoomInfoFromJson(const QJsonObject &json);
};

} // namespace

#endif
