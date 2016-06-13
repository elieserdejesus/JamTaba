#include "TestChatVotingMessages.h"
#include "gui/chat/NinjamVotingMessageParser.h"
#include <QTest>

using namespace Gui::Chat;

void TestChatVotingMessages::invalidLocalUserVotingMessage_data()
{
    QTest::addColumn<QString>("message");

    QTest::newRow("Any text")           << QString("testing invalid message");
    QTest::newRow("Missing vote value") << QString("!vote bpi");
    QTest::newRow("Missing vote type")  << QString("!vote");
    QTest::newRow("Lower case")         << QString("!VOTE BPM 120");
    QTest::newRow("Missing exclamation")<< QString("vote bpi 32");
    QTest::newRow("BPM in lower case")  << QString("!vote BPM 140");
}

void TestChatVotingMessages::invalidLocalUserVotingMessage()
{
    QFETCH(QString, message);
    QVERIFY(!isLocalUserVotingMessage(message));
}

void TestChatVotingMessages::validLocalUserVotingMessage_data()
{
    QTest::addColumn<QString>("voteType");
    QTest::addColumn<quint32>("voteValue");

    QTest::newRow("3 BPI") << QString("BPI") << (quint32)3;
    QTest::newRow("32 BPI") << QString("BPI") << (quint32)32;
    QTest::newRow("16 BPI") << QString("BPI") << (quint32)16;
    QTest::newRow("90 BPM") << QString("BPM") << (quint32)90;
    QTest::newRow("240 BPM") << QString("BPM") << (quint32)240;
}

void TestChatVotingMessages::validLocalUserVotingMessage()
{
    QFETCH(QString, voteType);
    QFETCH(quint32, voteValue);

    QString message = buildLocalUserVotingMessage(voteType, voteValue);
    QVERIFY(isLocalUserVotingMessage(message));
}

void TestChatVotingMessages::parsingValidSystemVotingMessages()
{
    QFETCH(QString, voteType);
    QFETCH(quint32, voteValue);
    QFETCH(quint32, expirationTime);
    QFETCH(quint8, currentVote);
    QFETCH(quint8, maxVotes);

    QString message = buildSystemVotingMessage(voteType, voteValue, expirationTime, currentVote, maxVotes);
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
    QTest::addColumn<quint8>("currentVote");
    QTest::addColumn<quint8>("maxVotes");

    QTest::newRow("32 BPI, expires in 60s, 1/2") << QString("BPI") << (quint32)32 << (quint32)60 << (quint8)1 << (quint8)2;
    QTest::newRow("16 BPI, expires in 5s, 2/2") << QString("BPI") << (quint32)16 << (quint32)5 << (quint8)2 << (quint8)2;
    QTest::newRow("64 BPI, expires in 120s, 2/3") << QString("BPI") << (quint32)64 << (quint32)120 << (quint8)2 << (quint8)3;

    QTest::newRow("80 BPM, expires in 51s, 3/3") << QString("BPM") << (quint32)80 << (quint32)51 << (quint8)3 << (quint8)3;
    QTest::newRow("240 BPM, expires in 11s, 1/3") << QString("BPM") << (quint32)240 << (quint32)11 << (quint8)1 << (quint8)3;
}

void TestChatVotingMessages::parsingInvalidSystemVotingMessages()
{
    QFETCH(QString, voteType);
    QFETCH(quint32, voteValue);
    QFETCH(quint32, expirationTime);
    QFETCH(quint8, currentVote);
    QFETCH(quint8, maxVotes);

    QString message = buildSystemVotingMessage(voteType, voteValue, expirationTime, currentVote, maxVotes);
    SystemVotingMessage votingMessage = parseSystemVotingMessage(message);

    QVERIFY(!votingMessage.isValidVotingMessage());
}

void TestChatVotingMessages::parsingInvalidSystemVotingMessages_data()
{
    QTest::addColumn<QString>("voteType");
    QTest::addColumn<quint32>("voteValue");
    QTest::addColumn<quint32>("expirationTime");
    QTest::addColumn<quint8>("currentVote");
    QTest::addColumn<quint8>("maxVotes");

    QTest::newRow("Invalid vote type, 1/2") << QString("BPA") << (quint32)32 << (quint32)60 << (quint8)1 << (quint8)2;
    QTest::newRow("Invalid vote value, 1/2") << QString("BPI") << (quint32)1616 << (quint32)5 << (quint8)1 << (quint8)2;
    QTest::newRow("Invalid expiration time, 1/2") << QString("BPI") << (quint32)16 << (quint32)1234 << (quint8)1 << (quint8)2;
}

QString TestChatVotingMessages::buildSystemVotingMessage(const QString &voteType, quint32 voteValue, quint32 expirationTime, quint8 currentVote, quint8 maxVotes)
{
    return "[voting system] leading candidate: " + QString::number(currentVote) + "/" + QString::number(maxVotes) + " votes for " + QString::number(voteValue) + " " + voteType + " [each vote expires in " + QString::number(expirationTime) + "s]";
}

QString TestChatVotingMessages::buildLocalUserVotingMessage(const QString &voteType, quint32 voteValue)
{
    return "!vote " + voteType.toLower() + " " + QString::number(voteValue);
}

void TestChatVotingMessages::validFirstSystemVotingMessage()
{
    QFETCH(QString, chatMessage);
    SystemVotingMessage msg = Gui::Chat::parseSystemVotingMessage(chatMessage);
    QVERIFY(msg.isFirstVotingMessage());
}

void TestChatVotingMessages::validFirstSystemVotingMessage_data()
{
    QTest::addColumn<QString>("chatMessage");

    QTest::newRow("12 BPI, expires in 60s, 1/2") << QString("[voting system] leading candidate: 1/2 votes for 12 BPI [each vote expires in 60s]");

    QTest::newRow("32 BPI, expires in 30s, 1/3") << QString("[voting system] leading candidate: 1/3 votes for 32 BPI [each vote expires in 30s]");

    QTest::newRow("3 BPI, expires in 5s, 1/4")   << QString("[voting system] leading candidate: 1/4 votes for 3 BPI [each vote expires in 5s]");
}

void TestChatVotingMessages::invalidFirstSystemVotingMessage()
{
    QFETCH(QString, chatMessage);
    SystemVotingMessage msg = Gui::Chat::parseSystemVotingMessage(chatMessage);
    QVERIFY(!msg.isFirstVotingMessage());
}

void TestChatVotingMessages::invalidFirstSystemVotingMessage_data()
{
    QTest::addColumn<QString>("chatMessage");

    QTest::newRow("12 BPI, expires in 60s, 2/2") << QString("[voting system] leading candidate: 2/2 votes for 12 BPI [each vote expires in 60s]");

    QTest::newRow("32 BPI, expires in 30s, 2/3") << QString("[voting system] leading candidate: 2/3 votes for 32 BPI [each vote expires in 30s]");

    QTest::newRow("3 BPI, expires in 5s, 3/4")   << QString("[voting system] leading candidate: 3/4 votes for 3 BPI [each vote expires in 5s]");
}

void TestChatVotingMessages::validSystemVotingMessage()
{
    QFETCH(QString, chatMessage);
    QVERIFY(Gui::Chat::parseSystemVotingMessage(chatMessage).isValidVotingMessage());
}

void TestChatVotingMessages::validSystemVotingMessage_data()
{
    QTest::addColumn<QString>("chatMessage");
    QTest::newRow("12 BPI, expires in 60s") << QString("[voting system] leading candidate: 1/2 votes for 12 BPI [each vote expires in 60s]");

    QTest::newRow("32 BPI, expires in 30s") << QString("[voting system] leading candidate: 2/2 votes for 32 BPI [each vote expires in 30s]");

    QTest::newRow("3 BPI, expires in 5s")   << QString("[voting system] leading candidate: 1/2 votes for 3 BPI [each vote expires in 5s]");

    QTest::newRow("240 BPM, expires in 5s") << QString("[voting system] leading candidate: 1/2 votes for 240 BPM [each vote expires in 5s]");

    QTest::newRow("240 BPM, expires in 120s") << QString("[voting system] leading candidate: 1/2 votes for 240 BPM [each vote expires in 120s]");
}

void TestChatVotingMessages::invalidSystemVotingMessage()
{
    QFETCH(QString, chatMessage);
    QFETCH(QString, userName);
    QVERIFY(!userName.isEmpty() || !Gui::Chat::parseSystemVotingMessage(chatMessage).isValidVotingMessage());
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

