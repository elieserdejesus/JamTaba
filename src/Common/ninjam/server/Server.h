#ifndef _SERVER_SERVER_
#define _SERVER_SERVER_

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QList>

namespace ninjam {
namespace server {

class Server : public QObject
{
public:
    Server();
    ~Server();
    void start(quint16 port);
    void shutdown();

private slots:
    void handleNewConnection();
    void handleAcceptError(QAbstractSocket::SocketError socketError);
    void handleReceivedMessages();
    void handleDisconnection();
    void handleClientSocketError(QAbstractSocket::SocketError error);

private:
    QTcpServer tcpServer;
    QList<QTcpSocket *> clients; // connected clients

    void disconnectClient(QTcpSocket *socket);
};

} // ns server
} // ns ninjam

#endif
