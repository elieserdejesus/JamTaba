#pragma once

#include <QDataStream>
#include <QByteArray>
#include <QDataStream>
#include <QMap>
#include <memory>

namespace Ninjam {



class ServerMessage;
enum class ServerMessageType : std::uint8_t;

template<type T>
class ServerMessageParser  {

protected:
    static QString extractString(QDataStream& stream) ;

public:
    virtual const ServerMessage& parse(QDataStream& stream, quint32 payloadLenght) = 0;
    static const ServerMessageParser& getParser(ServerMessageType messageType) ;
};

//+++++++++++++++++++++++++++++++++++++++++++++++
class AuthChallengeParser : public ServerMessageParser<AuthChallengeParser>{
public:
    virtual const AuthChallengeParser parse(QDataStream& stream, quint32 payloadLenght) ;

};
//++++++++++++++++++++++++++++++++++++++++++++++++++
class AuthReplyParser : public ServerMessageParser<>{

public:
    virtual const ServerMessage& parse(QDataStream &stream, quint32 payloadLenght);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++
class ServerKeepAliveMessage;
class KeepAliveParser : public ServerMessageParser{

public:
    virtual const ServerMessage& parse(QDataStream &stream, quint32 payloadLenght);
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
class ConfigChangeNotifyParser : public ServerMessageParser{

public:
    virtual const ServerMessage& parse(QDataStream &stream, quint32 payloadLenght);
};
//++++++++++++++++++++++
class UserInfoChangeNotifyParser : public ServerMessageParser {

public:
    virtual const ServerMessage& parse(QDataStream &stream, quint32 payloadLenght);

};
//+++++++++++++++++++=

class ChatMessageParser : public ServerMessageParser {

    virtual const ServerMessage& parse(QDataStream &stream, quint32 payloadLenght);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
 Offset Type        Field
 0x0    uint8_t[16] GUID (binary)
 0x10   uint32_t    Estimated Size
 0x14   uint8_t[4]  FourCC
 0x18   uint8_t     Channel Index
 0x19   ...         Username (NUL-terminated)
 */
class DownloadIntervalBeginParser : public ServerMessageParser {

    virtual const ServerMessage& parse(QDataStream& stream, quint32 payload);

};

//++++++++++++++++++++
/*
 Offset Type        Field
 0x0    uint8_t[16] GUID (binary)
 0x10   uint8_t     Flags
 0x11   ...         Audio Data
 */
class DownloadIntervalWriteParser : public ServerMessageParser {

    //max payload recebido do reaper foi 2076

public:
    virtual const ServerMessage& parse(QDataStream& stream, quint32 payloadLenght);

};

}
