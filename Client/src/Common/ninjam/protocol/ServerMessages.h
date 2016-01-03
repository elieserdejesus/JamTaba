#ifndef SERVER_MESSAGES_H
#define SERVER_MESSAGES_H

#include <QMap>
class QStringList;
class QtGlobal;

/**
 * All details about ninjam protocol are based on the Stefanha documentation work in wahjam.
 */

namespace Ninjam {


class Service;
class ServerMessageVisitor;

QString extractString(QDataStream &stream); // ninjam strings are NUL(\0) terminated

enum class ServerMessageType : quint8 {
    AUTH_CHALLENGE = 0x00, // received after connect in server
    AUTH_REPLY = 0x01, // received after respond to auth challenge
    SERVER_CONFIG_CHANGE_NOTIFY = 0x02, // received when BPI or BPM change
    USER_INFO_CHANGE_NOTIFY = 0x03,// received when user add/remove channels or rename a channel
    DOWNLOAD_INTERVAL_BEGIN = 0x04, // received when server is notifiyng about the start of a new audio interval stream
    DOWNLOAD_INTERVAL_WRITE = 0x05, // received for every audio interval chunk. We receive a lot of these messages while jamming.
    KEEP_ALIVE = 0xfd,// server requesting a keepalive. If Jamtaba not respond the server will disconnect.
    CHAT_MESSAGE = 0xc0 // received when users are sending chat messages
};

class ServerMessage
{
    friend QDebug &operator<<(QDebug &dbg, const ServerMessage &message);
    friend QDataStream &operator >>(QDataStream &stream, ServerMessage &message);

public:
    explicit ServerMessage(ServerMessageType messageType, quint32 payload);
    virtual ~ServerMessage();

    inline ServerMessageType getMessageType() const
    {
        return messageType;
    }

protected:
    quint32 payload;

private:
    const ServerMessageType messageType;

    // used by overloaded operators only
    virtual void readFrom(QDataStream &stream) = 0;
    virtual void printDebug(QDebug &dbg) const = 0;
};

// ++++++++++++++++++++++++++++++++++

class ServerAuthChallengeMessage : public ServerMessage
{

public:
    ServerAuthChallengeMessage(quint32 payload);

    inline QByteArray getChallenge() const
    {
        return challenge;
    }

    inline int getProtocolVersion() const
    {
        return protocolVersion;
    }

    inline int getServerKeepAlivePeriod() const
    {
        return serverKeepAlivePeriod;
    }

    inline bool serverHasLicenceAgreement() const
    {
        return !licenceAgreement.isNull() && !licenceAgreement.isEmpty();
    }

    inline QString getLicenceAgreement() const
    {
        return licenceAgreement;
    }

private:
    QByteArray challenge;
    QString licenceAgreement;
    int serverKeepAlivePeriod;
    int protocolVersion;// The Protocol Version field should contain 0x00020000.
    void printDebug(QDebug &dbg) const override;

    void readFrom(QDataStream &stream) override;

};
// ++++++++++++++++++++++++++++++++
class ServerAuthReplyMessage : public ServerMessage
{

public:
    ServerAuthReplyMessage(quint32 payload);

    inline QString getErrorMessage() const
    {
        return message;
    }

    inline QString getNewUserName() const
    {
        if (!userIsAuthenticated())
            qCritical("user is note authenticated!");
        return message;
    }

    inline bool userIsAuthenticated() const
    {
        return flag == 1;
    }

    inline quint8 getMaxChannels() const
    {
        return maxChannels;
    }

private:
    quint8 flag;
    QString message;
    quint8 maxChannels;

    void printDebug(QDebug &debug) const override;
    void readFrom(QDataStream &stream) override;
};
// +++++++++++++++++++++++++++++++
class ServerKeepAliveMessage : public ServerMessage
{
public:
    ServerKeepAliveMessage(quint32 payload);

private:
    void printDebug(QDebug &dbg) const override;
    void readFrom(QDataStream &stream) override;
};
// ++++++++++++++++++++++++=
class ServerConfigChangeNotifyMessage : public ServerMessage
{
private:
    quint16 bpm;
    quint16 bpi;

public:
    ServerConfigChangeNotifyMessage(quint32 payload);

    inline quint16 getBpi() const
    {
        return bpi;
    }

    inline quint16 getBpm() const
    {
        return bpm;
    }

private:
    void readFrom(QDataStream &stream) override;
    void printDebug(QDebug &dbg) const override;
};
// ++++++++++++++
class User;
class UserChannel;

class UserInfoChangeNotifyMessage : public ServerMessage
{
public:
    UserInfoChangeNotifyMessage(quint32 payload);
    ~UserInfoChangeNotifyMessage();

    inline QList<QString> getUsersNames() const
    {
        return usersChannels.keys();
    }

    QList<UserChannel> getUserChannels(const QString &userFullName) const
    {
        return usersChannels[userFullName];
    }

private:
    QMap<QString, QList<UserChannel> > usersChannels;

    void readFrom(QDataStream &stream) override;
    void printDebug(QDebug &dbg) const override;
};
// ++++++++++++=

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

enum class  ChatCommandType : quint8 {
    MSG = 0, PRIVMSG, TOPIC, JOIN, PART, USERCOUNT
    // TODO remove this 0 value. Is uncessary, right?
};

class ServerChatMessage : public ServerMessage
{
public:
    ServerChatMessage(quint32 payload);

    inline QList<QString> getArguments() const
    {
        return arguments;
    }

    inline ChatCommandType getCommand() const
    {
        return commandType;
    }

private:
    ChatCommandType commandType;
    QStringList arguments;

    void printDebug(QDebug &dbg) const override;
    void readFrom(QDataStream &stream) override;
    ChatCommandType commandTypeFromString(const QString &string);

};
// ++++++++++++++++
// ++++++++++++++++
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
class DownloadIntervalBegin : public ServerMessage
{

public:
    DownloadIntervalBegin(quint32 payload);

    inline quint8  getChannelIndex() const
    {
        return channelIndex;
    }

    inline quint32 getEstimatedSize() const
    {
        return estimatedSize;
    }

    inline QByteArray getGUID() const
    {
        return GUID;
    }

    inline QString getUserName() const
    {
        return userName;
    }

    inline bool isValidOggDownload() const
    {
        return isValidOgg;
    }

    inline bool downloadShouldBeStopped() const
    {
        return GUID.at(0) == '0' && GUID.at(GUID.size()-1) == '0';
    }

    inline bool downloadIsComplete() const
    {
        return fourCC[0] == 0 && fourCC[3] == 0;
    }

private:
    QByteArray GUID;
    quint32 estimatedSize;
    quint8 fourCC[4];// = new byte[4];
    quint8 channelIndex;
    QString userName;
    bool isValidOgg;

    void readFrom(QDataStream &stream) override;
    void printDebug(QDebug &dbg) const override;
};
// ++++++++++++++++++
/*
  Offset Type        Field
  0x0    uint8_t[16] GUID (binary)
  0x10   uint8_t     Flags
  0x11   ...         Audio Data
  If the Flags field has bit 0 set then this download should be aborted.
  */
class DownloadIntervalWrite : public ServerMessage
{

public:
    DownloadIntervalWrite(quint32 payload);

    inline QByteArray getGUID() const
    {
        return GUID;
    }

    inline QByteArray getEncodedAudioData() const
    {
        return encodedAudioData;
    }

    inline bool downloadIsComplete() const
    {
        return flags == 1;
    }

private:
    QByteArray GUID;
    quint8 flags;
    QByteArray encodedAudioData;

    void readFrom(QDataStream &stream) override;
    void printDebug(QDebug &dbg) const override;
};

// ++++++++++++++++++++
QDebug operator<<(QDebug dbg, const Ninjam::ServerMessage &message);

QDataStream &operator >>(QDataStream &stream, ServerMessage &message);

}

#endif
