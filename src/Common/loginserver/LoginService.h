#ifndef LOGIN_SERVICE_H
#define LOGIN_SERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "ninjam/Server.h"

class NatMap;
class QTimer;

namespace Login {

class UserInfo
{

public:
    UserInfo(long long id, const QString &name, const QString &ip);
    inline QString getIp() const { return ip; }
    inline QString getName() const { return name; }

private:
    long long id;
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
    RoomInfo(long long id, const QString &roomName, int roomPort, RoomTYPE roomType, int maxUsers,
             const QList<UserInfo> &users, int maxChannels, int bpi, int bpm, const QString &streamUrl);

    RoomInfo(const QString &roomName, int roomPort, RoomTYPE roomType, int maxUsers, int maxChannels = 0);

    ~RoomInfo();

    QString getName() const;
    long long getID() const;

    RoomTYPE getType() const;

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

protected:
    long long id;

    QString name;

    int port;

    RoomTYPE type;

    int maxUsers;
    int maxChannels;

    QList<UserInfo> users;

    QString streamUrl;

    int bpi;
    int bpm;
};

inline RoomInfo::~RoomInfo()
{
    //
}

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

inline long long RoomInfo::getID() const
{
    return id;
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

inline RoomTYPE RoomInfo::getType() const
{
    return type;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++

class LoginService : public QObject
{
    Q_OBJECT

public:

    explicit LoginService(QObject *parent = 0);
    virtual ~LoginService();
    virtual void connectInServer(const QString &userName, int instrumentID, const QString &channelName,
                                    const NatMap &localPeerMap, const QString &version, const QString &environment, int sampleRate);

    virtual void disconnectFromServer();
    bool isConnected() const;

    QString getChordProgressionFor(const Login::RoomInfo &roomInfo) const;
    void sendChordProgressionToServer(const QString &userName, const QString &serverName, quint32 serverPort, const QString &chordProgression);

signals:
    void roomsListAvailable(const QList<Login::RoomInfo> &publicRooms);
    void incompatibilityWithServerDetected();
    void newVersionAvailableForDownload(const QString &latestVersionDetails);
    void errorWhenConnectingToServer(const QString &error);

private:

    enum Command {
        CONNECT,
        DISCONNECT,
        REFRESH_ROOMS_LIST
    };

    QNetworkAccessManager httpClient;
    QNetworkReply *pendingReply;
    static const QString LOGIN_SERVER_URL;
    bool connected;

    static const int REFRESH_PERIOD = 30000;
    QTimer *refreshTimer;

    QMap<QString, QString> lastChordProgressions;


    void handleJson(const QString &json);

    QNetworkReply *sendCommandToServer(const QUrlQuery &, bool synchronous = false);

    RoomInfo buildRoomInfoFromJson(const QJsonObject &json);

    static QString getRoomInfoUniqueName(const Login::RoomInfo &roomInfo);

private slots:
    void connectedSlot();
    void roomsListReceivedSlot();

    void errorSlot(QNetworkReply::NetworkError error);
    void connectNetworkReplySlots(QNetworkReply *reply, Command command);

    void refreshTimerSlot();
};

inline bool LoginService::isConnected() const
{
    return connected;
}

} // namespace

#endif
