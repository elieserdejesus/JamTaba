#include "TestChatMessages.h"
#include "gui/chat/NinjamVotingMessageParser.h"
#include <QTest>

using namespace gui::chat;

void TestAdminCommands::invalidPrivateMessage_data()
{
    QTest::addColumn<QString>("message");

    QTest::newRow("Public message")   << QString("testing a simple message");
    QTest::newRow("Admin command")   << QString("/bpi 32");
}

void TestAdminCommands::invalidPrivateMessage()
{
    QFETCH(QString, message);
    QVERIFY(!isPrivateMessage(message));
}


void TestAdminCommands::validPrivateMessage_data()
{
    QTest::addColumn<QString>("message");

    QTest::newRow("test 1")   << QString("/msg user@200.123.123.x testing");
    QTest::newRow("test 2")   << QString("/msg user@127.0.0.1 testing a local host message");
}

void TestAdminCommands::validPrivateMessage()
{
    QFETCH(QString, message);
    QVERIFY(isPrivateMessage(message));
}

void TestAdminCommands::validAdminCommands_data()
{
    QTest::addColumn<QString>("message");

    QTest::newRow("/bpi")   << QString("/bpi 32");
    QTest::newRow("/bpm")   << QString("/bpm 120");
    QTest::newRow("/topic") << QString("/topic testing");
    QTest::newRow("/kick") << QString("/kick username");
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
    QTest::newRow("/msg")   << QString("/msg @ip hello");
}

void TestAdminCommands::invalidAdminCommands()
{
    QFETCH(QString, message);
    QVERIFY(!isAdminCommand(message));
}

