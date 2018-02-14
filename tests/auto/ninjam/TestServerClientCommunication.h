#ifndef TESTE_SERVER_CLIENT_COMMUNICATION_H
#define TESTE_SERVER_CLIENT_COMMUNICATION_H

#include <QObject>
#include "ninjam/client/ServerMessages.h"
#include "ninjam/client/UserChannel.h"
#include "ninjam/client/ClientMessages.h"
#include "ninjam/server/Server.h"
#include "ninjam/client/ServerInfo.h"

using namespace ninjam::client;
using namespace ninjam::server;
using namespace ninjam;

// these tests are communication server and client to check messages order and integrity

class TestServerClientCommunication : public QObject
{
    Q_OBJECT

private slots:
    void clientServerConnectionAndDisconnection();
    void clientServerConnectionAndDisconnection_data();

    void publicChatMessage();

    void privateChatMessage();

    void connectInNonEmptyServer();

};

#endif
