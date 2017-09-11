#include <QObject>

#include <QtTest>
#include "TestSamplesBuffer.h"
#include "TestLooper.h"

int main(int argc, char *argv[])
{
    TestSamplesBuffer testSamplesBuffer;
    TestLooper testLooper;

    int result = QTest::qExec(&testSamplesBuffer, argc, argv);

    result |= QTest::qExec(&testLooper, argc, argv);

    return result;
}
