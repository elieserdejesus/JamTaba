#include "MainChat.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSslError>

using login::MainChat;

const QString MainChat::MAIN_CHAT_URL = "ws://jamtaba-chat-backend.herokuapp.com";
//const QString MainChat::MAIN_CHAT_URL = "ws://localhost:3000";

const quint8 MainChat::SERVER_PING_PERIOD = 45; // in seconds - Hekoru timeout is 55 seconds

MainChat::MainChat(QObject *parent) :
    QObject(parent),
    userName("guest"),
    pingTimer(new QTimer(this))
{

    connect(&webSocket, &QWebSocket::connected, [=](){

        pingTimer->start();

        emit connected();
    });

    connect(&webSocket, &QWebSocket::disconnected, this, [=](){

        auto sender = qobject_cast<QWebSocket *>(QObject::sender());
        if (!sender)
            return;

        if (sender->error() != QAbstractSocket::UnknownSocketError)
            qCritical() << sender->errorString();

        emit disconnected();
    });

    connect(&webSocket, &QWebSocket::textMessageReceived, this, &MainChat::textMessageReceived);

    connect(&webSocket, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error), [=]() {
        auto sender = qobject_cast<QWebSocket *>(QObject::sender());
        if (sender)
            emit error(sender->errorString());
    });

    pingTimer->setInterval(SERVER_PING_PERIOD * 1000);

    connect(pingTimer, &QTimer::timeout, [=](){
        webSocket.ping(); // ping in the server to avoid time out and close connection
    });

    connect(&webSocket, &QWebSocket::bytesWritten, [=](){
        pingTimer->start(); // restart the ping timer when some message is sended
    });
}

void MainChat::sendPublicMessage(const QString &content)
{
    if (!userName.isEmpty()) {
        QJsonObject message;
        message["command"] = "MSG";
        message["userName"] = userName;
        message["content"] = content;

        webSocket.sendTextMessage(QJsonDocument(message).toJson());
    }
    else {
        emit error(tr("You can't send messages using an empty user name!"));
    }
}

void MainChat::sendServerInvite(const QString &destinationUserFullName, const QString &serverIP, quint16 serverPort)
{
    QJsonObject message;
    message["command"] = "SERVER_INVITE";
    message["destinationUser"] = destinationUserFullName;
    message["senderUser"] = userName;
    message["serverIP"] = serverIP;
    message["serverPort"] = serverPort;

    webSocket.sendTextMessage(QJsonDocument(message).toJson());
}

void MainChat::setUserName(const QString &userName)
{
    if (this->userName != userName) {
        this->userName = userName;

        QJsonObject message;
        message["command"] = "USERNAME";
        message["userName"] = userName;

        webSocket.sendTextMessage(QJsonDocument(message).toJson());
    }
}

void MainChat::textMessageReceived(const QString &message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (doc.isNull()) {
        qCritical() << "Invalid json message " << doc.toJson();
        return;
    }

    parseReceivedData(doc.object());
}

void MainChat::parseReceivedMessage(const QJsonObject &data)
{
    if (!data.contains("userName")) {
        qCritical() << "Message not contain 'userName' \"" << data << "\"";
        return;
    }
    if (!data.contains("content")) {
        qCritical() << "Message not contain 'content' \"" << data << "\"";
        return;
    }

    QString userName = data["userName"].toString();
    QString content = data["content"].toString();

    emit messageReceived(userName, content);
}

void MainChat::parseNewUsersList(const QJsonObject &data)
{
    if (!data.contains("users")) {
        qCritical() << " message not contains 'users' array";
        return;
    }

    if (!data["users"].isArray()) {
        qCritical() << "'users' is not an array";
        return;
    }

    QStringList usersNames;
    QJsonArray array = data["users"].toArray();
    for (int i = 0; i < array.count(); ++i) {
        usersNames.append(array[i].toString());
    }

    emit usersListChanged(usersNames);
}

void MainChat::parseServerInvite(const QJsonObject &data)
{
    QStringList expectedFields = QStringList() << "senderUser" << "serverIP" << "serverPort";
    for (auto field : expectedFields) {
        if (!data.contains(field)) {
            qCritical() << QString("data not contain '%1' field").arg(field);
            return;
        }
    }

    QString senderFullName = data["senderUser"].toString();
    QString serverIP = data["serverIP"].toString();
    quint16 serverPort = data["serverPort"].toInt();

    emit serverInviteReceived(senderFullName, serverIP, serverPort);
}

void MainChat::parseReceivedData(const QJsonObject &data)
{
    if (!data.contains("command")) {
        qCritical() << "The message not contains 'command' \"" << data << "\"";
        return;
    }

    QString command = data["command"].toString();
    if (command == "MSG") { // new text message received
        parseReceivedMessage(data);
    }
    else if (command == "USERS") { // new users list received
        parseNewUsersList(data);
    }
    else if (command == "SERVER_INVITE") {
        parseServerInvite(data);
    }
    else {
        qCritical() << "Command not handled yet: " << command;
    }
}

void MainChat::connectWithServer(const QString &serverUrl)
{
    if (webSocket.isValid())
        webSocket.close();

    webSocket.open(QUrl(serverUrl));
}
