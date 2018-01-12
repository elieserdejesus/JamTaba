#include <QTest>
#include "TestServer.h"
#include "TestServerMessages.h"
//#include "TestServerMessagesHandler.h"

int main(int argc, char *argv[])
{
    TestServerMessages testServerMessages;
    TestServer testServer;
    //TestServerMessagesHandler testServerMessagesHandler;
    int testResults = 0;
    testResults |= QTest::qExec(&testServerMessages, argc, argv);
    testResults |= QTest::qExec(&testServer, argc, argv);
    //testResults |= QTest::qExec(&testServerMessagesHandler);
    return testResults;
}
