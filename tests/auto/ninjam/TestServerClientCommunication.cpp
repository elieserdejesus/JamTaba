#include "TestServerClientCommunication.h"
#include <QTest>
#include <QDebug>
#include <QCoreApplication>
#include <QTimer>

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

void TestServerClientCommunication::clientServerConnectionAndDisconnection()
{
    int argc = 0;
    char **argv;

    QCoreApplication app(argc, argv);

    const quint16 serverPort = 2049;
    Server server;
    server.start(serverPort);

    Service client;

    connect(&client, &Service::connectedInServer, [&](const ServerInfo &info){
        qDebug() << "Connected in server" << info.getUniqueName();
        client.disconnectFromServer(true);
        app.quit();
    });

    QString userName("test");
    QStringList channels;
    channels << "firstChannel";
    channels << "secondChannel";
    client.startServerConnection("localhost", serverPort, userName, channels);

    app.exec();
}

