#include "ServerMessageParser.h"
#include "ServerMessages.h"
#include "../User.h"
#include <QDataStream>
#include <QSharedPointer>
#include <QDebug>

using namespace Ninjam;


QString ServerMessageParser::extractString(QDataStream &stream){
    quint8 byte;
    QByteArray byteArray;
    while(!stream.atEnd()){
        stream >> byte;
        if(byte == '\0'){
            break;
        }
        byteArray.append(byte);
    }
    return QString::fromUtf8(byteArray.data(), byteArray.size());
}
//+++++++++++++++++++++++++++++++++++++++++
const ServerMessage& ServerMessageParser::parse(ServerMessageType msgType, QDataStream &stream, quint32 payloadLenght){
    //const ServerMessage* message = nullptr;
    switch (msgType) {
    case ServerMessageType::AUTH_CHALLENGE: return parseAuthChallenge(stream, payloadLenght);
    case ServerMessageType::AUTH_REPLY: return parseAuthReply(stream, payloadLenght);
    case ServerMessageType::USER_INFO_CHANGE_NOTIFY: return parseUserInfoChangeNotify(stream, payloadLenght);
    case ServerMessageType::SERVER_CONFIG_CHANGE_NOTIFY: return parseServerConfigChangeNotify(stream, payloadLenght);
    case ServerMessageType::CHAT_MESSAGE: return parseChatMessage(stream, payloadLenght);
    case ServerMessageType::KEEP_ALIVE: return parseKeepAlive(stream, payloadLenght);
    case ServerMessageType::DOWNLOAD_INTERVAL_BEGIN: return parseDownloadIntervalBegin(stream, payloadLenght);
    case ServerMessageType::DOWNLOAD_INTERVAL_WRITE: return parseDownloadIntervalWrite(stream, payloadLenght);
    }
    throw std::runtime_error("invalid message type!");
}

const ServerMessage& ServerMessageParser::parseAuthChallenge(QDataStream &stream, quint32 /*payloadLenght*/){
    quint8 challenge[8];
    for (int i = 0; i < 8; ++i) {
        stream >> challenge[i];
    }
    quint32 serverCapabilities;
    stream >> serverCapabilities;

    //If the Server Capabilities field has bit 0 set then the License Agreement is present.
    bool serverHasLicenceAgreement = serverCapabilities & 0xFFFFFFFF;

    //The Server Capabilities field bits 8-15 contains the client keepalive interval in seconds. The client sends a Keepalive message if it has sent no messages for the interval.
    quint8 serverKeepAlivePeriod = quint8(serverCapabilities >> 8);

    quint32 protocolVersion;
    stream >> protocolVersion;
    Q_ASSERT(protocolVersion == 0x00020000);

    QString licenceAgreement = "";
    if (serverHasLicenceAgreement) {
        licenceAgreement = ServerMessageParser::extractString(stream);
    }
    static ServerAuthChallengeMessage msg;
    msg.set(serverKeepAlivePeriod, challenge, licenceAgreement, protocolVersion);
    return msg;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
const ServerMessage& ServerMessageParser::parseAuthReply(QDataStream &stream, quint32 /*payloadLenght*/)
{
    quint8 flag;
    quint8 maxChannels;
    stream >> flag;

    QString serverMessage = ServerMessageParser::extractString(stream);
    //acho que o extractString não movimenta o cursos interno do stream, por isso não está lendo o maxChannels corretamente
    stream >> maxChannels;
    static ServerAuthReplyMessage msg;
    msg.set(flag, maxChannels, serverMessage);
    return msg;
}
//++++++++++++++++++++++++++++++++++++++=
const ServerMessage& ServerMessageParser::parseServerConfigChangeNotify(QDataStream &stream, quint32 /*payloadLenght*/){
    quint16 bpm;
    quint16 bpi;
    stream >> bpm;
    stream >> bpi;
    static ServerConfigChangeNotifyMessage msg;
    msg.set(bpm, bpi);
    return msg;
}


const ServerMessage& ServerMessageParser::parseUserInfoChangeNotify(QDataStream &stream, quint32 payloadLenght)
{
    static UserInfoChangeNotifyMessage msg;
    if (payloadLenght <= 0) {//no users
        msg.set(QMap<QString, QList<UserChannel>>());//empy user list
        return  msg;
    }
    QMap<QString, QList<UserChannel>> allUsersChannels;
    unsigned int bytesConsumed = 0;
    while (bytesConsumed < payloadLenght) {
        quint8 active;
        quint8 channelIndex;
        quint16 volume;
        quint8 pan;
        quint8 flags;
        stream >> active >> channelIndex >> volume >> pan >> flags;
        bytesConsumed += 6;
        QString userFullName = ServerMessageParser::extractString(stream);
        bytesConsumed += userFullName.size() + 1;
        QList<UserChannel> & userChannels = allUsersChannels[userFullName];//use the reference to change the same instance inside of QMap
        QString channelName = ServerMessageParser::extractString(stream);
        bytesConsumed += channelName.size() + 1;
        //qDebug() << userFullName << "active:" << active << "volume:" << volume << "pan:" << pan << "flags: "<< flags;
        userChannels.append(UserChannel(userFullName, channelName, (bool)active, channelIndex, volume, pan, flags));
    }

    msg.set(allUsersChannels);
    return msg;
}

//+++++++++++++++++++
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

int getStringSize(char* data, int maxLenght){
    int p = 0;
    for (; p < maxLenght-1; ++p) {
        if(data[p] == '\0'){
            break;
        }
    }
    return p + 1;
}

const ServerMessage& ServerMessageParser::parseChatMessage(QDataStream &stream, quint32 payloadLenght){
    char data[payloadLenght];
    stream.readRawData(data, payloadLenght);
    quint32 consumedBytes = 0;

    int commandStringSize = getStringSize(data, payloadLenght);
    QString command = QString::fromLatin1(data, commandStringSize-1);//remove the NULL terminator (2 bytes - utf-8)
    consumedBytes += commandStringSize;


    int parsedArgs = 0;
    QStringList arguments;
    while(consumedBytes < payloadLenght && parsedArgs < 4){
        int argStringSize = getStringSize(data + consumedBytes, payloadLenght - consumedBytes);
        if(parsedArgs > 0){
            for (int c = 0; c < argStringSize-1; ++c) {
                qWarning() << "'" << data[consumedBytes + c] << "'";
            }
        }
        QString arg = QString::fromLatin1(data + consumedBytes, argStringSize-1);
        arguments.append(arg);
        consumedBytes += argStringSize;
        parsedArgs++;
    }

    static ServerChatMessage msg;
    msg.set(command, arguments);
    return msg;
}

//+++++++++++++++++++++++++++=

const ServerMessage& ServerMessageParser::parseKeepAlive(QDataStream &/*stream*/, quint32 /*payloadLenght*/)
{
    //tinha um bug aqui. Eu estava retornando uma instância estática da mensagem, só que o método
    //que chama o parse deleta a mensagem depois de processá-la. Ou seja, a instância estática era
    //destruída e dava um SIGSEV logo em seguida.

    static  ServerKeepAliveMessage msg;
    return msg;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++

const ServerMessage& ServerMessageParser::parseDownloadIntervalBegin(QDataStream &stream, quint32 /*payload*/)
{
    QByteArray GUID;
    quint8 byte;
    for (int i = 0; i < 16; ++i) {
        stream >> byte;//será que é suficiente?
        GUID.append(byte);
    }
    quint32 estimatedSize;
    stream >> estimatedSize;
    quint8 fourCC[4];
    for (int i = 0; i < 4; ++i) {
        stream >> fourCC[i];
    }
    quint8 channelIndex;
    stream >> channelIndex;
    QString userName = ServerMessageParser::extractString(stream);

    static DownloadIntervalBegin msg;
    msg.set(estimatedSize, channelIndex, userName, fourCC, GUID);
    return msg;
}


const ServerMessage &ServerMessageParser::parseDownloadIntervalWrite(QDataStream &stream, quint32 payloadLenght)
{
    QByteArray GUID;
    quint8 byte;
    for (int i = 0; i < 16; ++i) {
        stream >> byte;//será que é suficiente?
        GUID.append(byte);
    }
    quint8 flags;
    stream >> flags;

    quint32 lenght = payloadLenght - 17;
    QByteArray encodedData;
    encodedData.resize(lenght);
    int bytesReaded = stream.readRawData(encodedData.data(), lenght);
    if(bytesReaded <= 0){
        qWarning() << "ERRO na leitura do audio codificado! "  << bytesReaded;
    }
    static DownloadIntervalWrite msg;
    msg.set(GUID, flags, encodedData);
    return msg;
}
//++++++++++++++++++++++++++++++++++++++
