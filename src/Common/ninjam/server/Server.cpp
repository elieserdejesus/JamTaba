#include "Server.h"

#include <QDebug>
#include <QDataStream>

#include "ninjam/Ninjam.h"
#include "ninjam/client/ServerMessages.h"
#include "ninjam/client/ClientMessages.h"

using ninjam::server::Server;
using ninjam::client::AuthChallengeMessage; // TODO message used both in server and client
using ninjam::client::ClientAuthUserMessage; // todo message used both in server and client
using ninjam::MessageHeader;

// -------------------------------------------------------------

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

    connect(socket, &QTcpSocket::disconnected, this, &Server::handleDisconnection);
    connect(socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), this, &Server::handleClientSocketError);

    addClient(socket);
}

void Server::addClient(QTcpSocket *socket)
{
    clients.append(socket);

    connect(socket, &QIODevice::readyRead, this, &Server::handleReceivedMessages);

    sendAuthChallenge(socket);
}

void Server::sendAuthChallenge(QTcpSocket *device)
{
    QByteArray challenge("abcdabcd");
    QString licence("No licence at moment!");

    quint32 protocolVersion = 0x00020000; // fixed value
    quint16 keepAlivePeriod = 30; // in seconds
    quint32 serverCapabilities = keepAlivePeriod << 8; // keep alive period value is stored in bytes 8-15
    if(!licence.isEmpty())
        serverCapabilities |= 1; // when server has licence the first bit is set.

    auto msg = AuthChallengeMessage(challenge, licence, serverCapabilities, protocolVersion);
    msg.to(device);
}

void Server::handleReceivedMessages()
{
    auto socket = qobject_cast<QTcpSocket *>(QObject::sender());
    if (socket) {
        qDebug() << "handleReceivedMessages();" << socket->bytesAvailable();
        while (device->bytesAvailable() >= 5) { // all messages have minimum of 5 bytes
            auto header = MessageHeader::from(socket);
            if (header.getMessageType() == 0x80) { // client auth user message
                qDebug() << "Received Client auth user message";
                auto msg = ClientAuthUserMessage::fro
            }
            else {
                qCritical() << "message code:" << header.getMessageType();
            }
        }
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
    if (tcpServer.isListening()) {
        tcpServer.close();

        for (auto socket : clients)
             socket->deleteLater();

        clients.clear();

        qDebug() << "server shutdown";
    }
}
