#include "TestChatMessages.h"
#include "gui/chat/NinjamChatMessageParser.h"
#include <QTest>

using namespace gui::chat;

void TestNinbotCommands::ninbotLevelMessages_data()
{
    QTest::addColumn<QString>("userName");
    QTest::addColumn<float>("db");
    QTest::addColumn<QString>("message");

    QList<QString> names{ "tester", "ninjamer", "another_tester", "testing"};
    QList<float> dbValues{ 122.49f, 88.5f, 92.01f, 499.572f};

    for (int i = 0; i < names.size(); ++i) {
        QString name = names.at(i);
        float db = dbValues.at(i);

        QTest::newRow(QString("%1 [%2]").arg(name).arg(db).toStdString().c_str()) <<
                                   name <<
                                   db   <<
                                   QString("%1, you are clipping  %2 db").arg(name).arg(db);
    }

}

void TestNinbotCommands::ninbotLevelMessages()
{
    QFETCH(QString, userName);
    QFETCH(float, db);
    QFETCH(QString, message);

    QVERIFY(isNinbotLevelMessage(message));
    QCOMPARE(extractUserNameFromNinbotLevelMessage(message), userName);
    QCOMPARE(extractDBValueFromNinbotLevelMessage(message), db);
}

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

