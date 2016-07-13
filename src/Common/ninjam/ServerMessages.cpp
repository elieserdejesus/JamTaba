#include "ServerMessages.h"
#include <QDebug>
#include <QDataStream>
#include "ninjam/UserChannel.h"
#include "ninjam/User.h"
#include "ninjam/Service.h"

using namespace Ninjam;

namespace Ninjam {

//some functions used only in the Ninjam namespace...

int getRawStringSize(char *data, int maxLenght)
{
    int p = 0;
    for (; p < maxLenght-1; ++p) {
        if (data[p] == '\0')
            break;
    }
    return p + 1;
}

QString extractString(QDataStream &stream)
{
    quint8 byte;
    QByteArray byteArray;
    while (!stream.atEnd()) {
        stream >> byte;
        if (byte == '\0')
            break;
        byteArray.append(byte);
    }
    return QString::fromUtf8(byteArray.data(), byteArray.size());
}
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// +++++++++++++  SERVER MESSAGE (Base class) +++++++++++++++=
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
ServerMessage::ServerMessage(ServerMessageType messageType, quint32 payload) :
    messageType(messageType),
    payload(payload)
{
    //
}

ServerMessage::~ServerMessage()
{
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// +++++++++++++++++++++  SERVER AUTH CHALLENGE+++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
ServerAuthChallengeMessage::ServerAuthChallengeMessage(quint32 payload) :
    ServerMessage(ServerMessageType::AUTH_CHALLENGE, payload)
{

}

void ServerAuthChallengeMessage::readFrom(QDataStream &stream)
{
    challenge.reserve(8);
    stream.readRawData(challenge.data(), challenge.capacity());

    quint32 serverCapabilities;
    stream >> serverCapabilities;

    // If the Server Capabilities field has bit 0 set then the License Agreement is present.
    bool serverHasLicenceAgreement = serverCapabilities & 0xFFFFFFFF;

    // The Server Capabilities field bits 8-15 contains the client keepalive interval in seconds. The client sends a Keepalive message if it has sent no messages for the interval.
    serverKeepAlivePeriod = static_cast<quint8>(serverCapabilities >> 8);

    stream >> protocolVersion;
    //Q_ASSERT(protocolVersion == 0x00020000);

    if (serverHasLicenceAgreement)
        licenceAgreement = Ninjam::extractString(stream);
}

void ServerAuthChallengeMessage::printDebug(QDebug &dbg) const
{
    dbg << "RECEIVED ServerAuthChallengeMessage{" << endl
        << "\t challenge=" << getChallenge() << endl
        << "\t protocolVersion=" << getProtocolVersion()<< endl
        << "\t serverKeepAlivePeriod=" << getServerKeepAlivePeriod() << endl
        <<"}" << endl;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// +++++++++++++++++++++  SERVER AUTH REPLY ++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
ServerAuthReplyMessage::ServerAuthReplyMessage(quint32 payload) :
    ServerMessage(ServerMessageType::AUTH_REPLY, payload)
{
}

void ServerAuthReplyMessage::readFrom(QDataStream &stream)
{
    stream >> flag;
    message = Ninjam::extractString(stream);
    stream >> maxChannels;
}

void ServerAuthReplyMessage::printDebug(QDebug &debug) const
{
    debug << "RECEIVED ServerAuthReply{ flag=" << flag << " errorMessage='" << message
          << "' maxChannels=" << maxChannels << '}' << endl;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// +++++++++++++++++++++  SERVER KEEP ALIVE ++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
ServerKeepAliveMessage::ServerKeepAliveMessage(quint32 payload) :
    ServerMessage(ServerMessageType::KEEP_ALIVE, 0)// no payload in KeepAlive, but we need the payload constructor parameter to call this constructor using template (all messages receive a payload as parameter in constructor)
{
    Q_UNUSED(payload)
}

void ServerKeepAliveMessage::readFrom(QDataStream &)
{
    // keep alive don't have anything to read from the stream
}

void ServerKeepAliveMessage::printDebug(QDebug &dbg) const
{
    dbg << "RECEIVED ServerKeepAlive{ }";
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// +++++++++++++++++++++  SERVER CONFIG CHANGE NOTIFY ++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
ServerConfigChangeNotifyMessage::ServerConfigChangeNotifyMessage(quint32 payload) :
    ServerMessage(ServerMessageType::SERVER_CONFIG_CHANGE_NOTIFY, payload)
{
}

void ServerConfigChangeNotifyMessage::readFrom(QDataStream &stream)
{
    stream >> bpm;
    stream >> bpi;
}

void ServerConfigChangeNotifyMessage::printDebug(QDebug &dbg) const
{
    dbg << "RECEIVE ConfigChangeNotify{ bpm=" << bpm << ", bpi=" << bpi << "}" << endl;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// +++++++++++++++++++++  SERVER USER INFO CHANGE NOTIFY +++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
UserInfoChangeNotifyMessage::UserInfoChangeNotifyMessage(quint32 payload) :
    ServerMessage(ServerMessageType::USER_INFO_CHANGE_NOTIFY, payload)
{
}

void UserInfoChangeNotifyMessage::readFrom(QDataStream &stream)
{
    if (payload <= 0)  // payload is zero when server return no users
        return;// will use empy user list;

    unsigned int bytesConsumed = 0;
    while (bytesConsumed < payload) {
        quint8 active;
        quint8 channelIndex;
        quint16 volume;
        quint8 pan;
        quint8 flags;
        stream >> active >> channelIndex >> volume >> pan >> flags;
        bytesConsumed += 6;
        QString userFullName = Ninjam::extractString(stream);
        if(!users.contains(userFullName)){
            users.insert(userFullName, User(userFullName));
        }
        User &user = users[userFullName];
        bytesConsumed += userFullName.toUtf8().size() + 1;
        QString channelName = Ninjam::extractString(stream);
        bytesConsumed += channelName.toUtf8().size() + 1;
        bool channelIsActive = active > 0 ? true : false;
        user.addChannel(UserChannel(userFullName, channelName, channelIndex, channelIsActive,
                                    volume, pan, flags));
    }
}

UserInfoChangeNotifyMessage::~UserInfoChangeNotifyMessage()
{
    // qWarning() << "destrutor UserInfoChangeNotifyMessage";
}

void UserInfoChangeNotifyMessage::printDebug(QDebug &dbg) const
{
    dbg << "UserInfoChangeNotify{\n";
    foreach (const User &user, users.values()){
        dbg << "\t" << user.getFullName() << "\n";
        foreach (const UserChannel &channel, user.getChannels()) {
            dbg << "\t\t" << channel.getName() << "\n";;
        }
        dbg << "\n";
    }
    dbg << "}";
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++ SERVER CHAT MESSAGE +++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ServerChatMessage::ServerChatMessage(quint32 payload) :
    ServerMessage(ServerMessageType::CHAT_MESSAGE, payload)
{
}

void ServerChatMessage::readFrom(QDataStream &stream)
{
    /*
     Offset Type Field
     0x0    ...  Command (NUL-terminated)
     a+0x0  ...  Argument 1 (NUL-terminated)
     b+0x0  ...  Argument 2 (NUL-terminated)
     c+0x0  ...  Argument 3 (NUL-terminated)
     d+0x0  ...  Argument 4 (NUL-terminated)

     The server-to-client commands are:
     MSG <username> <text> -- a broadcast message
     PRIVMSG <username> <text> -- a private message
     TOPIC <username> <text> -- server topic change
     JOIN <username> -- user enters server
     PART <username> -- user leaves server
     USERCOUNT <users> <maxusers> -- server status
     */

    static char data[4096];
    stream.readRawData(data, qMin((int)payload, 4096));
    quint32 consumedBytes = 0;

    int commandStringSize = Ninjam::getRawStringSize(data, payload);
    QString commandString = QString::fromUtf8(data, commandStringSize-1);// remove the NULL terminator
    commandType = commandTypeFromString(commandString);

    consumedBytes += commandStringSize;

    int parsedArgs = 0;
    while (consumedBytes < payload && parsedArgs < 4) {
        int argStringSize = Ninjam::getRawStringSize(data + consumedBytes, payload - consumedBytes);
        QString arg = QString::fromUtf8(data + consumedBytes, argStringSize-1);
        arguments.append(arg);
        consumedBytes += argStringSize;
        parsedArgs++;
    }
}

ChatCommandType ServerChatMessage::commandTypeFromString(const QString &string)
{
    // "MSG", "PRIVMSG", "TOPIC", "JOIN", "PART", "USERCOUNT"
    if (string == "MSG") return ChatCommandType::MSG;
    if (string == "PRIVMSG") return ChatCommandType::PRIVMSG;
    if (string == "TOPIC") return ChatCommandType::TOPIC;
    if (string == "JOIN") return ChatCommandType::JOIN;
    if (string == "PART") return ChatCommandType::PART;
    /*if(string == "USERCOUNT")*/ return ChatCommandType::USERCOUNT;
}

void ServerChatMessage::printDebug(QDebug &dbg) const
{
    dbg << "RECEIVE ServerChatMessage{ command=" << static_cast<quint8>(commandType)
        << " arguments="
        << arguments << "}" << endl;
}

// ++++++++++++++++++++++++++++++++++++++++++++++
DownloadIntervalBegin::DownloadIntervalBegin(quint32 payload) :
    ServerMessage(ServerMessageType::DOWNLOAD_INTERVAL_BEGIN, payload)
{
    // for (int i = 0; i < 4; ++i)
    // this->fourCC[i] = fourCC[i];
    // isValidOgg = fourCC[0] == 'O' && fourCC[1] == 'G' && fourCC[2] == 'G' && fourCC[3] == 'v';
}

void DownloadIntervalBegin::readFrom(QDataStream &stream)
{
    // TODO the code to read GUID bytes is duplicated in DownloadIntervalWrite
    quint8 byte;
    for (int i = 0; i < 16; ++i) {
        stream >> byte;
        GUID.append(byte);
    }
    stream >> estimatedSize;
    for (int i = 0; i < 4; ++i)
        stream >> fourCC[i];
    stream >> channelIndex;
    userName = Ninjam::extractString(stream);

    isValidOgg = fourCC[0] == 'O' && fourCC[1] == 'G' && fourCC[2] == 'G' && fourCC[3] == 'v';
}

void DownloadIntervalBegin::printDebug(QDebug &dbg) const
{
    dbg << "DownloadIntervalBegin{ " <<endl
        << "\tfourCC='"<< fourCC[0] << fourCC[1] << fourCC[2] << fourCC[3] << endl
        << "\tGUID={"<< GUID << "} " << endl
        << "\tisValidOggDownload="<< isValidOggDownload() << endl
        << "\tdownloadShoudBeStopped="<< downloadShouldBeStopped() << endl
        << "\tdownloadIsComplete="<< downloadIsComplete() << endl
        << "\testimatedSize=" << estimatedSize << endl
        << "\tchannelIndex=" << channelIndex  << endl
        << "\tuserName=" << userName << endl <<"}" << endl;
}

void DownloadIntervalWrite::printDebug(QDebug &dbg) const
{
    dbg << "RECEIVE DownloadIntervalWrite{ flags='" << flags << "' GUID={" << GUID
        << "} downloadIsComplete=" << downloadIsComplete() << ", audioData="
        << encodedAudioData.size() << " bytes }";
}

DownloadIntervalWrite::DownloadIntervalWrite(quint32 payload) :
    ServerMessage(ServerMessageType::DOWNLOAD_INTERVAL_WRITE, payload)
{
}

void DownloadIntervalWrite::readFrom(QDataStream &stream)
{
    quint8 byte;
    for (int i = 0; i < 16; ++i) {
        stream >> byte;
        GUID.append(byte);
    }

    stream >> flags;

    quint32 lenght = payload - 17;
    encodedAudioData.resize(lenght);
    int bytesReaded = stream.readRawData(encodedAudioData.data(), lenght);
    if (bytesReaded <= 0)
        qWarning() << "Error reading encoded audio! "  << bytesReaded;
}

// ++++++++++++++++++

QDataStream& Ninjam::operator >>(QDataStream &stream, ServerMessage &message)
{
    message.readFrom(stream);
    return stream;
}

// +++++++++++++++++++++++++++++++++++++++++=


