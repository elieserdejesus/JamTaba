#ifndef TESTE_SERVER_CLIENT_COMMUNICATION_H
#define TESTE_SERVER_CLIENT_COMMUNICATION_H

#include <QObject>
#include "ninjam/client/ServerMessages.h"
#include "ninjam/client/UserChannel.h"

using namespace ninjam::client;

// these tests are communication server and client to check messages order and integrity

class TestServerClientCommunication : public QObject
{
    Q_OBJECT

private slots:
    void clientServerConnectionAndDisconnection();
    void clientServerConnectionAndDisconnection_data();

    void publicChatMessage();

    //void privateChatMessage();
};

#endif
