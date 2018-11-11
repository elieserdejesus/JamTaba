#include "NinjamChatMessageParser.h"
#include <QRegularExpression>
#include <QDebug>

using gui::chat::SystemVotingMessage;

/** System voting format is: [voting system] leading candidate: 1/2 votes for 12 BPI [each vote expires in 60s] */
const QRegularExpression gui::chat::SYSTEM_VOTING_REGEX("\\[voting system\\] leading candidate: (\\d{1,2})\\/(\\d{1,2}) votes for (\\d{1,3}) (\\bBPI|\\bBPM) \\[each vote expires in (\\d{1,3})s\\]");

/** Local user voting format is: !vote bpi/bpm 120, always in lower case */
const QRegularExpression gui::chat::LOCAL_USER_VOTING_REGEX("!vote (\\bbpi|\\bbpm) \\d{1,3}");

const QRegularExpression gui::chat::ADMIN_COMMAND_REGEX("^/bpi|^/bpm|^/kick|^/topic");

const QRegularExpression gui::chat::PRIVATE_MESSAGE_REGEX("^/msg");

const QRegularExpression gui::chat::PUBLIC_SERVER_INVITE_REGEX("^(Let's play in ([A-Za-z\\.0-9]+) \\: ([0-9]{4}) \\?)");
const QRegularExpression gui::chat::PRIVATE_SERVER_INVITE_REGEX("^(Let's play in my private server\\?)\\n\\n IP\\: ([A-Za-z\\.0-9]+) \\n\\n PORT\\: ([0-9]{4}) \\n");

const QRegularExpression gui::chat::NINBOT_LEVEL_MESSAGE_REGEX("^(.+), you are clipping [ ]?([0-9]+\\.[0-9]+) db");

bool gui::chat::isNinbotLevelMessage(const QString &message)
{
    return NINBOT_LEVEL_MESSAGE_REGEX.match(message).hasMatch();
}

QString gui::chat::extractUserNameFromNinbotLevelMessage(const QString &message)
{
    if (isNinbotLevelMessage(message)) {
        auto match = NINBOT_LEVEL_MESSAGE_REGEX.match(message);
        if (match.lastCapturedIndex() >= 1)
            return match.captured(1); // return the second captured (the first group is the entire message)
    }

    return QString();
}

float gui::chat::extractDBValueFromNinbotLevelMessage(const QString &message)
{
    if (isNinbotLevelMessage(message)) {
        auto match = NINBOT_LEVEL_MESSAGE_REGEX.match(message);
        if (match.lastCapturedIndex() >= 2)
            return match.captured(2).toFloat(); // return the third captured group (the first group is the entire message, the 2nd is the user name and the 3rd is the dB value)
    }

    return 0;
}

QString gui::chat::extractDestinationUserNameFromPrivateMessage(const QString &text)
{
    QString t(text);
    t.replace("/msg ", "");
    int index = t.indexOf(QChar(' '));
    if (index > 0) {
        return t.left(index);
    }

    return text;
}

bool gui::chat::isServerInvitation(const QString &message)
{
    /**
        Let's play in %1 : %2 ?
        Let's play in my private server?
        \n\n IP: %1 \n\n PORT: %2 \n
    */

    return PUBLIC_SERVER_INVITE_REGEX.match(message).hasMatch()
            || PRIVATE_SERVER_INVITE_REGEX.match(message).hasMatch();
}

gui::chat::ServerInviteMessage gui::chat::parseServerInviteMessage(const QString &message)
{
    ServerInviteMessage msg;

    QString text;
    QString serverIP;
    quint16 serverPort = 0;

    auto matcher = PUBLIC_SERVER_INVITE_REGEX.match(message);
    if (!matcher.hasMatch())
        matcher = PRIVATE_SERVER_INVITE_REGEX.match(message);

    text = matcher.captured(1);
    serverIP = matcher.captured(2);
    serverPort = matcher.captured(3).toInt();

    msg.message = text;
    msg.serverIP = serverIP;
    msg.serverPort = serverPort;

    return msg;
}

bool gui::chat::isPrivateMessage(const QString &message)
{
    return PRIVATE_MESSAGE_REGEX.match(message).hasMatch();
}

bool gui::chat::isAdminCommand(const QString &message)
{
    return ADMIN_COMMAND_REGEX.match(message).hasMatch();
}

bool gui::chat::isLocalUserVotingMessage(const QString &message)
{
    return LOCAL_USER_VOTING_REGEX.match(message).hasMatch();
}

bool gui::chat::isFirstSystemVotingMessage(const QString &userName, const QString &message)
{
    if (!userName.isEmpty())
        return false; // the user name in system voting message is always empty. If we have a valid user name somebody it trying to inpersonate the "server" :)

    SystemVotingMessage msg = parseSystemVotingMessage(message);
    return msg.isValidVotingMessage() && msg.isFirstVotingMessage();
}

SystemVotingMessage gui::chat::parseSystemVotingMessage(const QString &message)
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

bool gui::chat::SystemVotingMessage::isValidVotingMessage() const
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
