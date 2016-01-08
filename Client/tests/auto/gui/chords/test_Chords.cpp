#include <QTest>
#include "TestChatChordsProgressionParser.h"

int main(int argc, char *argv[])
{
    TestChatChordsProgressionParser testChatParser;
    return QTest::qExec(&testChatParser);
}
