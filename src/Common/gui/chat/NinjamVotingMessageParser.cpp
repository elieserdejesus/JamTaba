#include "NinjamVotingMessageParser.h"
#include <QRegularExpression>
#include <QDebug>

using namespace Gui::Chat;

// System voting format is: [voting system] leading candidate: 1/2 votes for 12 BPI [each vote expires in 60s]
const QRegularExpression Gui::Chat::VOTING_REGEX("\\[voting system\\] leading candidate: \\d{1,2}\\/\\d{1,2} votes for (\\d{1,3}) (\\bBPI|\\bBPM) \\[each vote expires in (\\d{1,3})s\\]");

bool Gui::Chat::isSystemVotingMessage(const QString &userName, const QString &message)
{
    if (!userName.isEmpty())
        return false; // the user name in system voting message is always empty. If we have a valid user name somebody it trying to inpersonate the "server" :)

    return VOTING_REGEX.match(message).hasMatch();
}

SystemVotingMessage Gui::Chat::parseVotingMessage(const QString &message)
{
    if (!isSystemVotingMessage("", message))
        return SystemVotingMessage::newEmptyVotingMessage(); //return an invalid voting message.

    QRegularExpressionMatch match = VOTING_REGEX.match(message);

    QString voteValueString = match.captured(1);
    if (voteValueString.isNull()) //invalid vote value
        return SystemVotingMessage::newEmptyVotingMessage();

    QString voteType = match.captured(2); //BPI or BPM
    if (voteType.isNull() || (voteType != "BPI" && voteType != "BPM")) //invalid vote type?
        return SystemVotingMessage::newEmptyVotingMessage();

    QString expirationString = match.captured(3);
    if (expirationString.isNull())
        return SystemVotingMessage::newEmptyVotingMessage();

    bool stringConversionResult = false;
    quint32 voteValue = (quint32)voteValueString.toInt(&stringConversionResult);
    if (!stringConversionResult)
        return SystemVotingMessage::newEmptyVotingMessage();

    quint32 expirationTime = (quint32)expirationString.toInt(&stringConversionResult);
    if (!stringConversionResult)
        return SystemVotingMessage::newEmptyVotingMessage();

    if (voteType == "BPI")
        return SystemVotingMessage::newBpiVotingMessage(voteValue, expirationTime);

    return SystemVotingMessage::newBpmVotingMessage(voteValue, expirationTime);
}

bool Gui::Chat::SystemVotingMessage::isValidVotingMessage() const
{
    if ( voteType != "BPI" && voteType != "BPM")
        return false;

    if (voteValue == 0 || expirationTime == 0)
        return false;

    return true;
}

SystemVotingMessage SystemVotingMessage::newBpiVotingMessage(quint32 bpi, quint32 expirationTime)
{
    return SystemVotingMessage("BPI", bpi, expirationTime);
}

SystemVotingMessage SystemVotingMessage::newBpmVotingMessage(quint32 bpm, quint32 expirationTime)
{
    return SystemVotingMessage("BPM", bpm, expirationTime);
}

SystemVotingMessage SystemVotingMessage::newEmptyVotingMessage()
{
    return SystemVotingMessage("", 0, 0);
}

SystemVotingMessage::SystemVotingMessage(const QString &voteType, quint32 voteValue, quint32 expirationTime)
    : voteType(voteType),
      voteValue(voteValue),
      expirationTime(expirationTime)
{

}
