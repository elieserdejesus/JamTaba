#include <QTest>
#include "TestChatChordsProgressionParser.h"
#include "TestChatVotingMessage.h"

int main(int argc, char *argv[])
{
    TestChatChordsProgressionParser testChatParser;
    TestChatVotingMessage testVotingMessage;
    int result = 0;

    result += QTest::qExec(&testVotingMessage);
    result += QTest::qExec(&testChatParser);

    return result > 0 ? -result : 0;
}
