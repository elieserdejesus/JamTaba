#ifndef NINJAM_VOTING_MESSAGE_PARSER_H
#define NINJAM_VOTING_MESSAGE_PARSER_H

#include <QString>

namespace Gui
{

    namespace Chat
    {

        extern const QRegularExpression VOTING_REGEX;

        class SystemVotingMessage
        {
        public:
            static SystemVotingMessage newBpiVotingMessage(quint32 bpi, quint32 expirationTime);
            static SystemVotingMessage newBpmVotingMessage(quint32 bpm, quint32 expirationTime);
            static SystemVotingMessage newEmptyVotingMessage();
            inline quint32 getExpirationTime() const { return expirationTime; }
            inline quint32 getVoteValue() const { return voteValue; }
            inline bool isBpiVotingMessage() const { return voteType == "BPI"; }
            inline bool isBpmVotingMessage() const { return voteType == "BPM"; }
            bool isValidVotingMessage() const;
        private:
            SystemVotingMessage(const QString &voteType, quint32 voteValue, quint32 expirationTime);
            QString voteType; // BPI or BPM
            quint32 voteValue; // the BPI or BPM value (90, 120, 16, 32, etc)
            quint32 expirationTime; // voting expiration time in seconds;
        };

        bool isSystemVotingMessage(const QString &userName, const QString &message);

        SystemVotingMessage parseSystemVotingMessage(const QString &message);

    } // Chat namespace

} // Gui namespace

#endif
