#include "ServerMessages.h"
#include <QDebug>
#include <QDataStream>
#include "ninjam/client/UserChannel.h"
#include "ninjam/client/User.h"
#include "ninjam/client/Service.h"

QString ninjam::client::extractString(QDataStream &stream)
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

QString extractString(QDataStream &stream, quint32 size)
{
    return QString::fromUtf8(stream.device()->read(size));
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// +++++++++++++  SERVER MESSAGE (Base class) +++++++++++++++=
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=

using ninjam::client::ServerMessage;
using ninjam::client::ServerMessageType;
using ninjam::client::ServerAuthChallengeMessage;
using ninjam::client::ServerAuthReplyMessage;
using ninjam::client::ServerChatMessage;
using ninjam::client::ServerConfigChangeNotifyMessage;
using ninjam::client::ServerKeepAliveMessage;
using ninjam::client::UserInfoChangeNotifyMessage;
using ninjam::client::DownloadIntervalBegin;
using ninjam::client::DownloadIntervalWrite;
using ninjam::client::ChatCommandType;

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
        licenceAgreement = ninjam::client::extractString(stream);
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

    quint32 stringSize = payload - sizeof(flag) - sizeof(maxChannels);
    message = extractString(stream, stringSize);

    stream >> maxChannels;
}

void ServerAuthReplyMessage::printDebug(QDebug &debug) const
{
    debug << "RECEIVED ServerAuthReply{ flag=" << flag
          << " errorMessage='" << message
          << "' maxChannels=" << maxChannels << '}'
          << endl;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// +++++++++++++++++++++  SERVER KEEP ALIVE ++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
ServerKeepAliveMessage::ServerKeepAliveMessage(quint32 payload) :
    ServerMessage(ServerMessageType::KEEP_ALIVE, 0) // no payload in KeepAlive, but we need the payload constructor parameter to call this constructor using template (all messages receive a payload as parameter in constructor)
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
        return; // will use empy user list;

    unsigned int bytesConsumed = 0;
    while (bytesConsumed < payload) {
        quint8 active;
        quint8 channelIndex;
        quint16 volume;
        quint8 pan;
        quint8 flags;
        stream >> active >> channelIndex >> volume >> pan >> flags;
        bytesConsumed += 6;
        QString userFullName = ninjam::client::extractString(stream);
        if(!users.contains(userFullName)){
            users.insert(userFullName, User(userFullName));
        }
        User &user = users[userFullName];
        bytesConsumed += userFullName.toUtf8().size() + 1;
        QString channelName = ninjam::client::extractString(stream);
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
    for (const User &user : users.values()) {
        dbg << "\t" << user.getFullName() << "\n";
        for (const UserChannel &channel : user.getChannels()) {
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
    ServerMessage(ServerMessageType::CHAT_MESSAGE, payload),
    commandType(ChatCommandType::MSG)
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

    const static uint BUFFER_SIZE = 4096;
    static char data[BUFFER_SIZE];

    int readed = stream.readRawData(data, qMin(static_cast<uint>(payload), BUFFER_SIZE));

    auto strings = QString::fromUtf8(data, readed).split(QChar('\0'));

    commandType = commandTypeFromString(strings.takeFirst()); // the first string is the msg command

    arguments = strings; // the 4 last strings are the arguments
}

ChatCommandType ServerChatMessage::commandTypeFromString(const QString &string)
{
    if (string == "MSG")
        return ChatCommandType::MSG;

    if (string == "PRIVMSG")
        return ChatCommandType::PRIVMSG;

    if (string == "TOPIC")
        return ChatCommandType::TOPIC;

    if (string == "JOIN")
        return ChatCommandType::JOIN;

    if (string == "PART")
        return ChatCommandType::PART;

    return ChatCommandType::USERCOUNT; /*if(string == "USERCOUNT")*/
}

void ServerChatMessage::printDebug(QDebug &dbg) const
{
    dbg << "RECEIVE ServerChatMessage{ command=" << static_cast<quint8>(commandType)
        << " arguments="
        << arguments << "}" << endl;
}

// ++++++++++++++++++++++++++++++++++++++++++++++
DownloadIntervalBegin::DownloadIntervalBegin(quint32 payload) :
    ServerMessage(ServerMessageType::DOWNLOAD_INTERVAL_BEGIN, payload),
    GUID(16, Qt::Uninitialized)
{
    //
}

void DownloadIntervalBegin::readFrom(QDataStream &stream)
{
    stream.readRawData(GUID.data(), GUID.size()); // 16 bytes

    stream >> estimatedSize;

    stream.readRawData(&fourCC[0], 4); // 4 bytes

    stream >> channelIndex;

    quint32 stringSize = payload - 16 - sizeof(estimatedSize) - 4 - sizeof(channelIndex);
    userName = extractString(stream, stringSize);
}

bool DownloadIntervalBegin::isAudio() const
{
   return  fourCC[0] == 'O' &&
           fourCC[1] == 'G' &&
           fourCC[2] == 'G' &&
           fourCC[3] == 'v';
}

bool DownloadIntervalBegin::isVideo() const
{
   return !isAudio();
}

void DownloadIntervalBegin::printDebug(QDebug &dbg) const
{
    dbg << "DownloadIntervalBegin{ " <<endl
        << "\tfourCC='"<< fourCC[0] << fourCC[1] << fourCC[2] << fourCC[3] << endl
        << "\tGUID={"<< GUID << "} " << endl
        << "\tisValidOggDownload="<< isAudio() << endl
        << "\tdownloadShoudBeStopped="<< shouldBeStopped() << endl
        << "\tdownloadIsComplete="<< isComplete() << endl
        << "\testimatedSize=" << estimatedSize << endl
        << "\tchannelIndex=" << channelIndex  << endl
        << "\tuserName=" << userName << endl <<"}" << endl;
}

void DownloadIntervalWrite::printDebug(QDebug &dbg) const
{
    dbg << "RECEIVE DownloadIntervalWrite{ flags='" << flags << "' GUID={" << GUID
        << "} downloadIsComplete=" << downloadIsComplete() << ", audioData="
        << encodedData.size() << " bytes }";
}

DownloadIntervalWrite::DownloadIntervalWrite(quint32 payload) :
    ServerMessage(ServerMessageType::DOWNLOAD_INTERVAL_WRITE, payload),
    GUID(16, Qt::Uninitialized),
    encodedData(payload - 17, Qt::Uninitialized)
{

}

void DownloadIntervalWrite::readFrom(QDataStream &stream)
{
    stream.readRawData(GUID.data(), 16); // 16 bytes

    stream >> flags;

    int bytesReaded = stream.readRawData(encodedData.data(), encodedData.size());
    if (bytesReaded < 0)
        qWarning() << "Error reading encoded data!  return:" << bytesReaded << " payload:" << payload << " encodedData.size:" << encodedData.size();
}


