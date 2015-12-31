#ifndef SERVER_MESSAGES_PARSER_H
#define SERVER_MESSAGES_PARSER_H

#include <QByteArray>
#include "ServerMessages.h"
#include <cstdint>

class QDataStream;

namespace Ninjam {
class ServerMessage;
enum class ServerMessageType : std::uint8_t;

class ServerMessageParser
{
protected:
    static QString extractString(QDataStream &stream);

public:
    static const ServerMessage &parse(ServerMessageType msgType, QDataStream &stream,
                                      quint32 payloadLenght);

private:

    // TODO remove these static instances
    static const ServerMessage &parseAuthChallenge(QDataStream &stream, quint32 payloadLenght);
    static const ServerMessage &parseAuthReply(QDataStream &stream, quint32 payloadLenght);
    static const ServerMessage &parseServerConfigChangeNotify(QDataStream &stream,
                                                              quint32 payloadLenght);
    static const ServerMessage &parseUserInfoChangeNotify(QDataStream &stream,
                                                          quint32 payloadLenght);
    static const ServerMessage &parseChatMessage(QDataStream &stream, const quint32 payloadLenght);
    static const ServerMessage &parseKeepAlive(QDataStream &stream, quint32 payloadLenght);
    static const ServerMessage &parseDownloadIntervalBegin(QDataStream &stream,
                                                           quint32 payload);
    static const ServerMessage &parseDownloadIntervalWrite(QDataStream &stream,
                                                           quint32 payloadLenght);
};
}

#endif
