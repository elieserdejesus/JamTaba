#include <QTest>
#include <QApplication>
#include "TestEmojiParser.h"

int main(int argc, char *argv[])
{
    TestEmojiParser test;

    int result = 0;

    result += QTest::qExec(&test, argc, argv);

    return result > 0 ? -result : 0;
}
