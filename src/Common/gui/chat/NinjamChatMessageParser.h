#ifndef NINJAM_CHAT_MESSAGE_PARSER_H
#define NINJAM_CHAT_MESSAGE_PARSER_H

#include <QString>

namespace gui
{

    namespace chat
    {

        extern const QRegularExpression SYSTEM_VOTING_REGEX;
        extern const QRegularExpression LOCAL_USER_VOTING_REGEX;
        extern const QRegularExpression ADMIN_COMMAND_REGEX;
        extern const QRegularExpression PRIVATE_MESSAGE_REGEX;
        extern const QRegularExpression PUBLIC_SERVER_INVITE_REGEX;
        extern const QRegularExpression PRIVATE_SERVER_INVITE_REGEX;

        extern const QRegularExpression NINBOT_LEVEL_MESSAGE_REGEX;

        class SystemVotingMessage
        {

        public:
            static SystemVotingMessage newBpiVotingMessage(quint32 bpi, quint32 expirationTime, quint8 currentVote, quint8 maxVotes);
            static SystemVotingMessage newBpmVotingMessage(quint32 bpm, quint32 expirationTime, quint8 currentVote, quint8 maxVotes);
            static SystemVotingMessage newEmptyVotingMessage();
            inline quint32 getExpirationTime() const { return expirationTime; }
            inline quint32 getVoteValue() const { return voteValue; }
            inline bool isBpiVotingMessage() const { return voteType == "BPI"; }
            inline bool isBpmVotingMessage() const { return voteType == "BPM"; }
            bool isValidVotingMessage() const;
            inline bool isFirstVotingMessage() const { return currentVote == 1; }

        private:
            SystemVotingMessage(const QString &voteType, quint32 voteValue, quint32 expirationTime, quint8 currentVote, quint8 maxVotes);
            QString voteType;  // BPI or BPM
            quint32 voteValue; // the BPI or BPM value (90, 120, 16, 32, etc)
            quint32 expirationTime; // voting expiration time in seconds;
            quint8 maxVotes;
            quint8 currentVote;
        };

        struct ServerInviteMessage
        {
            QString serverIP;
            quint16 serverPort;
            QString message;
        };

        bool isServerInvitation(const QString &message);
        ServerInviteMessage parseServerInviteMessage(const QString &message);

        bool isLocalUserVotingMessage(const QString &message);

        bool isFirstSystemVotingMessage(const QString &userName, const QString &message);

        SystemVotingMessage parseSystemVotingMessage(const QString &message);

        bool isAdminCommand(const QString &message);
        bool isPrivateMessage(const QString &message);
        QString extractDestinationUserNameFromPrivateMessage(const QString &text);

        bool isNinbotLevelMessage(const QString &message);
        QString extractUserNameFromNinbotLevelMessage(const QString &message);
        float extractDBValueFromNinbotLevelMessage(const QString &message);

    } // Chat namespace

} // Gui namespace

#endif
