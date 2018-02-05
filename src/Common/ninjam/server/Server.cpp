#include "Server.h"

#include <QDebug>
#include <QDataStream>

#include "ninjam/Ninjam.h"
#include "ninjam/client/ServerMessages.h"
#include "ninjam/client/ClientMessages.h"

using ninjam::server::Server;
using ninjam::client::AuthChallengeMessage; // TODO message used both in server and client
using ninjam::client::ClientAuthUserMessage; // todo message used both in server and client
using ninjam::client::ClientSetChannel; // used in both
using ninjam::client::AuthReplyMessage; // used in both client and server
using ninjam::client::ConfigChangeNotifyMessage;// used in both
using ninjam::server::RemoteUser;
using ninjam::MessageHeader;
using ninjam::MessageType;

// -------------------------------------------------------------

RemoteUser::RemoteUser(const QString &name, QTcpSocket *socket) :
    socket(socket),
    name(name)
{

}

RemoteUser::~RemoteUser()
{
    qDebug() << "~RemoteUser";
    socket->deleteLater();
}

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
    connect(socket, &QIODevice::readyRead, this, &Server::processReceivedBytes);

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

bool Server::invokeMessageHandler(const MessageHeader &header)
{
    Q_ASSERT(header.isValid());

    auto type = static_cast<MessageType>(header.getMessageType());
    switch (type) {
//    case MessageType::AUTH_CHALLENGE:
//        return handleMessage<AuthChallengeMessage>(header.getPayload());
    default:
        qCritical() << "Can't handle the message code " << QString::number(header.getMessageType());
    }

    return false;
}

void Server::processReceivedBytes()
{
    auto socket = qobject_cast<QTcpSocket *>(QObject::sender());
    if (!socket)
        return;

    while (socket->bytesAvailable() >= 5) { // all messages have minimum of 5 bytes
        auto header = MessageHeader::from(socket);
        if (socket->bytesAvailable() < header.getPayload()) {
            qCritical() << " not ENOUGH BYTES!";
            return;
        }
        switch (header.getMessageType()) {
        case MessageType::AuthReply: {
                auto msg = ClientAuthUserMessage::unserializeFrom(socket, header.getPayload());
                //igoring challenge and password for while

                qDebug() << "ClientAuthUser received, sending AuthReply";

                quint8 flag = 1; // authentication suceeded
                QString message("updateUserName"); // updated user name or error message;
                quint8 maxChannels = 2;
                AuthReplyMessage authReply(flag, message, maxChannels);
                authReply.to(socket);

                if (authReply.userIsAuthenticated()) {
                    clients.insert(socket, new RemoteUser(msg.getUserName(), socket));
                }

            break;
        }
        case MessageType::ClientAuthUser: { // Client Set Channel Info
            ClientSetChannel msg = ClientSetChannel::unserializeFrom(socket, header.getPayload());
            qDebug() << "Client Set Channel received  channels:" << msg.getChannelNames();

            // send server config change
            quint16 bpm = 121;
            quint16 bpi = 13;
            auto configChange = ConfigChangeNotifyMessage(bpm, bpi);
            configChange.to(socket);

            // send each connected user infos to everybody
            //broadcastUserChangeNotify(???);

            break;
        }
        case MessageType::KeepAlive: { // Client keep alive
            qDebug() << "Client keep alive received";
            break;
        }
        default:
            qCritical() << "not handled message code:" << header.getMessageType();
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
    if (clients.contains(socket)) {
        delete clients[socket];
        clients.remove(socket);
    }
}

void Server::handleClientSocketError(QAbstractSocket::SocketError error)
{
    qCritical() << "ERROR" << error << qobject_cast<QTcpSocket *>(QObject::sender())->errorString();
}

void Server::handleAcceptError(QAbstractSocket::SocketError socketError)
{
    qCritical() << socketError <<  tcpServer.errorString();
}

void Server::shutdown()
{
    if (tcpServer.isListening()) {
        tcpServer.close();

        clients.clear();

        qDebug() << "server shutdown";
    }
}
