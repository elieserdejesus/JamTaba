#pragma once

#include <QDataStream>
#include <QByteArray>
#include <QBuffer>
#include <QMap>
#include <memory>


class ServerMessage;

namespace ServerMessageType{
    enum MessageType {
        AUTH_CHALLENGE = 0x00,
        AUTH_REPLY = 0x01,
        CONFIG_CHANGE_NOTIFY = 0x02,
        USER_INFO_CHANGE_NOTIFY = 0x03,
        DOWNLOAD_INTERVAL_BEGIN = 0x04,
        DOWNLOAD_INTERVAL_WRITE = 0x05,
        KEEP_ALIVE = 0xfd,//server requesting a keepalive
        CHAT_MESSAGE= 0xc0
    };
}

class ServerMessageParser  {

private:
    static QMap<ServerMessageType::MessageType, std::shared_ptr<ServerMessageParser>> parsers;

    static ServerMessageParser* createInstance(ServerMessageType::MessageType messageType);

protected:
    static QString extractString(QDataStream& buffer) {
        return QString(buffer.device()->readLine());
        qCritical("\\0 is not finded in QByteArray buffer!");
        return "";
    }

public:
    virtual ServerMessage* parse(QDataStream& buffer, int payloadLenght) = 0;

    static ServerMessageParser* getParser(ServerMessageType::MessageType messageType) ;
};

//+++++++++++++++++++++++++++++++++++++++++++++++
class AuthChallengeParser : public ServerMessageParser{


public:
    virtual ServerMessage* parse(QDataStream& buffer, int payloadLenght) ;

};
//++++++++++++++++++++++++++++++++++++++++++++++++++
