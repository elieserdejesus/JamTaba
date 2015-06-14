#include "ServerMessageParser.h"
#include "ServerMessages.h"
#include "../User.h"

using namespace Ninjam;

QMap<ServerMessageType, std::shared_ptr<ServerMessageParser>> ServerMessageParser::parsers;

ServerMessageParser* ServerMessageParser::getParser(ServerMessageType type)
{
    if(!parsers.contains(type)){
        //parsers.emplace(std::make_pair(type,std::move(createInstance(type))));
        parsers[type] = std::shared_ptr<ServerMessageParser>( createInstance(type));
    }
    return parsers[type].get();
}

ServerMessageParser* ServerMessageParser::createInstance(ServerMessageType messageType){
    switch (messageType) {
    case ServerMessageType::AUTH_CHALLENGE: return new AuthChallengeParser();
    case ServerMessageType::AUTH_REPLY: return new AuthReplyParser();
    case ServerMessageType::USER_INFO_CHANGE_NOTIFY: return new UserInfoChangeNotifyParser();
    case ServerMessageType::CONFIG_CHANGE_NOTIFY: return new ConfigChangeNotifyParser();
    case ServerMessageType::CHAT_MESSAGE: return new ChatMessageParser();
    case ServerMessageType::KEEP_ALIVE: return new KeepAliveParser();
    case ServerMessageType::DOWNLOAD_INTERVAL_BEGIN: return new DownloadIntervalBeginParser();
    case ServerMessageType::DOWNLOAD_INTERVAL_WRITE: return new DownloadIntervalWriteParser();
    }
    qFatal("Parser not implemented for " + (unsigned char)messageType);
    return nullptr;
}

QString ServerMessageParser::extractString(QDataStream &stream)
{
    QString str;
    quint8 byte;
    while(!stream.atEnd()){
        stream >> byte;
        if(byte != 0x00){
            str.append(byte);
        }
        else{
            break;
        }
    }
    //qDebug() << "extraiu " << str;
    return str;
}

//+++++++++++++++++++++++++++++++++++++++++

ServerMessage* AuthChallengeParser::parse(QDataStream &stream, quint32 /*payloadLenght*/){
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
    return new ServerAuthChallengeMessage(serverKeepAlivePeriod, challenge, licenceAgreement, protocolVersion);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
ServerMessage *AuthReplyParser::parse(QDataStream &stream, quint32 /*payloadLenght*/)
{
    quint8 flag;
    quint8 maxChannels;
    stream >> flag;

    QString serverMessage = ServerMessageParser::extractString(stream);
    //acho que o extractString não movimenta o cursos interno do stream, por isso não está lendo o maxChannels corretamente
    stream >> maxChannels;
    return new ServerAuthReplyMessage(flag, maxChannels, serverMessage);
}
//++++++++++++++++++++++++++++++++++++++=
ServerMessage* ConfigChangeNotifyParser::parse(QDataStream &stream, quint32 /*payloadLenght*/){
    quint16 bpm;
    quint16 bpi;
    stream >> bpm;
    stream >> bpi;
    return new ConfigChangeNotifyMessage(bpm, bpi);

}


ServerMessage *UserInfoChangeNotifyParser::parse(QDataStream &stream, quint32 payloadLenght)
{
    if (payloadLenght <= 0) {//no users
        return new UserInfoChangeNotifyMessage();
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
        //QMap creates a empty object when the key is not found in map
        //User user = User::getUser(userFullName);
        QList<UserChannel> & userChannels = allUsersChannels[userFullName];//use the reference to change the same instance inside of QMap
        QString channelName = ServerMessageParser::extractString(stream);
        bytesConsumed += channelName.size() + 1;
        userChannels.append(UserChannel(userFullName, channelName, (bool)active, channelIndex, volume, pan, flags));
    }
    return new UserInfoChangeNotifyMessage(allUsersChannels);
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
ServerMessage* ChatMessageParser::parse(QDataStream &stream, quint32 payloadLenght){
    quint32 consumedBytes = 0;
    QString command = ServerMessageParser::extractString(stream);
    consumedBytes += command.size() + 1;
    QStringList arguments;
    int parsedArgs = 0;
    while(consumedBytes < payloadLenght && parsedArgs < 4){
        QString arg = ServerMessageParser::extractString(stream);
        arguments.append(arg);
        parsedArgs++;
        consumedBytes += arg.size() + 1;
    }
    Q_ASSERT(consumedBytes == payloadLenght);
    return new ServerChatMessage(command, arguments);
}

//+++++++++++++++++++++++++++=

ServerMessage *KeepAliveParser::parse(QDataStream &/*stream*/, quint32 /*payloadLenght*/)
{
    //tinha um bug aqui. Eu estava retornando uma instância estática da mensagem, só que o método
    //que chama o parse deleta a mensagem depois de processá-la. Ou seja, a instância estática era
    //destruída e dava um SIGSEV logo em seguida.
    return new ServerKeepAliveMessage();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++

ServerMessage *DownloadIntervalBeginParser::parse(QDataStream &stream, quint32 /*payload*/)
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
    return new DownloadIntervalBegin(estimatedSize, channelIndex, userName, fourCC, GUID);
}


ServerMessage *DownloadIntervalWriteParser::parse(QDataStream &stream, quint32 payloadLenght)
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
    return new DownloadIntervalWrite(GUID, flags, encodedData);
}
//++++++++++++++++++++++++++++++++++++++
