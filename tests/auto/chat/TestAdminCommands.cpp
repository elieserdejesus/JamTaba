#include "TestAdminCommands.h"
#include "gui/chat/NinjamVotingMessageParser.h"
#include <QTest>

using namespace Gui::Chat;

void TestAdminCommands::validAdminCommands_data()
{
    QTest::addColumn<QString>("message");

    QTest::newRow("/bpi")   << QString("/bpi 32");
    QTest::newRow("/bpm")   << QString("/bpm 120");
    QTest::newRow("/topic") << QString("/topic testing");
    QTest::newRow("/msg")   << QString("/msg @ip hello");
}

void TestAdminCommands::validAdminCommands()
{
    QFETCH(QString, message);
    QVERIFY(isAdminCommand(message));
}

void TestAdminCommands::invalidAdminCommands_data()
{
    QTest::addColumn<QString>("message");

    QTest::newRow("test 1")   << QString("testing /bpi 32");
    QTest::newRow("test 2")   << QString("changing /bpm to 120");
    QTest::newRow("test 3") << QString(" /topic testing");
    QTest::newRow("test 4")   << QString(" /msg /msg @ip hello");
}

void TestAdminCommands::invalidAdminCommands()
{
    QFETCH(QString, message);
    QVERIFY(!isAdminCommand(message));
}

