#ifndef TEST_CHAT_VOTING_MESSAGE_H
#define TEST_CHAT_VOTING_MESSAGE_H

#include <QObject>

class TestChatVotingMessages : public QObject
{
    Q_OBJECT

private slots:

    //local user voting message
    void validLocalUserVotingMessage_data();
    void validLocalUserVotingMessage();
    void invalidLocalUserVotingMessage_data();
    void invalidLocalUserVotingMessage();

    // System voting message
    void validSystemVotingMessage_data();
    void validSystemVotingMessage();

    void invalidSystemVotingMessage_data();
    void invalidSystemVotingMessage();

    void parsingValidSystemVotingMessages();
    void parsingValidSystemVotingMessages_data();

    void parsingInvalidSystemVotingMessages();
    void parsingInvalidSystemVotingMessages_data();

private:
    static QString buildSystemVotingMessage(const QString &voteType, quint32 voteValue, quint32 expirationTime);
    static QString buildLocalUserVotingMessage(const QString &voteType, quint32 voteValue);
};

#endif // TESTCHATCHORDSPROGRESSIONPARSER_H
