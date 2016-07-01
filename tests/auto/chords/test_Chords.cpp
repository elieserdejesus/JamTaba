#include <QTest>
#include "TestChatChordsProgressionParser.h"
#include "TestChordsParser.h"

int main(int argc, char *argv[])
{
    TestChatChordsProgressionParser testChatParser;
    TestChordsParser testChordsParser;

    int result = 0;

    result += QTest::qExec(&testChatParser);
    result += QTest::qExec(&testChordsParser);

    return result > 0 ? -result : 0;
}
