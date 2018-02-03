#include "Server.h"

#include <QDebug>
#include <QDataStream>

#include "ninjam/client/ServerMessages.h"

using ninjam::server::Server;

Server::Server()
{
    connect(&tcpServer, &QTcpServer::newConnection, this, &Server::handleNewConnection);
    connect(&tcpServer, &QTcpServer::acceptError, this, &Server::handleAcceptError);
}

Server::~Server()
{
    shutdown();
}

void Server::start(quint16 port)
{
    shutdown();

    bool listening = tcpServer.listen(QHostAddress::AnyIPv4, port);
    if (listening)
        qDebug() << "Server listening port:" << tcpServer.serverPort();
    else
        qCritical() << tcpServer.errorString();
}

void Server::handleNewConnection()
{
    QTcpSocket *socket = tcpServer.nextPendingConnection();
    if (!socket)
        return;

    qCritical() << "new connection from" << socket->peerAddress().toString();

    clients.append(socket);

//    QByteArray buffer;
//    QDataStream stream(&buffer, QIODevice::WriteOnly);
//    stream.setByteOrder(QDataStream::LittleEndian);

//    ninjam::client::ServerAuthChallengeMessage authChallenge(0); // tenho que ver esse construtor recebendo o payload
//    authChallenge.serializeTo(stream);

//    int writed = socket->write(buffer.data(), buffer.size());
//    qDebug() << writed << " bytes writed in socket";

    connect(socket, &QTcpSocket::readyRead, this, &Server::handleReceivedMessages);
    connect(socket, &QTcpSocket::disconnected, this, &Server::handleDisconnection);
    connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), this, &Server::handleClientSocketError);
}

void Server::handleReceivedMessages()
{
    auto socket = qobject_cast<QTcpSocket *>(QObject::sender());
    if (socket) {
        qDebug() << "receiving messages" << socket->bytesAvailable() << "bytes available";
    }

}

void Server::handleDisconnection()
{
    qDebug() << "handling disconnection";
    auto socket = qobject_cast<QTcpSocket *>(QObject::sender());
    if (socket)
        disconnectClient(socket);
}

void Server::disconnectClient(QTcpSocket *socket)
{
    if (clients.removeOne(socket)) {
        qDebug() << "socket" << socket->peerAddress() << "removed";
    }
}

void Server::handleClientSocketError(QAbstractSocket::SocketError error)
{
    qCritical() << "ERROR" << error;
}

void Server::handleAcceptError(QAbstractSocket::SocketError socketError)
{
    qCritical() << socketError <<  tcpServer.errorString();
}

void Server::shutdown()
{
    tcpServer.close();

    for (auto socket : clients)
        socket->deleteLater();

    clients.clear();

    qDebug() << "server shutdown";
}
