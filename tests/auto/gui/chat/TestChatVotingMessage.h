#ifndef TEST_CHAT_VOTING_MESSAGE_H
#define TEST_CHAT_VOTING_MESSAGE_H

#include <QObject>

class TestChatVotingMessage : public QObject
{
    Q_OBJECT

private slots:
    void isVotingMessage_data();
    void isVotingMessage();

    void parseVotingMessage();
    void parseVotingMessage_data();
};

#endif // TESTCHATCHORDSPROGRESSIONPARSER_H
