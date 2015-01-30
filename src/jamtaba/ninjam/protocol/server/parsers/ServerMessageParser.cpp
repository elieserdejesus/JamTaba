#include "ServerMessageParser.h"
#include "../../ServerMessages.h"

QMap<ServerMessageType::MessageType, std::shared_ptr<ServerMessageParser>> ServerMessageParser::parsers;

ServerMessageParser *ServerMessageParser::getParser(ServerMessageType::MessageType type)
{
    if(!parsers.contains(type)){
        parsers.insert(type, std::shared_ptr<ServerMessageParser>(createInstance(type) ));
    }
    return parsers[type].get();
}

ServerMessageParser* ServerMessageParser::createInstance(ServerMessageType::MessageType messageType){
    switch (messageType) {
    case ServerMessageType::AUTH_CHALLENGE: return new AuthChallengeParser();
    case ServerMessageType::AUTH_REPLY: return new AuthReplyParser();
        //case USER_INFO_CHANGE_NOTIFY: return new UserInfoChangeNotifyParser();
        //case CONFIG_CHANGE_NOTIFY: return new ConfigChangeNotifyParser();
        //case CHAT_MESSAGE: return new ChatMessageParser();
        //case KEEP_ALIVE: return new KeepAliveMessageParser();
        //case DOWNLOAD_INTERVAL_BEGIN: return new DownloadIntervalBeginParser();
        //case DOWNLOAD_INTERVAL_WRITE: return new DownloadIntervalWriteParser();
    }
    qCritical("Parser not implemented for " + messageType);
    return nullptr;
}

//+++++++++++++++++++++++++++++++++++++++++

ServerMessage* AuthChallengeParser::parse(QDataStream &stream, int /*payloadLenght*/){
        quint8 challenge[8];// = new char[8];
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



ServerMessage *AuthReplyParser::parse(QDataStream &stream, int payloadLenght)
{
    quint8 flag, maxChannels;
    stream >> flag;

    QString serverMessage = ServerMessageParser::extractString(stream);

    stream >> maxChannels;
    return new ServerAuthReplyMessage(flag, maxChannels, serverMessage);
}
