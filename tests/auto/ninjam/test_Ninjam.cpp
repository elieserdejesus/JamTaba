#include <QTest>
#include "TestServerInfo.h"
#include "TestMessagesSerialization.h"
#include "TestServerMessagesHandler.h"

int main(int argc, char *argv[])
{
    TestMessagesSerialization testServerMessages;
    TestServer testServer;
    TestServerMessagesHandler testServerMessagesHandler;

    int testResults = 0;
    testResults |= QTest::qExec(&testServerMessages, argc, argv);
    testResults |= QTest::qExec(&testServer, argc, argv);
    testResults |= QTest::qExec(&testServerMessagesHandler, argc, argv);
    return testResults;
}
