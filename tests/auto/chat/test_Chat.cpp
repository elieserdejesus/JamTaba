#include <QTest>
#include "TestChatVotingMessages.h"
#include "TestChatMessages.h"

int main(int argc, char *argv[])
{
    TestChatVotingMessages testVotingMessage;
    TestAdminCommands testAdminCommands;

    int result = 0;

    result += QTest::qExec(&testVotingMessage, argc, argv);
    result += QTest::qExec(&testAdminCommands, argc, argv);

    return result > 0 ? -result : 0;
}
