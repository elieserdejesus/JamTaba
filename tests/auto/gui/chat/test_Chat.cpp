#include <QTest>
#include "TestChatChordsProgressionParser.h"
#include "TestChatVotingMessage.h"

int main(int argc, char *argv[])
{
    TestChatChordsProgressionParser testChatParser;
    TestChatVotingMessage testVotingMessage;
    int result = 0;


    result += QTest::qExec(&testChatParser);
    result += QTest::qExec(&testVotingMessage);

    return result > 0 ? -result : 0;
}
