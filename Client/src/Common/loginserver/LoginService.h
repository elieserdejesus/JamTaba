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
    UserInfo(long long id, QString name, QString ip);

    inline QString getIp() const
    {
        return ip;
    }

    inline QString getName() const
    {
        return name;
    }

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
    RoomInfo(long long id, QString roomName, int roomPort, RoomTYPE roomType, int maxUsers,
             QList<UserInfo> users, int maxChannels, int bpi, int bpm, QString streamUrl = "");

    RoomInfo(QString roomName, int roomPort, RoomTYPE roomType, int maxUsers, int maxChannels = 0);

    ~RoomInfo()
    {
    }

    inline QString getName() const
    {
        return name;
    }

    inline RoomTYPE getType() const
    {
        return type;
    }

    bool isEmpty() const;
    inline bool isFull() const
    {
        return users.size() >= maxUsers;
    }

    inline int getPort() const
    {
        return port;
    }

    inline QList<UserInfo> getUsers() const
    {
        return users;
    }

    inline bool hasStream() const
    {
        return !streamUrl.isEmpty();
    }

    inline long long getID() const
    {
        return id;
    }

    inline QString getStreamUrl() const
    {
        return streamUrl;
    }

    inline int getMaxChannels() const
    {
        return maxChannels;
    }

    int getNonBotUsersCount() const;
    inline int getMaxUsers() const
    {
        return maxUsers;
    }

    inline int getBpm() const
    {
        return bpm;
    }

    inline int getBpi() const
    {
        return bpi;
    }

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

// +++++++++++++++++++++++++++++++++++++++++++++++++++
class LoginService : public QObject
{
    Q_OBJECT

public:

    explicit LoginService(QObject *parent = 0);
    ~LoginService();
    virtual void connectInServer(QString userName, int instrumentID, QString channelName,
                                 const NatMap &localPeerMap, QString version, QString environment,
                                 int sampleRate);
    virtual void disconnectFromServer();
    inline bool isConnected() const
    {
        return connected;
    }

signals:
    void roomsListAvailable(QList<Login::RoomInfo> publicRooms);
    void incompatibilityWithServerDetected();
    void newVersionAvailableForDownload();
    void errorWhenConnectingToServer(QString error);
private:

    enum Command {
        CONNECT, DISCONNECT, REFRESH_ROOMS_LIST
    };

    QNetworkAccessManager httpClient;
    QNetworkReply *pendingReply;
    QNetworkReply *sendCommandToServer(const QUrlQuery &, bool synchronous = false);
    static const QString SERVER;
    bool connected;
    void handleJson(QString json);

    RoomInfo buildRoomInfoFromJson(QJsonObject json);

    static const int REFRESH_PERIOD = 30000;
    QTimer *refreshTimer;

private slots:
    void connectedSlot();
    void roomsListReceivedSlot();

    void errorSlot(QNetworkReply::NetworkError error);
    void connectNetworkReplySlots(QNetworkReply *reply, Command command);

    void refreshTimerSlot();
};
}

#endif
