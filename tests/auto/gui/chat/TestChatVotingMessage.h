#ifndef TEST_CHAT_VOTING_MESSAGE_H
#define TEST_CHAT_VOTING_MESSAGE_H

#include <QObject>

class TestChatVotingMessage : public QObject
{
    Q_OBJECT

private slots:
    void validVotingMessage_data();
    void validVotingMessage();

    void invalidVotingMessage_data();
    void invalidVotingMessage();

    void parsingValidVotingMessages();
    void parsingValidVotingMessages_data();

    void parsingInvalidVotingMessages();
    void parsingInvalidVotingMessages_data();

private:
    static QString buildVotingSystemMessage(const QString &voteType, quint32 voteValue, quint32 expirationTime);

};

#endif // TESTCHATCHORDSPROGRESSIONPARSER_H
