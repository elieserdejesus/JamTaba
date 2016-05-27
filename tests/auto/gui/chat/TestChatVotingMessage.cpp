#include "TestChatVotingMessage.h"
#include "gui/chat/NinjamVotingMessageParser.h"
#include <QTest>

using namespace Gui::Chat;

void TestChatVotingMessages::parsingValidSystemVotingMessages()
{
    QFETCH(QString, voteType);
    QFETCH(quint32, voteValue);
    QFETCH(quint32, expirationTime);

    QString message = buildSystemVotingMessage(voteType, voteValue, expirationTime);
    SystemVotingMessage votingMessage = parseSystemVotingMessage(message);

    QVERIFY(votingMessage.isValidVotingMessage());

    QCOMPARE(votingMessage.getExpirationTime(), expirationTime);
    QCOMPARE(votingMessage.getVoteValue(), voteValue);

    if (voteType == "BPI")
        QVERIFY(votingMessage.isBpiVotingMessage());
    else
        QVERIFY(votingMessage.isBpmVotingMessage());
}

void TestChatVotingMessages::parsingValidSystemVotingMessages_data()
{
    QTest::addColumn<QString>("voteType");
    QTest::addColumn<quint32>("voteValue");
    QTest::addColumn<quint32>("expirationTime");

    QTest::newRow("32 BPI, expires in 60s") << QString("BPI") << (quint32)32 << (quint32)60;
    QTest::newRow("16 BPI, expires in 5s") << QString("BPI") << (quint32)16 << (quint32)5;
    QTest::newRow("64 BPI, expires in 120s") << QString("BPI") << (quint32)64 << (quint32)120;

    QTest::newRow("80 BPM, expires in 51s") << QString("BPM") << (quint32)80 << (quint32)51;
    QTest::newRow("240 BPM, expires in 11s") << QString("BPM") << (quint32)240 << (quint32)11;
}

void TestChatVotingMessages::parsingInvalidSystemVotingMessages()
{
    QFETCH(QString, voteType);
    QFETCH(quint32, voteValue);
    QFETCH(quint32, expirationTime);

    QString message = buildSystemVotingMessage(voteType, voteValue, expirationTime);
    SystemVotingMessage votingMessage = parseSystemVotingMessage(message);

    QVERIFY(!votingMessage.isValidVotingMessage());
}

void TestChatVotingMessages::parsingInvalidSystemVotingMessages_data()
{
    QTest::addColumn<QString>("voteType");
    QTest::addColumn<quint32>("voteValue");
    QTest::addColumn<quint32>("expirationTime");

    QTest::newRow("Invalid vote type") << QString("BPA") << (quint32)32 << (quint32)60;
    QTest::newRow("Invalid vote value") << QString("BPI") << (quint32)1616 << (quint32)5;
    QTest::newRow("Invalid expiration time") << QString("BPI") << (quint32)16 << (quint32)1234;
}

QString TestChatVotingMessages::buildSystemVotingMessage(const QString &voteType, quint32 voteValue, quint32 expirationTime)
{
    return "[voting system] leading candidate: 1/2 votes for " + QString::number(voteValue) + " " + voteType + " [each vote expires in " + QString::number(expirationTime) + "s]";
}

void TestChatVotingMessages::validSystemVotingMessage()
{
    QFETCH(QString, chatMessage);
    QString userName = ""; //empty user for system voting messages
    QVERIFY(Gui::Chat::isSystemVotingMessage(userName, chatMessage));
}

void TestChatVotingMessages::validSystemVotingMessage_data()
{
    QTest::addColumn<QString>("chatMessage");
    QTest::newRow("12 BPI, expires in 60s") << QString("[voting system] leading candidate: 1/2 votes for 12 BPI [each vote expires in 60s]");

    QTest::newRow("32 BPI, expires in 30s") << QString("[voting system] leading candidate: 1/2 votes for 32 BPI [each vote expires in 30s]");

    QTest::newRow("3 BPI, expires in 5s")   << QString("[voting system] leading candidate: 1/2 votes for 3 BPI [each vote expires in 5s]");

    QTest::newRow("240 BPM, expires in 5s") << QString("[voting system] leading candidate: 1/2 votes for 240 BPM [each vote expires in 5s]");

    QTest::newRow("240 BPM, expires in 120s") << QString("[voting system] leading candidate: 1/2 votes for 240 BPM [each vote expires in 120s]");
}

void TestChatVotingMessages::invalidSystemVotingMessage()
{
    QFETCH(QString, chatMessage);
    QFETCH(QString, userName);
    QVERIFY(!Gui::Chat::isSystemVotingMessage(userName, chatMessage));
}

void TestChatVotingMessages::invalidSystemVotingMessage_data()
{
    QTest::addColumn<QString>("userName");
    QTest::addColumn<QString>("chatMessage");

    QTest::newRow("Invalid user name, valid message")
            << QString("Any username")
            << QString("[voting system] leading candidate: 1/2 votes for 12 BPI [each vote expires in 60s]");

    QTest::newRow("Valid user name, invalid BPI")
            << QString("") //empty user name is valid for system voting messages
            << QString("[voting system] leading candidate: 1/2 votes for 1234 BPI [each vote expires in 60s]");

    QTest::newRow("Valid user name, valid vote value, invalid (empty) expiration time")
            << QString("")
            << QString("[voting system] leading candidate: 1/2 votes for 32 BPI [each vote expires in s]");

    QTest::newRow("Valid user name, valid vote value, invalid (big) expiration time")
            << QString("")
            << QString("[voting system] leading candidate: 1/2 votes for 32 BPI [each vote expires in 1234s]");

}

