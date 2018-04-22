#include "ClientMessages.h"
#include "ninjam/client/User.h"
#include "ninjam/client/UserChannel.h"
#include "ninjam/Ninjam.h"

#include <QCryptographicHash>
#include <QIODevice>
#include <QDebug>
#include <QDataStream>
#include <QString>

using ninjam::client::ClientMessage;
using ninjam::client::ClientAuthUserMessage;
using ninjam::client::UploadIntervalWrite;
using ninjam::client::ClientKeepAlive;
using ninjam::client::ClientSetChannel;
using ninjam::client::ClientSetUserMask;
using ninjam::client::UploadIntervalBegin;
using ninjam::client::ClientToServerChatMessage;
using ninjam::client::UserChannel;
using ninjam::MessageType;

ClientMessage::ClientMessage(MessageType type, quint32 payload) :
    msgType(type),
    payload(payload)
{
    //
}

ClientMessage::~ClientMessage()
{
    //
}

//++++++++++++++++++++++++++++++++++++++=
//class ClientAuthUser
/*
     //message type 0x80

     Offset Type        Field
     0x0    uint8_t[20] Password Hash (binary hash value)
     0x14   ...         Username (NUL-terminated)
     x+0x0  uint32_t    Client Capabilities
     x+0x4  uint32_t    Client Version

     The Password Hash field is calculated by SHA1(SHA1(username + ":" + password) + challenge).
     If the user acknowledged a license agreement from the server then Client Capabilities bit 0 is set.
     The server responds with Server Auth Reply.

    //message lenght = 20 bytes password hash + user name lengh + 4 bytes client capabilites + 4 bytes client version
*/

ClientAuthUserMessage::ClientAuthUserMessage(const QString &userName, const QByteArray &challenge, quint32 protocolVersion, const QString &password)
    : ClientMessage(MessageType::ClientAuthUser, 0),
      userName(userName),
      clientCapabilites(1),
      protocolVersion(protocolVersion),
      challenge(challenge)
{
    if (!password.isNull() && !password.isEmpty()) {
        this->userName = userName;
    }
    else{
        this->userName = "anonymous:" + userName;
    }

    QCryptographicHash sha1(QCryptographicHash::Sha1);
    sha1.addData( userName.toStdString().c_str(), userName.size() );
    sha1.addData(":", 1);
    sha1.addData(password.toStdString().c_str(), password.size());
    QByteArray passHash = sha1.result();
    sha1.reset();
    sha1.addData(passHash);
    sha1.addData(challenge.constData(), 8);
    this->passwordHash = sha1.result();
    this->payload = 29 + this->userName.size();
}

ClientAuthUserMessage ClientAuthUserMessage::unserializeFrom(QIODevice *device, quint32 payload)
{
    Q_UNUSED(payload)

    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    QByteArray passwordHash(20, Qt::Uninitialized);
    int readed = stream.readRawData(passwordHash.data(), passwordHash.size());
    Q_ASSERT(readed = passwordHash.size());

    QString userName = ninjam::extractString(stream);

    QByteArray challenge(8, Qt::Uninitialized);
    readed = stream.readRawData(challenge.data(), challenge.size());
    Q_ASSERT(readed = challenge.size());

    quint32 clientCapabilites;
    quint32 protocolVersion;

    stream >> clientCapabilites;
    stream >> protocolVersion;

    return ClientAuthUserMessage(userName, challenge, protocolVersion, QString(passwordHash));
}

void ClientAuthUserMessage::serializeTo(QIODevice *device) const
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << static_cast<quint8>(msgType);
    stream << payload;

    ninjam::serializeByteArray(passwordHash, stream);
    ninjam::serializeString(userName, stream);
    stream << clientCapabilites;
    stream << protocolVersion;
}

void ClientAuthUserMessage::printDebug(QDebug &dbg) const
{
    dbg << "SEND ClientAuthUserMessage{  userName:" << userName << " challenge:" << challenge <<"}" << endl;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClientSetChannel::ClientSetChannel() :
    ClientMessage(MessageType::ClientSetChannel, 0)
{
    payload = sizeof(quint16);
}

ClientSetChannel::ClientSetChannel(const QStringList &channelsNames) :
    ClientSetChannel()
{
    for (auto channelName : channelsNames)
        addChannel(channelName);
}

void ClientSetChannel::addChannel(const QString &channelName, bool active)
{
    payload += (channelName.toUtf8().size() + 1) + 2 + 1 + 1; // NUL + volume(short) + pan(byte) + flags(byte)

    channels.append(UserChannel(channelName, channels.size(), active));
}

ClientSetChannel ClientSetChannel::unserializeFrom(QIODevice *device, quint32 payload)
{
    if (payload <= 0)
        return ClientSetChannel(); // no channels

    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    quint16 channelParameterSize; // ??
    stream >> channelParameterSize;

    quint32 bytesConsumed = sizeof(quint16);

    ClientSetChannel  msg;

    while (bytesConsumed < payload && !stream.atEnd()) {

        quint16 volume;
        quint8 pan;
        quint8 flags;

        QString channelName = ninjam::extractString(stream);

        stream >> volume;
        stream >> pan;
        stream >> flags;

        bytesConsumed += channelName.toUtf8().size() + 1;
        bytesConsumed += sizeof(volume) + sizeof(pan) + sizeof(flags);

        bool active = flags == 0;

        msg.addChannel(channelName, active);
    }

    return msg;
}

void ClientSetChannel::serializeTo(QIODevice *device) const
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << static_cast<quint8>(msgType);
    stream << payload;
    //++++++++
    stream << quint16(4); // parameter size (4 bytes - volume (2 bytes) + pan (1 byte) + flags (1 byte))
    for (const UserChannel &channel : channels) {
        ninjam::serializeString(channel.getName(), stream);
        stream << channel.getVolume();
        stream << channel.getPan();
        stream << channel.getFlags();
    }
}

void ClientSetChannel::printDebug(QDebug &dbg) const
{
    dbg << "SEND ClientSetChannel{ payloadLenght="
        << payload
        << " channels="
        << channels.size()
        << '}'
        << endl;
}

//+++++++++++++++++++++

ClientKeepAlive::ClientKeepAlive() :
    ClientMessage(MessageType::KeepAlive, 0)
{

}

void ClientKeepAlive::serializeTo(QIODevice *device) const
{
    // just the header bytes, no payload
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << static_cast<quint8>(msgType);
    stream << payload;
}

void ClientKeepAlive::printDebug(QDebug &dbg) const
{
    dbg << "SEND {Client KeepAlive}"
        << endl;
}

//+++++++++++++++++

ClientSetUserMask::ClientSetUserMask(const QString &userName, quint32 channelsMask) :
    ClientMessage(MessageType::ClientSetUserMask, 0),
    userName(userName),
    channelsMask(channelsMask)
{
    payload = 4; // 4 bytes (int) flag
    payload += userName.size() + 1;
}

ClientSetUserMask ClientSetUserMask::from(QIODevice *device, quint32 payload)
{
    Q_UNUSED(payload)

    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    QString userName(ninjam::extractString(stream));
    quint32 channelsMask;

    stream >> channelsMask;

    return ClientSetUserMask(userName, channelsMask);
}

void ClientSetUserMask::serializeTo(QIODevice *device) const
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << static_cast<quint8>(msgType);
    stream << payload;

    //++++++++++++  END HEADER ++++++++++++

    ninjam::serializeString(userName, stream);
    stream << channelsMask;
}

void ClientSetUserMask::printDebug(QDebug &dbg) const
{
    dbg << "SEND ClientSetUserMask{ userName="
        << userName
        << " flag="
        << channelsMask
        << '}';
}

//+++++++++++++++++++++++++++++
ClientToServerChatMessage::ClientToServerChatMessage(const QString &command, const QString &arg1, const QString &arg2, const QString &arg3, const QString &arg4) :
    ClientMessage(MessageType::ChatMessage, 0),
    command(command)
{
    arguments.append(arg1);
    arguments.append(arg2);
    arguments.append(arg3);
    arguments.append(arg4);

    Q_ASSERT(arguments.size() == 4);
}

ClientToServerChatMessage ClientToServerChatMessage::buildPublicMessage(const QString &message)
{
    return ClientToServerChatMessage("MSG", message, QString(), QString(), QString());
}

ClientToServerChatMessage ClientToServerChatMessage::buildPrivateMessage(const QString &message, const QString &destionationUserName)
{
    QString sanitizedMessage(message);
    sanitizedMessage.replace(QString("/msg "), "");

    return ClientToServerChatMessage("PRIVMSG", destionationUserName, sanitizedMessage, QString(), QString());
}

ClientToServerChatMessage ClientToServerChatMessage::buildAdminMessage(const QString &message)
{
    QString msg = message.right(message.size() - 1); // remove the first char (/)
    return ClientToServerChatMessage("ADMIN", msg, QString(), QString(), QString());
}

bool ClientToServerChatMessage::isBpiVoteMessage() const
{
    if (!isPublicMessage())
        return false;

    QString voteCommand = arguments.at(0);
    return voteCommand.startsWith("!vote bpi ");
}

quint16 ClientToServerChatMessage::extractVoteValue(const QString &string)
{
    auto parts = string.split(" ");
    if (parts.size() == 3) {
        return static_cast<quint16>(parts.at(2).toInt());
    }

    return 0;
}


quint16 ClientToServerChatMessage::extractBpmVoteValue() const
{
    if (isBpmVoteMessage()) {
        if (!arguments.empty()) {
            return extractVoteValue(arguments.at(0));
        }
    }

    return 0;
}

quint16 ClientToServerChatMessage::extractBpiVoteValue() const
{
    if (isBpiVoteMessage()) {
        if (!arguments.empty()) {
            return extractVoteValue(arguments.at(0));
        }
    }

    return 0;
}

bool ClientToServerChatMessage::isBpmVoteMessage() const
{
    if (!isPublicMessage())
        return false;

    QString voteCommand = arguments.at(0);
    return voteCommand.startsWith("!vote bpm ");
}

bool ClientToServerChatMessage::isPublicMessage() const
{
    return command == "MSG";
}

bool ClientToServerChatMessage::isPrivateMessage() const
{
    return command == "PRIVMSG";
}

bool ClientToServerChatMessage::isAdminMessage() const
{
    return command == "ADMIN";
}

ClientToServerChatMessage ClientToServerChatMessage::from(QIODevice *device, quint32 payload)
{

    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    QByteArray byteArray(payload, Qt::Uninitialized);

    int readed = stream.readRawData(byteArray.data(), payload);

    Q_ASSERT(readed == payload);

    auto arrays = byteArray.split('\0');

    Q_ASSERT(arrays.size() > 1);

    QString command(arrays.at(0));
    QString arg1 = QString::fromUtf8(arrays.at(1));

    QString arg2 = (arrays.size() >= 3) ? QString::fromUtf8(arrays.at(2)) : QString();
    QString arg3 = (arrays.size() >= 4) ? QString::fromUtf8(arrays.at(3)) : QString();
    QString arg4 = (arrays.size() >= 5) ? QString::fromUtf8(arrays.at(4)) : QString();

    return ClientToServerChatMessage(command, arg1, arg2, arg3,arg4);
}

void ClientToServerChatMessage::serializeTo(QIODevice *device) const
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    quint32 payload = command.size() + 1;
    for (auto &arg : arguments)
        payload += arg.toUtf8().size() + 1;

    stream << static_cast<quint8>(msgType);
    stream << payload;

    ninjam::serializeString(command, stream);

    for (auto &arg : arguments)
        ninjam::serializeString(arg, stream);
}

void ClientToServerChatMessage::printDebug(QDebug &dbg) const
{
    dbg << "SEND ChatMessage{"
        << "command="
        << command
        << " arg1=" << arguments.at(0)
        << " arg2=" << arguments.at(1)
        << " arg3=" << arguments.at(2)
        << " arg4=" << arguments.at(3)
        << '}';
}

//+++++++++++++++++++++++++

UploadIntervalBegin::UploadIntervalBegin(const QByteArray &GUID, quint8 channelIndex, bool isAudioInterval) :
    ClientMessage(MessageType::UploadIntervalBegin, 16 + 4 + 4 + 1),
    GUID(GUID),
    estimatedSize(0),
    channelIndex(channelIndex)
{
    if (isAudioInterval) {
        this->fourCC[0] = 'O';
        this->fourCC[1] = 'G';
        this->fourCC[2] = 'G';
        this->fourCC[3] = 'v';
    }
    else {
        // JamTaba Video prefix
        this->fourCC[0] = 'J';
        this->fourCC[1] = 'T';
        this->fourCC[2] = 'B';
        this->fourCC[3] = 'v';
    }
}

UploadIntervalBegin UploadIntervalBegin::from(QIODevice *device, quint32 payload)
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    QByteArray GUID(16, Qt::Uninitialized);
    quint32 estimatedSize;

    stream.readRawData(GUID.data(), GUID.size());
    stream >> estimatedSize;

    QByteArray fourCC(4, Qt::Uninitialized);
    stream.readRawData(fourCC.data(), fourCC.size());

    quint8 channelIndex;
    stream >> channelIndex;

    // reading and discarding another bytes, old jamtaba versions are wrongly sending user name in this message
    ninjam::extractString(stream, payload - 16 - 4 - 4 - 1);

    bool isAudioInterval = fourCC[0] == 'O' && fourCC[1] == 'G' && fourCC[2] == 'G' && fourCC[3] == 'v';
    return UploadIntervalBegin(GUID, channelIndex, isAudioInterval);
}

void UploadIntervalBegin::serializeTo(QIODevice *device) const
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << static_cast<quint8>(msgType);
    stream << payload;

    stream.writeRawData(GUID.data(), 16);
    stream << estimatedSize;
    stream.writeRawData(fourCC, 4);
    stream << channelIndex;

    //ninjam::serializeString(userName, stream);
}

void UploadIntervalBegin::printDebug(QDebug &dbg) const
{
    dbg << "SEND ClientUploadIntervalBegin{ GUID "
        << QString(GUID)
        << " fourCC"
        << QString(fourCC)
        << "channelIndex: "
        << channelIndex
        << "}";
}

//+++++++++++++++++++++

UploadIntervalWrite::UploadIntervalWrite(const QByteArray &GUID, const QByteArray &encodedData, bool isLastPart) :
    ClientMessage(MessageType::UploadIntervalWrite, 16 + 1 + encodedData.size()),
    GUID(GUID),
    encodedData(encodedData),
    lastPart(isLastPart)
{

}

UploadIntervalWrite UploadIntervalWrite::from(QIODevice *device, quint32 payload)
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    QByteArray GUID(16, Qt::Uninitialized);
    stream.readRawData(GUID.data(), GUID.size());

    quint8 lastPart;
    stream >> lastPart;

    bool isLastPart = lastPart == 1;

    QByteArray encodedData(payload - 16 - 1, Qt::Uninitialized);

    stream.readRawData(encodedData.data(), encodedData.size());

    return UploadIntervalWrite(GUID, encodedData, isLastPart);
}

void UploadIntervalWrite::serializeTo(QIODevice *device) const
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << static_cast<quint8>(msgType);
    stream << payload;

    stream.writeRawData(GUID.data(), 16);
    quint8 intervalCompleted = lastPart ? (quint8) 1 : (quint8) 0; // If the Flag field bit 0 is set then the upload is complete.
    stream << intervalCompleted;
    stream.writeRawData(encodedData.data(), encodedData.size());
}

void UploadIntervalWrite::printDebug(QDebug &dbg) const
{
    dbg << "SEND ClientIntervalUploadWrite{"
        << "GUID="
        << QString(GUID)
        << ", encodedAudioBuffer= "
        << payload
        << " bytes, isLastPart="
        << lastPart
        << '}';
}
