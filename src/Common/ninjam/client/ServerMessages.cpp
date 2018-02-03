#include "ServerMessages.h"
#include <QDebug>
#include <QDataStream>
#include "ninjam/client/UserChannel.h"
#include "ninjam/client/User.h"
#include "ninjam/client/Service.h"
#include "ninjam/Ninjam.h"

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
using ninjam::client::AuthChallengeMessage;
using ninjam::client::AuthReplyMessage;
using ninjam::client::ServerChatMessage;
using ninjam::client::ConfigChangeNotifyMessage;
using ninjam::client::ServerKeepAliveMessage;
using ninjam::client::UserInfoChangeNotifyMessage;
using ninjam::client::DownloadIntervalBegin;
using ninjam::client::DownloadIntervalWrite;
using ninjam::client::ChatCommandType;
using ninjam::client::User;
using ninjam::client::UserChannel;

ServerMessage::ServerMessage(ServerMessageType messageType) :
    messageType(messageType)
{
    //
}

ServerMessage::~ServerMessage()
{
}

// +++++++++++++++++++++  SERVER AUTH CHALLENGE+++++++++++++++

AuthChallengeMessage::AuthChallengeMessage(const QByteArray &challenge, const QString &licence, quint32 serverCapabilities, quint32 protocolVersion) :
    ServerMessage(ServerMessageType::AUTH_CHALLENGE),
    challenge(challenge),
    licenceAgreement(licence),
    serverCapabilities(serverCapabilities),
    protocolVersion(protocolVersion)
{

}

void AuthChallengeMessage::to(QIODevice *device) const
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << static_cast<quint8>(messageType);
    stream << static_cast<quint32>(8 + 4 + 4 + licenceAgreement.toUtf8().size() + 1); // payload

    stream.writeRawData(challenge.data(), challenge.capacity());
    stream << static_cast<quint32>(serverCapabilities);
    stream << static_cast<quint32>(protocolVersion);

    ninjam::serializeString(licenceAgreement, stream);
}

AuthChallengeMessage AuthChallengeMessage::from(QIODevice *device, quint32)
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    QByteArray challenge(8, Qt::Uninitialized);
    int readed = stream.readRawData(challenge.data(), challenge.capacity());
    Q_ASSERT(readed = 8);

    quint32 serverCapabilities;
    stream >> serverCapabilities;

    // If the Server Capabilities field has bit 0 set then the License Agreement is present.
    bool serverHasLicenceAgreement = serverCapabilities & 0xFFFFFFFF;

    quint32 protocolVersion;
    stream >> protocolVersion;
    //Q_ASSERT(protocolVersion == 0x00020000);

    QString licence;
    if (serverHasLicenceAgreement)
        licence = ninjam::client::extractString(stream);

    return AuthChallengeMessage(challenge, licence, serverCapabilities, protocolVersion);
}

void AuthChallengeMessage::printDebug(QDebug &dbg) const
{
    dbg << "RECEIVED ServerAuthChallengeMessage{" << endl
        << "\t challenge=" << getChallenge() << endl
        << "\t protocolVersion=" << getProtocolVersion()<< endl
        << "\t serverKeepAlivePeriod=" << getServerKeepAlivePeriod() << endl
        <<"}" << endl;
}

quint32 AuthChallengeMessage::getServerKeepAlivePeriod() const
{
    /**
        The Server Capabilities field bits 8-15 contains the client keepalive interval in seconds.
        The client sends a Keepalive message if it has sent no messages for the interval.
    */

    return serverCapabilities >> 8;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// +++++++++++++++++++++  SERVER AUTH REPLY ++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
AuthReplyMessage::AuthReplyMessage(quint8 flag, const QString &message, quint8 maxChannels) :
    ServerMessage(ServerMessageType::AUTH_REPLY),
    flag(flag),
    message(message),
    maxChannels(maxChannels)
{

}

void AuthReplyMessage::to(QIODevice *device) const
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << static_cast<quint8>(messageType);
    stream << static_cast<quint32>(1 + 1 + message.toUtf8().size() + 1); // payload

    stream << flag;

    ninjam::serializeString(message, stream);

    stream << maxChannels;
}

AuthReplyMessage AuthReplyMessage::from(QIODevice *device, quint32 payload)
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    quint8 flag;
    quint8 maxChannels;
    QString message;

    stream >> flag;
    quint32 stringSize = payload - sizeof(flag) - sizeof(maxChannels);
    message = extractString(stream, stringSize);

    stream >> maxChannels;

    return AuthReplyMessage(flag, message, maxChannels);
}

void AuthReplyMessage::printDebug(QDebug &debug) const
{
    debug << "RECEIVED ServerAuthReply{ flag=" << flag
          << " errorMessage='" << message
          << "' maxChannels=" << maxChannels << '}'
          << endl;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// +++++++++++++++++++++  SERVER KEEP ALIVE ++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
ServerKeepAliveMessage::ServerKeepAliveMessage() :
    ServerMessage(ServerMessageType::KEEP_ALIVE)
{

}

ServerKeepAliveMessage ServerKeepAliveMessage::from(QIODevice *, quint32)
{
    // keep alive don't have anything to read from the stream
    return ServerKeepAliveMessage();
}

void ServerKeepAliveMessage::printDebug(QDebug &dbg) const
{
    dbg << "RECEIVED ServerKeepAlive{ }";
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// +++++++++++++++++++++  SERVER CONFIG CHANGE NOTIFY ++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
ConfigChangeNotifyMessage::ConfigChangeNotifyMessage(quint16 bpm, quint16 bpi) :
    ServerMessage(ServerMessageType::SERVER_CONFIG_CHANGE_NOTIFY),
    bpm(bpm),
    bpi(bpi)
{

}

void ConfigChangeNotifyMessage::to(QIODevice *device) const
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << static_cast<quint8>(messageType);
    stream << static_cast<quint32>(4); // payload (2 x sizeof(quint16))

    stream << bpm;
    stream << bpi;
}

ConfigChangeNotifyMessage ConfigChangeNotifyMessage::from(QIODevice *device, quint32)
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    quint16 bpm;
    quint16 bpi;

    stream >> bpm;
    stream >> bpi;

    return ConfigChangeNotifyMessage(bpm, bpi);
}

void ConfigChangeNotifyMessage::printDebug(QDebug &dbg) const
{
    dbg << "RECEIVE ConfigChangeNotify{ bpm=" << bpm << ", bpi=" << bpi << "}" << endl;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// +++++++++++++++++++++  SERVER USER INFO CHANGE NOTIFY +++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
UserInfoChangeNotifyMessage::UserInfoChangeNotifyMessage() :
    ServerMessage(ServerMessageType::USER_INFO_CHANGE_NOTIFY)
{

}

void UserInfoChangeNotifyMessage::addUserChannel(const QString &userFullName, const UserChannel &channel)
{
    if (!users.contains(userFullName))
        users.insert(userFullName, User(userFullName));

    User &user = users[userFullName];
    user.addChannel(channel);
}

quint32 UserInfoChangeNotifyMessage::getPayload() const
{
    quint32 payload = 0;
    for (const User &user : users.values()) {
        for (const UserChannel &channel : user.getChannels()) {
            payload += 1; // Active
            payload += 1; // Channel Index
            payload += 2; // Volume
            payload += 1; // Pan
            payload += 1; // Flags
            payload += user.getFullName().toUtf8().size() + 1;
            payload += channel.getName().toUtf8().size() + 1;
        }
    }

    return payload;
}

void UserInfoChangeNotifyMessage::to(QIODevice *device) const
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << static_cast<quint8>(messageType);
    stream << static_cast<quint32>(getPayload());

    for (const User &user : users.values()) {
        for (const UserChannel &channel : user.getChannels()) {
            stream << static_cast<quint8>(channel.isActive() ? 1 : 0);
            stream << channel.getIndex();
            stream << static_cast<quint16>(0); // not sending volume
            stream << static_cast<quint8>(0); // not sending pan
            stream << channel.getFlags();
            ninjam::serializeString(user.getFullName(), stream);
            ninjam::serializeString(channel.getName(), stream);
        }
    }
}

UserInfoChangeNotifyMessage UserInfoChangeNotifyMessage::from(QIODevice *device, quint32 payload)
{
    UserInfoChangeNotifyMessage message;

    if (payload <= 0)  // payload is zero when server return no users
        return message; // will use empy user list;

    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

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
        bytesConsumed += userFullName.toUtf8().size() + 1;
        QString channelName = ninjam::client::extractString(stream);
        bytesConsumed += channelName.toUtf8().size() + 1;
        bool channelIsActive = active > 0 ? true : false;

        message.addUserChannel(userFullName, UserChannel(userFullName, channelName, channelIndex, channelIsActive, volume, pan, flags));
    }

    return message;
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
ServerChatMessage::ServerChatMessage(const QString &command, const QString &arg1, const QString &arg2, const QString &arg3, const QString &arg4) :
    ServerMessage(ServerMessageType::CHAT_MESSAGE),
    command(command)
{
    arguments.append(arg1);
    arguments.append(arg2);
    arguments.append(arg3);
    arguments.append(arg4);
}

quint32 ServerChatMessage::getPayload() const
{
    quint32 payload = command.size() + 1; // +1 to include nul terminator
    for (const QString &arg : arguments)
        payload += arg.toUtf8().size() + 1; // +1 to include nul terminator

    return payload;
}

void ServerChatMessage::to(QIODevice *device) const
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    Q_ASSERT(arguments.size() == 4);

    stream << static_cast<quint8>(messageType);
    stream << getPayload();

    ninjam::serializeString(command, stream);

    for (const QString &argument : arguments)
        ninjam::serializeString(argument, stream);
}

ServerChatMessage ServerChatMessage::from(QIODevice *device, quint32 payload)
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

    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    QByteArray byteArray(payload, Qt::Uninitialized);

    int readed = stream.readRawData(byteArray.data(), payload);

    Q_ASSERT(readed == payload);

    auto arrays = byteArray.split('\0');
    Q_ASSERT(arrays.size() >= 5);

    QString command(arrays.at(0));
    QString arg1 = QString::fromUtf8(arrays.at(1));
    QString arg2 = QString::fromUtf8(arrays.at(2));
    QString arg3 = QString::fromUtf8(arrays.at(3));
    QString arg4 = QString::fromUtf8(arrays.at(4));

    return ServerChatMessage(command, arg1, arg2, arg3,arg4);
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
    dbg << "RECEIVE ServerChatMessage{ command=" << command
        << " arguments=" << arguments << "}"
        << endl;
}

// ++++++++++++++++++++++++++++++++++++++++++++++
DownloadIntervalBegin::DownloadIntervalBegin(const QByteArray &GUID, quint32 estimatedSize, const QByteArray &fourCC, quint8 channelIndex, const QString &userName) :
    ServerMessage(ServerMessageType::DOWNLOAD_INTERVAL_BEGIN),
    GUID(GUID),
    estimatedSize(estimatedSize),
    channelIndex(channelIndex),
    userName(userName)
{
    Q_ASSERT(fourCC.size() == 4);
    Q_ASSERT(GUID.size() == 16);

    std::memcpy(this->fourCC, fourCC.data(), fourCC.size());
}

void DownloadIntervalBegin::to(QIODevice *device) const
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << static_cast<quint8>(messageType);
    stream << static_cast<quint32>(16 + 4 + 4 + 1 + userName.toUtf8().size() + 1); // payload

    ninjam::serializeByteArray(GUID, stream);

    stream << estimatedSize;

    stream.writeRawData(fourCC, 4);

    stream << channelIndex;

    ninjam::serializeString(userName, stream);
}

DownloadIntervalBegin DownloadIntervalBegin::from(QIODevice *device, quint32 payload)
{
    QByteArray GUID(16, Qt::Uninitialized);
    QByteArray fourCC(4, Qt::Uninitialized);

    quint32 estimatedSize;
    quint8 channelIndex;

    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream.readRawData(GUID.data(), GUID.size()); // 16 bytes

    stream >> estimatedSize;

    stream.readRawData(fourCC.data(), fourCC.size()); // 4 bytes

    stream >> channelIndex;

    quint32 stringSize = payload - 16 - sizeof(estimatedSize) - 4 - sizeof(channelIndex);
    QString userName = extractString(stream, stringSize);

    return DownloadIntervalBegin(GUID, estimatedSize, fourCC, channelIndex, userName);
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

// -------------------------------------------------------------------

DownloadIntervalWrite::DownloadIntervalWrite(const QByteArray &GUID, quint8 flags, const QByteArray &encodedData) :
    ServerMessage(ServerMessageType::DOWNLOAD_INTERVAL_WRITE),
    GUID(GUID),
    flags(flags),
    encodedData(encodedData)
{
    Q_ASSERT(GUID.size() == 16);
}

void DownloadIntervalWrite::printDebug(QDebug &dbg) const
{
    dbg << "RECEIVE DownloadIntervalWrite{ flags='" << flags << "' GUID={" << GUID
        << "} downloadIsComplete=" << downloadIsComplete() << ", audioData="
        << encodedData.size() << " bytes }";
}

void DownloadIntervalWrite::to(QIODevice *device) const
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    // header
    stream << static_cast<quint8>(messageType);
    stream << static_cast<quint32>(16 + 1 + encodedData.size()); // payload

    ninjam::serializeByteArray(GUID, stream);

    stream << flags;

    ninjam::serializeByteArray(encodedData, stream);
}

DownloadIntervalWrite DownloadIntervalWrite::from(QIODevice *device, quint32 payload)
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    QByteArray GUID(16, Qt::Uninitialized);

    stream.readRawData(GUID.data(), GUID.size()); // 16 bytes

    quint8 flags;
    stream >> flags;

    QByteArray encodedData(payload - 17, Qt::Uninitialized);
    int bytesReaded = stream.readRawData(encodedData.data(), encodedData.size());

    Q_ASSERT(bytesReaded == encodedData.size());

    return DownloadIntervalWrite(GUID, flags, encodedData);
}


