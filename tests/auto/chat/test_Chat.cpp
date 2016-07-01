#include <QTest>
#include "TestChatVotingMessages.h"

int main(int argc, char *argv[])
{
    TestChatVotingMessages testVotingMessage;
    int result = 0;


    result += QTest::qExec(&testVotingMessage);

    return result > 0 ? -result : 0;
}
