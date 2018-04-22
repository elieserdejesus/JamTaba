#include <QTest>
#include "TestChatChordsProgressionParser.h"
#include "TestChordsParser.h"

int main(int argc, char *argv[])
{
    TestChatChordsProgressionParser testChatParser;
    TestChordsParser testChordsParser;

    int result = 0;

    result += QTest::qExec(&testChatParser, argc, argv);
    result += QTest::qExec(&testChordsParser, argc, argv);

    return result > 0 ? -result : 0;
}
