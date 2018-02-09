#include "TestServerClientCommunication.h"
#include <QTest>
#include <QDebug>
#include <QCoreApplication>
#include <QTimer>
#include <QRegularExpression>

#include "ninjam/Ninjam.h"
#include "ninjam/client/User.h"
#include "ninjam/client/UserChannel.h"
#include "ninjam/client/ServerInfo.h"

#include "ninjam/client/ClientMessages.h"
#include "ninjam/client/ServerMessages.h"
#include "ninjam/client/Service.h"

#include "ninjam/server/Server.h"

using namespace ninjam;
using namespace ninjam::client;
using namespace ninjam::server;

void TestServerClientCommunication::privateChatMessage()
{
    int argc = 0;
    char **argv = nullptr;

    QCoreApplication app(argc, argv);

    const quint16 serverPort = 2049;
    Server server;
    server.start(serverPort);

    QString chatMessage("testing private chat message");

    QString receiverName("receiver");
    QString senderName("sender");

    Service client;
    Service client2;

    quint8 connections = 0;

    connect(&client, &Service::disconnectedFromServer, &app, &QCoreApplication::quit);

    connect(&client, &Service::privateChatMessageReceived, [&](const User &sender, const QString &msg){
        qDebug() << "Private chat message received:" << msg << " from" << sender.getFullName();
        QCOMPARE(chatMessage, msg);
        QCOMPARE(sender.getName(), senderName);

        client2.disconnectFromServer(true);
        client.disconnectFromServer(true);
    });

    connect(&client, &Service::connectedInServer, [&](const ServerInfo &serverInfo){
        qDebug() << "Connected in server" << serverInfo.getUniqueName();

        connections++;

        if (connections >= 2)
            client2.sendPrivateChatMessage(chatMessage, receiverName);
    });

    connect(&client2, &Service::connectedInServer, [&](const ServerInfo &serverInfo){
        qDebug() << "Connected in server" << serverInfo.getUniqueName();

        connections++;

        if (connections >= 2)
            client2.sendPrivateChatMessage(chatMessage, receiverName);
    });

    client.startServerConnection("localhost", serverPort, receiverName, QStringList());
    client2.startServerConnection("localhost", serverPort, senderName, QStringList());

    app.exec();
}

void TestServerClientCommunication::publicChatMessage()
{
    int argc = 0;
    char **argv = nullptr;

    QCoreApplication app(argc, argv);

    const quint16 serverPort = 2049;
    Server server;
    server.start(serverPort);

    QString chatMessage("testing chat message");

    Service client;

    connect(&client, &Service::disconnectedFromServer, &app, &QCoreApplication::quit);

    connect(&client, &Service::publicChatMessageReceived, [&](const User &sender, const QString &msg){
        qDebug() << "Chat message received:" << msg << sender.getName();
        QCOMPARE(chatMessage, msg);
        client.disconnectFromServer(true);
    });

    connect(&client, &Service::connectedInServer, [&](const ServerInfo &serverInfo){
        qDebug() << "Connected in server" << serverInfo.getUniqueName();

        client.sendPublicChatMessage(chatMessage);
    });

    client.startServerConnection("localhost", serverPort, "userName", QStringList());

    app.exec();
}

void TestServerClientCommunication::clientServerConnectionAndDisconnection_data()
{
    QTest::addColumn<QString>("userName");
    QTest::addColumn<QString>("newUserName"); // new name after connected
    QTest::addColumn<QStringList>("channels");

    QTest::newRow("Connecting using valid name, 2 channels")
            << QString("tester") << QString("tester") << (QStringList() << "first" << "second");

    QTest::newRow("Connecting using INvalid name, 2 channels")
            << QString("tester!@.") << QString("tester___") << (QStringList() << "first" << "second");

    QTest::newRow("Connecting using big name, no channels")
            << QString("01234567890123456789") << QString("0123456789012345") << (QStringList());

    QTest::newRow("Connecting as anon, no channels")
            << QString("") << QString("anon") << (QStringList());
}

void TestServerClientCommunication::clientServerConnectionAndDisconnection()
{
    int argc = 0;
    char **argv = nullptr;

    QCoreApplication app(argc, argv);

    const quint16 serverPort = 2049;
    Server server;
    server.start(serverPort);

    QFETCH(QString, userName);
    QFETCH(QString, newUserName);
    QFETCH(QStringList, channels);

    Service client;

    connect(&client, &Service::disconnectedFromServer, &app, &QCoreApplication::quit);

    connect(&client, &Service::connectedInServer, [&](const ServerInfo &serverInfo){
        qDebug() << "Connected in server" << serverInfo.getUniqueName();

        QCOMPARE(serverInfo.getTopic(), server.getTopic());
        QCOMPARE(serverInfo.getBpi(), server.getBpi());
        QCOMPARE(serverInfo.getBpm(), server.getBpm());
        QCOMPARE(serverInfo.getHostName(), QString("localhost"));
        QCOMPARE(serverInfo.getLicence(), server.getLicence());
        QCOMPARE(serverInfo.getMaxChannels(), server.getMaxChannels());
        QString name = ninjam::client::extractUserName(client.getConnectedUserName()); // remove IP from name
        QCOMPARE(name, newUserName);

        client.disconnectFromServer(true);

    });

    client.startServerConnection("localhost", serverPort, userName, channels);

    app.exec();
}

//void TestServerClientCommunication::connectInNonEmptyServer()
//{
//    int argc = 0;
//    char **argv = nullptr;

//    QCoreApplication app(argc, argv);

//    const quint16 serverPort = 2049;
//    Server server;
//    server.start(serverPort);

//    Service client1;
//    Service client2;
//    QString user1 = "user1";
//    QString user2 = "user2";

//    quint8 connections = 0;

//    connect(&client1, &Service::disconnectedFromServer, &app, &QCoreApplication::quit);

//    connect(&client1, &Service::connectedInServer, [&](const ServerInfo &serverInfo){
//        qDebug() << "Connected in server" << serverInfo.getUniqueName() << serverInfo.getUsers();
//        connections++;
//        if (connections >= 2) {
//            QCOMPARE(serverInfo.getUsers().size(), 1);
//            client2.disconnectFromServer(true);
//            client1.disconnectFromServer(true);
//        }
//    });

//    connect(&client2, &Service::connectedInServer, [&](const ServerInfo &serverInfo){
//        qDebug() << "Connected in server" << serverInfo.getUniqueName() << serverInfo.getUsers();
//        connections++;
//        if (connections >= 2) {
//            QCOMPARE(serverInfo.getUsers().size(), 1);
//            client2.disconnectFromServer(true);
//            client1.disconnectFromServer(true);
//        }
//    });

//    client1.startServerConnection("localhost", serverPort, user1, QStringList("channel1"));
//    client2.startServerConnection("localhost", serverPort, user2, QStringList("channel2"));

//    app.exec();
//}

