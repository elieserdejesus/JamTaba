#pragma once
#include <QString>
#include "ServerMessageParser.h"
#include <QDebug>
#include "../User.h"

namespace Ninjam {

enum class ServerMessageType : std::uint8_t{
    AUTH_CHALLENGE = 0x00,
    AUTH_REPLY = 0x01,
    CONFIG_CHANGE_NOTIFY = 0x02,
    USER_INFO_CHANGE_NOTIFY = 0x03,
    DOWNLOAD_INTERVAL_BEGIN = 0x04,
    DOWNLOAD_INTERVAL_WRITE = 0x05,
    KEEP_ALIVE = 0xfd,//server requesting a keepalive
    CHAT_MESSAGE= 0xc0
};

class ServerMessage {

public:
    ServerMessage(ServerMessageType messageType);
    virtual ~ServerMessage();
    virtual void printDebug(QDebug dbg) const = 0;
    inline ServerMessageType getMessageType() const { return messageType;}

private:
    const ServerMessageType messageType;

};
//++++++++++++++++++++++++++++++++++

class ServerAuthChallengeMessage : public ServerMessage {

    quint8 challenge[8];// = new byte[8];
    QString licenceAgreement;
    int serverKeepAlivePeriod;
    int protocolVersion;//The Protocol Version field should contain 0x00020000.

public:
    ServerAuthChallengeMessage();
    void set(int serverKeepAlivePeriod, quint8 challenge[], QString licenceAgreement, int protocolVersion );
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
    quint8 flag;
    QString message;
    quint8 maxChannels;

public:
    ServerAuthReplyMessage();
    void set(quint8 flag, quint8 maxChannels, QString responseMessage) ;
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
    quint16 bpm;
    quint16 bpi;

public:
    ConfigChangeNotifyMessage();
    void set(quint16 bpm, quint16 bpi) ;
    inline quint16 getBpi() const { return bpi; }
    virtual void printDebug(QDebug dbg) const;
    inline quint16 getBpm() const {return bpm;}

};
//++++++++++++++
class User;
class UserChannel;

class UserInfoChangeNotifyMessage : public ServerMessage{
private:
    QMap<QString, QList<UserChannel>> usersChannels;

public:
    //~UserInfoChangeNotifyMessage();
    UserInfoChangeNotifyMessage();
    void set(QMap<QString, QList<UserChannel> > allUsersChannels) ;

    inline QList<QString> getUsersNames() const{ return usersChannels.keys();}
    QList<UserChannel> getUserChannels(QString userFullName) const {return usersChannels[userFullName];}
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

enum class  ChatCommandType : std::uint8_t { MSG=0, PRIVMSG, TOPIC, JOIN, PART, USERCOUNT};

class ServerChatMessage : public ServerMessage {

private:
    ChatCommandType commandType;
    QStringList arguments;

    virtual void printDebug(QDebug dbg) const;
    ChatCommandType commandTypeFromString(QString string);
public:
    ServerChatMessage();
    void set(QString command, QStringList arguments);

    inline QList<QString> getArguments() const { return arguments;}

    inline ChatCommandType getCommand() const {return commandType;}

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
    QByteArray GUID;
    quint32 estimatedSize;
    quint8 fourCC[4];// = new byte[4];
    quint8 channelIndex;
    QString userName;
    bool isValidOgg;

public:
    DownloadIntervalBegin();
    void set(quint32 estimatedSize, quint8 channelIndex, QString userName, quint8 fourCC[], QByteArray GUID) ;

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
    QByteArray GUID;
    quint8 flags;
    QByteArray encodedAudioData;

public:
    virtual void printDebug(QDebug dbg) const;
    DownloadIntervalWrite();
    void set(QByteArray GUID, quint8 flags, QByteArray encodedAudioData) ;

    inline QString getGUID() const{ return GUID; }
    inline QByteArray getEncodedAudioData() const { return encodedAudioData;}
    inline bool downloadIsComplete() const { return flags == 1;}

};


//++++++++++++++++++++
QDebug operator<<(QDebug dbg, const Ninjam::ServerMessage& c);
//QDataStream &operator<<(QDataStream &out, const ServerAuthChallengeMessage &message);

}


