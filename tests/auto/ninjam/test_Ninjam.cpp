#include <QTest>
#include "TestServerInfo.h"
#include "TestMessagesSerialization.h"
#include "TestServerMessagesHandler.h"
#include "TestServerClientCommunication.h"

int main(int argc, char *argv[])
{
    TestMessagesSerialization testServerMessages;
    TestServerInfo testServer;
    TestServerMessagesHandler testServerMessagesHandler;
    //TestServerClientCommunication testServerClientCommunication;

    int testResults = 0;
    testResults |= QTest::qExec(&testServerMessages, argc, argv);
    testResults |= QTest::qExec(&testServer, argc, argv);
    testResults |= QTest::qExec(&testServerMessagesHandler, argc, argv);
    //testResults |= QTest::qExec(&testServerClientCommunication, argc, argv);
    return testResults;
}
