#ifndef SERVER_MESSAGES_H
#define SERVER_MESSAGES_H

#include "User.h"
#include "ninjam/Ninjam.h"

#include <QMap>
#include <QtGlobal>
#include <QStringList>
#include <QIODevice>
#include <QDebug>

/**
 * All details about ninjam protocol are based on the Stefanha documentation work in wahjam.
 */

namespace ninjam {
namespace client {

class User;
class UserChannel;
class Service;
class UploadIntervalBegin;
class UploadIntervalWrite;

enum class  ChatCommandType : quint8
{
    MSG,
    PRIVMSG,
    TOPIC,
    JOIN,
    PART,
    USERCOUNT
};

class ServerMessage
{
public:
    explicit ServerMessage(MessageType messageType);
    virtual ~ServerMessage();

    virtual void printDebug(QDebug &dbg) const = 0;

    inline MessageType getMessageType() const
    {
        return messageType;
    }

protected:
    MessageType messageType;
};

// ++++++++++++++++++++++++++++++++++

class AuthChallengeMessage : public ServerMessage
{
public:
    AuthChallengeMessage(const QByteArray &challenge, const QString &licence, quint32 serverCapabilities, quint32 protocolVersion);

    static AuthChallengeMessage from(QIODevice *device, quint32 payload);
    void to(QIODevice *stream) const;

    void printDebug(QDebug &dbg) const override;

    QByteArray getChallenge() const;
    quint32 getProtocolVersion() const;
    quint32 getServerKeepAlivePeriod() const;
    bool serverHasLicenceAgreement() const;
    QString getLicenceAgreement() const;

private:
    QByteArray challenge;
    QString licenceAgreement;
    quint32 serverCapabilities;
    quint32 protocolVersion;         // The Protocol Version field should contain 0x00020000.
};

inline QByteArray AuthChallengeMessage::getChallenge() const
{
    return challenge;
}

inline quint32 AuthChallengeMessage::getProtocolVersion() const
{
    return protocolVersion;
}

inline bool AuthChallengeMessage::serverHasLicenceAgreement() const
{
    return !licenceAgreement.isNull() && !licenceAgreement.isEmpty();
}

inline QString AuthChallengeMessage::getLicenceAgreement() const
{
    return licenceAgreement;
}

// ++++++++++++++++++++++++++++++++

class AuthReplyMessage : public ServerMessage
{
public:
    AuthReplyMessage(quint8 flag, const QString &message, quint8 maxChannels);
    void to(QIODevice *device) const;

    static AuthReplyMessage from(QIODevice *stream, quint32 payload);

    void printDebug(QDebug &debug) const override;

    inline QString getErrorMessage() const
    {
        return message;
    }

    inline QString getNewUserName() const
    {
        if (!userIsAuthenticated())
            qCritical("user is not authenticated!");

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
};

// +++++++++++++++++++++++++++++++

class ServerKeepAliveMessage : public ServerMessage
{
public:
    void printDebug(QDebug &dbg) const override;
    static ServerKeepAliveMessage from(QIODevice *stream, quint32 payload);
private:
    ServerKeepAliveMessage();
};

// -----------------------------------------------------------------

class ConfigChangeNotifyMessage : public ServerMessage
{
public:
    ConfigChangeNotifyMessage(quint16 bpm, quint16 bpi);
    static ConfigChangeNotifyMessage from(QIODevice *device, quint32 payload);
    void to(QIODevice *device) const;
    void printDebug(QDebug &dbg) const override;

    quint16 getBpi() const;
    quint16 getBpm() const;

private:
    quint16 bpm;
    quint16 bpi;
};

inline quint16 ConfigChangeNotifyMessage::getBpi() const
{
    return bpi;
}

inline quint16 ConfigChangeNotifyMessage::getBpm() const
{
    return bpm;
}

// -----------------------------------------------------------------

class UserInfoChangeNotifyMessage : public ServerMessage
{
public:
    UserInfoChangeNotifyMessage();
    ~UserInfoChangeNotifyMessage();
    void addUserChannel(const QString &userFullName, const UserChannel &channel);
    void to(QIODevice *device) const;
    static UserInfoChangeNotifyMessage from(QIODevice *stream, quint32 payload);
    static UserInfoChangeNotifyMessage buildDeactivationMessage(const User &user);

    void printDebug(QDebug &dbg) const override;

    QList<User> getUsers() const;

private:
    QMap<QString, User> users;
    quint32 getPayload() const;
};

inline QList<User> UserInfoChangeNotifyMessage::getUsers() const
{
    return users.values();
}

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

class ServerToClientChatMessage : public ServerMessage
{
public:

    ServerToClientChatMessage(const QString &command, const QString &arg1, const QString &arg2, const QString &arg3 = QString(), const QString &arg4 = QString());

    static ServerToClientChatMessage buildTopicMessage(const QString &topic);
    static ServerToClientChatMessage buildPublicMessage(const QString &authorName, const QString &message);
    static ServerToClientChatMessage buildPrivateMessage(const QString &destinationUserName, const QString &message);
    static ServerToClientChatMessage buildUserJoinMessage(const QString &userName);
    static ServerToClientChatMessage buildUserPartMessage(const QString &userName);
    static ServerToClientChatMessage buildVoteSystemMessage(const QString&message);

    static ServerToClientChatMessage from(QIODevice *stream, quint32 payload);

    void printDebug(QDebug &dbg) const override;

    void to(QIODevice *device) const;

    quint32 getPayload() const;

    inline QStringList getArguments() const
    {
        return arguments;
    }

    inline ChatCommandType getCommand() const
    {
        return commandTypeFromString(command);
    }

    static ChatCommandType commandTypeFromString(const QString &string);

private:
    QString command;
    QStringList arguments;


};

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
    DownloadIntervalBegin(const QByteArray &GUID, quint32 estimatedSize, const QByteArray &fourCC, quint8 channelIndex, const QString &userName);
    virtual ~DownloadIntervalBegin() {}

    static DownloadIntervalBegin from(QIODevice *stream, quint32 payload);
    static DownloadIntervalBegin from(const UploadIntervalBegin &msg, const QString &userName);
    virtual void to(QIODevice *device) const;

    void printDebug(QDebug &dbg) const override;

    inline quint8  getChannelIndex() const
    {
        return channelIndex;
    }

    inline QByteArray getFourCC() const
    {
        return QByteArray(&fourCC[0], 4);
    }

    inline quint32 getEstimatedSize() const
    {
        return estimatedSize;
    }

    inline QByteArray getGUID() const
    {
        return GUID;
    }

    bool isAudio() const;

    bool isVideo() const;

    inline bool shouldBeStopped() const
    {
        return GUID.at(0) == '0' && GUID.at(GUID.size()-1) == '0';
    }

    inline bool isComplete() const
    {
        return fourCC[0] == 0 && fourCC[3] == 0;
    }

    inline QString getUserName() const
    {
        return userName;
    }

private:
    QByteArray GUID;
    quint32 estimatedSize;
    char fourCC[4];
    quint8 channelIndex;
    QString userName;
};

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
    static DownloadIntervalWrite from(QIODevice *stream, quint32 payload);
    static DownloadIntervalWrite from(const UploadIntervalWrite &msg);

    void to(QIODevice *device) const;
    DownloadIntervalWrite(const QByteArray &GUID, quint8 flags, const QByteArray &encodedData);


    void printDebug(QDebug &dbg) const override;

    inline QByteArray getGUID() const
    {
        return GUID;
    }

    inline QByteArray getEncodedData() const
    {
        return encodedData;
    }

    inline bool downloadIsComplete() const
    {
        return flags == 1;
    }

private:
    QByteArray GUID;
    quint8 flags;
    QByteArray encodedData;
};

// ++++++++++++++++++++
}       // client ns
} // ninjam ns

#endif
