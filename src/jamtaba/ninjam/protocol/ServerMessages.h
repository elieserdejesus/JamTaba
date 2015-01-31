#pragma once
#include <QString>
#include "server/parsers/ServerMessageParser.h"
#include <QDebug>

class ServerMessage {

    const ServerMessageType::MessageType messageType;
public:
    ServerMessage(ServerMessageType::MessageType messageType);
    virtual ~ServerMessage();
    virtual void printDebug(QDebug dbg) const = 0;
    inline ServerMessageType::MessageType getMessageType() const { return messageType;}
};
//++++++++++++++++++++++++++++++++++

class ServerAuthChallengeMessage : public ServerMessage {

    quint8 challenge[];// = new byte[8];
    const QString licenceAgreement;
    const int serverKeepAlivePeriod;
    const int protocolVersion;//The Protocol Version field should contain 0x00020000.

public:
    ServerAuthChallengeMessage(int serverKeepAlivePeriod, quint8 challenge[], QString licenceAgreement, int protocolVersion );

    inline QByteArray getChallenge() const { return QByteArray((const char*)challenge); }
    inline int getProtocolVersion() const{ return protocolVersion; }
    inline int getServerKeepAlivePeriod() const{ return serverKeepAlivePeriod; }
    inline bool serverHasLicenceAgreement() const{ return !licenceAgreement.isNull() && !licenceAgreement.isEmpty();}
    inline QString getLicenceAgreement() const { return licenceAgreement; }
    virtual void printDebug(QDebug dbg) const;
};
//++++++++++++++++++++++++++++++++
class ServerAuthReplyMessage : public ServerMessage{
private:
    const quint8 flag;
    const QString message;
    const quint8 maxChannels;

public:
    ServerAuthReplyMessage(quint8 flag, quint8 maxChannels, QString responseMessage) ;
    virtual void printDebug(QDebug debug) const;
    inline QString getErrorMessage() const { return message;}

    inline QString getNewUserName() const{
        if(!userIsAuthenticated()){
            qCritical("user is note authenticated!");
        }
        return message;
    }
    inline bool userIsAuthenticated() const{ return flag == 1; }
    inline quint8 getMaxChannels() const{ return maxChannels; }

};
//+++++++++++++++++++++++++++++++
class ServerKeepAliveMessage : public ServerMessage {

public:
    ServerKeepAliveMessage();
    virtual void printDebug(QDebug dbg) const;
};
//++++++++++++++++++++++++=
class ConfigChangeNotifyMessage : public ServerMessage{

private:
    const quint16 bpm;
    const quint16 bpi;

public:
    ConfigChangeNotifyMessage(quint16 bpm, quint16 bpi) ;
    inline quint16 getBpi() const { return bpi; }
    virtual void printDebug(QDebug dbg) const;
    inline quint16 getBpm() const {return bpm;}

};
//++++++++++++++
class NinjamUser;
class NinjamUserChannel;

class UserInfoChangeNotifyMessage : public ServerMessage{
private:
    QMap<NinjamUser*, QList<NinjamUserChannel*>> usersChannels;

public:
    UserInfoChangeNotifyMessage();
    UserInfoChangeNotifyMessage(QMap<NinjamUser *, QList<NinjamUserChannel *> > allUsersChannels) ;

    inline QList<NinjamUser*> getUsers() const{ return usersChannels.keys();}
    inline QMap<NinjamUser*, QList<NinjamUserChannel*>> getUsersChannels() const{ return usersChannels;}
    virtual void printDebug(QDebug dbg) const;
};
//++++++++++++=


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

namespace ServerChatCommand {
enum CommandType{ MSG=0, PRIVMSG, TOPIC, JOIN, PART, USERCOUNT};
}

class ServerChatMessage : public ServerMessage {

private:
    const ServerChatCommand::CommandType commandType;
    QStringList arguments;

    virtual void printDebug(QDebug dbg) const;
    ServerChatCommand::CommandType commandTypeFromString(QString string);
public:

    ServerChatMessage(QString command, QStringList arguments);

    inline QList<QString> getArguments() const { return arguments;}

    inline ServerChatCommand::CommandType getCommand() const {return commandType;}

};
//++++++++++++++++
//++++++++++++++++
//++++++++++++++++
/*
Offset Type        Field
0x0    uint8_t[16] GUID (binary)
0x10   uint32_t    Estimated Size
0x14   uint8_t[4]  FourCC
0x18   uint8_t     Channel Index
0x19   ...         Username (NUL-terminated)
If the GUID field is zero then the download should be stopped.

If the FourCC field is zero then the download is complete.

If the FourCC field contains "OGGv" then this is a valid Ogg Vorbis encoded download.
*/
class DownloadIntervalBegin : public ServerMessage {

private:
    const QByteArray GUID;
    const quint32 estimatedSize;
    quint8 fourCC[];// = new byte[4];
    const quint8 channelIndex;
    const QString userName;
    bool isValidOgg;

public:
    DownloadIntervalBegin(quint32 estimatedSize, quint8 channelIndex, QString userName, quint8 fourCC[], QByteArray GUID) ;

    inline quint8  getChannelIndex() const{ return channelIndex; }
    inline quint32 getEstimatedSize() const { return estimatedSize; }
    inline QString getGUID() const { return GUID; }

    virtual void printDebug(QDebug dbg) const;

    inline QString getUserName() const { return userName; }
    inline bool isValidOggDownload() const{ return isValidOgg;}
    inline bool downloadShouldBeStopped() const{ return   GUID.at(0) == '0' && GUID.at(GUID.size()-1) == '0';}
    inline bool downloadIsComplete() const { return fourCC[0] == 0 && fourCC[3] == 0;}

};
//++++++++++++++++++
/*
  Offset Type        Field
  0x0    uint8_t[16] GUID (binary)
  0x10   uint8_t     Flags
  0x11   ...         Audio Data
  If the Flags field has bit 0 set then this download should be aborted.
  */
class DownloadIntervalWrite : public ServerMessage {

private:
    const QByteArray GUID;
    const quint8 flags;
    const QByteArray encodedAudioData;

public:
    virtual void printDebug(QDebug dbg) const;

    DownloadIntervalWrite(QByteArray GUID, quint8 flags, QByteArray encodedAudioData) ;

    inline QString getGUID() const{ return GUID; }
    inline QByteArray getEncodedAudioData() const { return encodedAudioData;}
    inline bool downloadIsComplete() const { return flags == 1;}

};
//++++++++++++++++++++
QDebug operator<<(QDebug dbg, ServerMessage* c);
//QDataStream &operator<<(QDataStream &out, const ServerAuthChallengeMessage &message);
