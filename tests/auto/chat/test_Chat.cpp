#include <QTest>
#include "TestChatVotingMessages.h"
#include "TestAdminCommands.h"

int main(int argc, char *argv[])
{
    TestChatVotingMessages testVotingMessage;
    TestAdminCommands testAdminCommands;

    int result = 0;

    result += QTest::qExec(&testVotingMessage);
    result += QTest::qExec(&testAdminCommands);

    return result > 0 ? -result : 0;
}
