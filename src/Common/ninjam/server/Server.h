#ifndef _SERVER_SERVER_
#define _SERVER_SERVER_

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QList>

#include "../Ninjam.h"

namespace ninjam {

namespace server {

class RemoteUser
{
public:
    RemoteUser(const QString &name, QTcpSocket *socket);
    ~RemoteUser();

    inline QString getName() const
    {
        return name;
    }

private:
    QTcpSocket *socket;
    QString name;
};

class Server : public QObject
{
public:
    Server();
    virtual ~Server();
    virtual void start(quint16 port);
    void shutdown();

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

    QMap<QTcpSocket *, RemoteUser *> clients; // connected clients

    void broadcastUserChangeNotify();
    bool invokeMessageHandler(const MessageHeader &header);

    void disconnectClient(QTcpSocket *socket);
};

} // ns server
} // ns ninjam

#endif
