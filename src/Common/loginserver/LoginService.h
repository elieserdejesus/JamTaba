#ifndef LOGIN_SERVICE_H
#define LOGIN_SERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "ninjam/client/ServerInfo.h"

class NatMap;
class QTimer;

namespace login {

struct Location {
    float latitude = 0;
    float longitude = 0;
    QString countryName;
    QString countryCode;
};

class UserInfo
{

public:
    UserInfo(const QString &name, const QString &ip, const QString &countryName, const QString &countryCode, float latitude, float longitude);
    inline QString getIp() const { return ip; }
    inline QString getName() const { return name; }
    inline Location getLocation() const { return location; }

private:
    QString name;
    QString ip;
    Location location;
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
    bool isPrivateServer() const { return isPrivate; }

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

    void setPreferredUserCredentials(const QString userName, const QString userPass = QString());

    bool hasPreferredUserCredentials() const;

    QString getPreferredUserName() const { return userCredentials.name; }
    QString getPreferredUserPass() const { return userCredentials.pass; }

protected:

    QString name;

    int port;

    int maxUsers;
    int maxChannels;

    QList<UserInfo> users;

    QString streamUrl;

    int bpi;
    int bpm;

    struct UserCredentials {
        QString name;
        QString pass;
    };

    UserCredentials userCredentials;

    bool isPrivate;
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
    void newVersionAvailableForDownload(const QString &newVersion, const QString &publicationDate, const QString &versionDetails);

private:

    QNetworkAccessManager httpClient;
    static const QString LOGIN_SERVER_URL;
    static const QString VERSION_SERVER_URL;

    static const int REFRESH_PERIOD = 60000;
    QTimer *refreshTimer;

    void handleJson(const QString &json);
    void handleServersJson(const QJsonObject &root);
    void handleVersionJson(const QJsonObject &root);

    RoomInfo buildRoomInfoFromJson(const QJsonObject &json);
};

} // namespace

#endif
