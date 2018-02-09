#ifndef _SERVER_SERVER_
#define _SERVER_SERVER_

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QList>

#include "../Ninjam.h"
#include "ninjam/client/User.h"

namespace ninjam { namespace client {
    class ClientToServerChatMessage;
}}

namespace ninjam {

namespace server {

using ninjam::client::User;
using ninjam::client::ClientToServerChatMessage;

class RemoteUser : public User
{
public:
    RemoteUser();
    void setLastKeepAliveToNow();
    quint64 getLastKeepAliveReceived() const;
    MessageHeader getCurrentHeader() const;
    void setCurrentHeader(MessageHeader header);
    void setFullName(const QString &fullName);

private:
    MessageHeader currentHeader;
    quint64 lastKeepAliveReceived;
};

inline void RemoteUser::setCurrentHeader(MessageHeader header)
{
    currentHeader = header;
}

inline MessageHeader RemoteUser::getCurrentHeader() const
{
    return currentHeader;
}

inline quint64 RemoteUser::getLastKeepAliveReceived() const
{
    return lastKeepAliveReceived;
}

class Server : public QObject
{
public:
    Server();
    virtual ~Server();
    virtual void start(quint16 port);
    void shutdown();

    quint16 getBpi() const;
    quint16 getBpm() const;
    QString getTopic() const;
    QString getLicence() const;
    quint8 getMaxUsers() const;
    quint8 getMaxChannels() const;

protected:
    void sendAuthChallenge(QTcpSocket *device);

protected slots:
    virtual void handleNewConnection();
    void handleAcceptError(QAbstractSocket::SocketError socketError);
    void processReceivedBytes();
    void handleDisconnection();
    void handleClientSocketError(QAbstractSocket::SocketError error);

private:
    QTcpServer tcpServer;
    QMap<QTcpSocket *, RemoteUser> remoteUsers; // connected clients

    quint16 bpm;
    quint16 bpi;
    QString topic;
    QString licence;
    quint8 maxUsers;
    quint8 maxChannels;
    quint16 keepAlivePeriod;

    void broadcastUserChangeNotify(const RemoteUser &user);
    void broadcastPublicChatMessage(const ClientToServerChatMessage &receivedMessage, const QString &userFullName);

    void processClientAuthUserMessage(QTcpSocket *socket, const MessageHeader &header);
    void processClientSetChannel(QTcpSocket *socket, const MessageHeader &header);
    void processUploadIntervalBegin(QTcpSocket *socket, const MessageHeader &header);
    void processUploadIntervalWrite(QTcpSocket *socket, const MessageHeader &header);
    void processChatMessage(QTcpSocket *socket, const MessageHeader &header);
    void processKeepAlive(QTcpSocket *socket, const MessageHeader &header);
    void processClientSetUserMask(QTcpSocket *socket, const MessageHeader &header);

    void disconnectClient(QTcpSocket *socket);

    QString generateUniqueUserName(const QString &userName) const; // return sanitized and unique username

    void updateKeepAliveInfos();

    static QHostAddress getBestHostAddress();
};

inline quint8 Server::getMaxChannels() const
{
    return maxChannels;
}

inline quint8 Server::getMaxUsers() const
{
    return maxUsers;
}

inline QString Server::getLicence() const
{
    return licence;
}

inline QString Server::getTopic() const
{
    return topic;
}

inline quint16 Server::getBpi() const
{
    return bpi;
}

inline quint16 Server::getBpm() const
{
    return bpm;
}

} // ns server
} // ns ninjam

#endif
