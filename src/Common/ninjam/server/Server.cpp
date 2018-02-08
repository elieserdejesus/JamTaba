#include "Server.h"

#include <QDebug>
#include <QDataStream>
#include <QRegularExpression>
#include <QNetworkInterface>
#include <QDateTime>

#include "ninjam/Ninjam.h"
#include "ninjam/client/ServerMessages.h"
#include "ninjam/client/ClientMessages.h"
#include "ninjam/client/User.h"
#include "ninjam/client/UserChannel.h"

using ninjam::server::Server;
using ninjam::client::AuthChallengeMessage;     // TODO message used both in server and client
using ninjam::client::ClientAuthUserMessage;    // todo message used both in server and client
using ninjam::client::ClientSetChannel;         // used in both
using ninjam::client::AuthReplyMessage;         // used in both client and server
using ninjam::client::ConfigChangeNotifyMessage;// used in both
using ninjam::client::UserInfoChangeNotifyMessage; // used in both
using ninjam::client::ServerToClientChatMessage;        // used in both
using ninjam::client::DownloadIntervalBegin;    // BOTH
using ninjam::client::DownloadIntervalWrite;    //BOTH
using ninjam::client::UploadIntervalBegin;      // BOTH
using ninjam::client::UploadIntervalWrite;      //BOTH
using ninjam::client::ServerKeepAliveMessage;
using ninjam::client::ClientToServerChatMessage;
using ninjam::client::ClientKeepAlive;
using ninjam::client::UserChannel;              // used in both
using ninjam::client::User;                     // both
using ninjam::server::RemoteUser;
using ninjam::MessageHeader;
using ninjam::MessageType;

RemoteUser::RemoteUser() :
    lastKeepAliveReceived(QDateTime::currentMSecsSinceEpoch()),
    currentHeader(MessageHeader())
{

}

void RemoteUser::setFullName(const QString &fullName)
{
    this->fullName = fullName;
    this->name = ninjam::client::extractUserName(fullName);
    this->ip = ninjam::client::extractUserIP(fullName);
}

void RemoteUser::setLastKeepAliveToNow()
{
    lastKeepAliveReceived = QDateTime::currentMSecsSinceEpoch();
}

// -------------------------------------------------------------

Server::Server() :
    bpm(120),
    bpi(8),
    topic("No topic!"),
    licence("No licence at moment!"),
    maxChannels(2),
    maxUsers(8),
    keepAlivePeriod(30)
{
    connect(&tcpServer, &QTcpServer::newConnection, this, &Server::handleNewConnection);
    connect(&tcpServer, &QTcpServer::acceptError, this, &Server::handleAcceptError);
}

Server::~Server()
{
    shutdown();
}

QHostAddress Server::getBestHostAddress()
{
    return QHostAddress::AnyIPv4;

//    QList<QHostAddress> possibleMatches;
//    auto interfaces = QNetworkInterface::allInterfaces();

//  QHostAddress bestHostAddress = QHostAddress::AnyIPv4;

//  for(int i = 0; i < interfaces.size(); i++)
//  {
//    unsigned int flags = interfaces[i].flags();
//    bool isLoopback = static_cast<bool>(flags & QNetworkInterface::IsLoopBack);
//    bool isP2P = static_cast<bool>(flags & QNetworkInterface::IsPointToPoint);
//    bool isRunning = static_cast<bool>(flags & QNetworkInterface::IsRunning);

//    if ( !isRunning ) // If this interface isn't running, we don't care about it
//        continue;

//    // We only want valid interfaces that aren't loopback/virtual and not point to point
//    if ( !interfaces[i].isValid() || isLoopback || isP2P )
//        continue;

//    auto addresses = interfaces[i].allAddresses();
//    for(int a=0; a < addresses.size(); a++)
//    {
//      if (addresses[a] == QHostAddress::LocalHost)
//          continue; // Ignore local host

//      if (!addresses[a].toIPv4Address())
//          continue; // Ignore non-ipv4 addresses

//      bestHostAddress = addresses[a];
//      if (bestHostAddress.isNull())
//          continue;
//      bool foundMatch = false;
//      for (int j=0; j < possibleMatches.size(); j++) {
//          if ( bestHostAddress == possibleMatches[j] ) {
//              foundMatch = true;
//              break;
//          }
//      }

//      if ( !foundMatch ) {
//          possibleMatches.push_back(bestHostAddress);
//          qDebug() << "possible address: " << interfaces[i].humanReadableName() << "->" << bestHostAddress;
//      }

//    }
//  }

//  return bestHostAddress;

}

void Server::start(quint16 port)
{
    shutdown();

    QHostAddress address = Server::getBestHostAddress();
    bool listening = tcpServer.listen(address, port);
    if (listening)
        qDebug() << "Server listening port:" << tcpServer.serverPort();
    else
        qCritical() << "Error starting server " << address << tcpServer.errorString();

    getBestHostAddress();
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

    remoteUsers.insert(socket, RemoteUser());

    sendAuthChallenge(socket);
}

void Server::sendAuthChallenge(QTcpSocket *device)
{
    QByteArray challenge("abcdabcd");
    quint32 protocolVersion = 0x00020000; // fixed value
    quint32 serverCapabilities = keepAlivePeriod << 8; // keep alive period value is stored in bytes 8-15
    if(!licence.isEmpty())
        serverCapabilities |= 1; // when server has licence the first bit is set.

    auto msg = AuthChallengeMessage(challenge, licence, serverCapabilities, protocolVersion);
    msg.to(device);
}

void Server::processClientAuthUserMessage(QTcpSocket *socket, const MessageHeader &header)
{
    auto msg = ClientAuthUserMessage::unserializeFrom(socket, header.getPayload());

    // ignoring challenge and password for while

    quint8 flag = 1; // authentication suceeded
    QString newUserName(generateUniqueUserName(msg.getUserName())); // updated user name or error message;
    newUserName += "@" + tcpServer.serverAddress().toString();

    remoteUsers[socket].setFullName(newUserName);

    AuthReplyMessage authReply(flag, newUserName, maxChannels);
    authReply.to(socket);

    if (!authReply.userIsAuthenticated()) {
        remoteUsers.remove(socket);
    }
}

QString Server::generateUniqueUserName(const QString &userName) const
{
    static const QString ANON_PREFIX("anonymous:");
    static const QString SPECIAL_SYMBOLS("[^a-zA-Z0-9 _-]"); // allowing lower and upper case letters, numbers, _ and - symbols, blank space, at least 1 character and max 16 characters.
    static const quint8 MAX_NAME_SIZE = 16;

    // replace special symbols
    QString newName(QString(userName).replace(QRegularExpression(SPECIAL_SYMBOLS), QString("_")));

    if (userName.startsWith(ANON_PREFIX)) // remove the anonymous: prefix
        newName = newName.right(newName.size() - ANON_PREFIX.size());

    if (newName.size() > MAX_NAME_SIZE) // truncate to 16 symbols
        newName = newName.left(MAX_NAME_SIZE);

    if (newName.isEmpty())
        newName = "anon";

    // check if is unique name
    for (const auto &user : remoteUsers.values()) {
        if (user.getName() == newName) {
            if (newName.size() < MAX_NAME_SIZE)
                newName = newName + "_";
            else
                newName = newName.left(MAX_NAME_SIZE - 1) + "_";
            break;
        }
    }

    return newName;
}

void Server::processClientSetChannel(QTcpSocket *socket, const ninjam::MessageHeader &header)
{
    auto msg = ClientSetChannel::unserializeFrom(socket, header.getPayload());
    qDebug() << "Client Set Channel received  channels:" << msg.getChannelNames();

    // send server config change
    auto configChange = ConfigChangeNotifyMessage(bpm, bpi);
    configChange.to(socket);

    // broadcast the new user to everybody
    if (remoteUsers.contains(socket))
        broadcastUserChangeNotify(remoteUsers[socket]);

    auto topicMessage = ServerToClientChatMessage::buildTopicMessage(topic);
    topicMessage.to(socket);
}

void Server::broadcastUserChangeNotify(const RemoteUser &remoteUser)
{
    UserInfoChangeNotifyMessage msg;

    for (int c = 0; c < remoteUser.getChannelsCount(); ++c)
        msg.addUserChannel(remoteUser.getFullName(), remoteUser.getChannel(c));

    for (auto socket : remoteUsers.keys())
        msg.to(socket);
}

void Server::processUploadIntervalBegin(QTcpSocket *socket, const MessageHeader &header)
{
    qint64 before = socket->bytesAvailable();
    auto msg = UploadIntervalBegin::from(socket, header.getPayload());
    //qDebug() << "Upload interval begin received from " << remoteUsers[socket].getName();
    Q_ASSERT(before - socket->bytesAvailable() == header.getPayload());
}

void Server::processUploadIntervalWrite(QTcpSocket *socket, const MessageHeader &header)
{
    qint64 before = socket->bytesAvailable();
    auto msg = UploadIntervalWrite::from(socket, header.getPayload());
    //qDebug() << "Upload interval write received " << msg.getEncodedData().size() << " bytes";
    Q_ASSERT(before - socket->bytesAvailable() == header.getPayload());
}

void Server::broadcastPublicChatMessage(const ClientToServerChatMessage &receivedMessage, const QString &userFullName)
{
    Q_ASSERT(receivedMessage.isPublicMessage());

    QString messageText = receivedMessage.getArguments().at(0);
    auto msg = ServerToClientChatMessage::buildPublicMessage(userFullName, messageText);
    for (auto s : remoteUsers.keys()) {
        msg.to(s);
        s->flush();
    }
}

void Server::processChatMessage(QTcpSocket *socket, const ninjam::MessageHeader &header)
{
    if (!remoteUsers.contains(socket))
        return;

    ClientToServerChatMessage receivedMessage = ClientToServerChatMessage::from(socket, header.getPayload());

    if (receivedMessage.isPublicMessage()) {
        QString userFullName = remoteUsers[socket].getFullName();
        broadcastPublicChatMessage(receivedMessage, userFullName);
    }
    else if (receivedMessage.isPrivateMessage()) {
        QString senderFullName = remoteUsers[socket].getFullName();
        QString destinationUserName = receivedMessage.getArguments().at(0);
        if (!destinationUserName.contains("@"))
            destinationUserName += "@" + tcpServer.serverAddress().toString();
        QString text = receivedMessage.getArguments().at(1);
        auto msg = ServerToClientChatMessage::buildPrivateMessage(senderFullName, text);
        for (auto s : remoteUsers.keys()) {
            const RemoteUser &user = remoteUsers[s];
            if (user.getFullName() == destinationUserName) {
                msg.to(s);
                break;
            }
        }
    }
    else if (receivedMessage.isAdminMessage()) {

    }
    else {
        qCritical() << "Error handling chat message!" << receivedMessage.getCommand() << receivedMessage.getArguments();
    }

}

void Server::processKeepAlive(QTcpSocket *socket, const ninjam::MessageHeader &)
{
    if (remoteUsers.contains(socket)) {
        qDebug() << "processing keep alive";
        remoteUsers[socket].setLastKeepAliveToNow();
    }
}

void Server::processReceivedBytes()
{
    auto socket = qobject_cast<QTcpSocket *>(QObject::sender());
    if (!socket) {
        qFatal("Error, socket is NULL!");
        return;
    }

    //check if remote users need keep alive request
    for (auto socket : remoteUsers.keys()) {
        const auto &user = remoteUsers[socket];
        auto now = QDateTime::currentMSecsSinceEpoch();
        auto delta = (now - user.getLastKeepAliveReceived()) / 1000; // in seconds
        if (delta >= keepAlivePeriod) {
            if (delta >= keepAlivePeriod * 3) { // client is not responding
                disconnectClient(socket);
            }
            else {
                //qDebug() << "Requesting keep alive to" << user.getFullName();
                ClientKeepAlive msg;
                msg.serializeTo(socket);
            }
        }
    }

    if (!remoteUsers.contains(socket)) {
        qFatal("NOT CONTAIN SOCKET");
        return;
    }

    RemoteUser &user = remoteUsers[socket];

    while (socket->bytesAvailable() >= 5) { // all messages have minimum of 5 bytes

        MessageHeader header = user.getCurrentHeader();
        if (!header.isValid()) {
            header = MessageHeader::from(socket);
            user.setCurrentHeader(header);
//            qDebug() << "Reading new header to" << user.getName()
//                     << " code:" << QString::number(static_cast<quint8>(header.getMessageType()), 16)
//                     << " payload:" << header.getPayload();
        }
//        else {
//            qDebug() << "Header is valid to " << user.getName() << " continuing...";
//        }

        Q_ASSERT(header.isValid());

        if (socket->bytesAvailable() < header.getPayload()) {
            qDebug() << "not enough bytes " << socket->bytesAvailable() << "/" << header.getPayload()
                     << " bytes available  msg code:"<< QString::number(static_cast<quint8>(header.getMessageType()), 16);
            return;
        }

        switch (header.getMessageType()) {
        case MessageType::ClientAuthUser:
            processClientAuthUserMessage(socket, header);
            break;

        case MessageType::ClientSetChannel:
            processClientSetChannel(socket, header);
            break;

        case MessageType::KeepAlive:
            processKeepAlive(socket, header);
            break;

        case MessageType::UploadIntervalBegin:
            processUploadIntervalBegin(socket, header);
            break;

        case MessageType::UploadIntervalWrite:
            processUploadIntervalWrite(socket, header);
            break;

        case MessageType::ChatMessage:
            processChatMessage(socket, header);
            break;

        default:
            qFatal(QString("not handled message code: %1")
                   .arg(QString::number(static_cast<quint8>(header.getMessageType())), 16).toStdString().c_str());
        }

        //qDebug() << "Header used, reseting to parse next message";
        user.setCurrentHeader(MessageHeader()); // invalidate header to force a new parsing in next loop iteration
    }

    user.setLastKeepAliveToNow();
}

void Server::handleDisconnection()
{
    auto socket = qobject_cast<QTcpSocket *>(QObject::sender());
    if (socket)
        disconnectClient(socket);
}

void Server::disconnectClient(QTcpSocket *socket)
{
    qDebug() << "Trying to disconnect" << socket;
    if (remoteUsers.contains(socket)) {
        qDebug() << "Disconnecting " << remoteUsers[socket].getName();
        remoteUsers.remove(socket);
        socket->deleteLater();
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

        remoteUsers.clear();

        qDebug() << "server shutdown";
    }
}
