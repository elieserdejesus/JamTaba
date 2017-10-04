#include "NinjamVotingMessageParser.h"
#include <QRegularExpression>
#include <QDebug>

using namespace Gui::Chat;

/** System voting format is: [voting system] leading candidate: 1/2 votes for 12 BPI [each vote expires in 60s] */
const QRegularExpression Gui::Chat::SYSTEM_VOTING_REGEX("\\[voting system\\] leading candidate: (\\d{1,2})\\/(\\d{1,2}) votes for (\\d{1,3}) (\\bBPI|\\bBPM) \\[each vote expires in (\\d{1,3})s\\]");

/** Local user voting format is: !vote bpi/bpm 120, always in lower case */
const QRegularExpression Gui::Chat::LOCAL_USER_VOTING_REGEX("!vote (\\bbpi|\\bbpm) \\d{1,3}");

const QRegularExpression Gui::Chat::ADMIN_COMMAND_REGEX("^/");


bool Gui::Chat::isAdminCommand(const QString &message)
{
    return ADMIN_COMMAND_REGEX.match(message).hasMatch();
}

bool Gui::Chat::isLocalUserVotingMessage(const QString &message)
{
    return LOCAL_USER_VOTING_REGEX.match(message).hasMatch();
}

bool Gui::Chat::isFirstSystemVotingMessage(const QString &userName, const QString &message)
{
    if (!userName.isEmpty())
        return false; // the user name in system voting message is always empty. If we have a valid user name somebody it trying to inpersonate the "server" :)

    SystemVotingMessage msg = parseSystemVotingMessage(message);
    return msg.isValidVotingMessage() && msg.isFirstVotingMessage();
}

SystemVotingMessage Gui::Chat::parseSystemVotingMessage(const QString &message)
{
    QRegularExpressionMatch match = SYSTEM_VOTING_REGEX.match(message);

    if (!match.hasMatch())
        return SystemVotingMessage::newEmptyVotingMessage(); // return an invalid voting message.

    int currentVote = match.captured(1).toInt();
    int maxVotes = match.captured(2).toInt();

    QString voteValueString = match.captured(3); // vote value
    if (voteValueString.isNull()) // invalid vote value
        return SystemVotingMessage::newEmptyVotingMessage();

    QString voteType = match.captured(4); // BPI or BPM
    if (voteType.isNull() || (voteType != "BPI" && voteType != "BPM")) // invalid vote type?
        return SystemVotingMessage::newEmptyVotingMessage();

    QString expirationString = match.captured(5); // expitarion time
    if (expirationString.isNull())
        return SystemVotingMessage::newEmptyVotingMessage();

    quint32 voteValue = voteValueString.toInt();
    quint32 expirationTime = expirationString.toInt();

    if (voteType == "BPI")
        return SystemVotingMessage::newBpiVotingMessage(voteValue, expirationTime, currentVote, maxVotes);

    return SystemVotingMessage::newBpmVotingMessage(voteValue, expirationTime, currentVote, maxVotes);
}

bool Gui::Chat::SystemVotingMessage::isValidVotingMessage() const
{
    if (voteType != "BPI" && voteType != "BPM")
        return false;

    if (voteValue == 0 || expirationTime == 0)
        return false;

    return true;
}

SystemVotingMessage SystemVotingMessage::newBpiVotingMessage(quint32 bpi, quint32 expirationTime, quint8 currentVote, quint8 maxVotes)
{
    return SystemVotingMessage("BPI", bpi, expirationTime, currentVote, maxVotes);
}

SystemVotingMessage SystemVotingMessage::newBpmVotingMessage(quint32 bpm, quint32 expirationTime, quint8 currentVote, quint8 maxVotes)
{
    return SystemVotingMessage("BPM", bpm, expirationTime, currentVote, maxVotes);
}

SystemVotingMessage SystemVotingMessage::newEmptyVotingMessage()
{
    return SystemVotingMessage("", 0, 0, 0, 0);
}

SystemVotingMessage::SystemVotingMessage(const QString &voteType, quint32 voteValue, quint32 expirationTime, quint8 currentVote, quint8 maxVotes) :
      voteType(voteType),
      voteValue(voteValue),
      expirationTime(expirationTime),
      currentVote(currentVote),
      maxVotes(maxVotes)
{

}
