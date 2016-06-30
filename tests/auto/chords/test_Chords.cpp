#include <QTest>
#include "TestChatChordsProgressionParser.h"

int main(int argc, char *argv[])
{
    TestChatChordsProgressionParser testChatParser;
    int result = 0;


    result += QTest::qExec(&testChatParser);

    return result > 0 ? -result : 0;
}
