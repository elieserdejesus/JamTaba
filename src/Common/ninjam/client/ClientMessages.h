#ifndef CLIENT_MESSAGES_H
#define CLIENT_MESSAGES_H

#include <QtGlobal>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QIODevice>
#include "ninjam/Ninjam.h"

namespace ninjam {

namespace client {

class User;

using ninjam::MessageType;

class ClientMessage
{
public:
    ClientMessage(MessageType type, quint32 payload);
    virtual ~ClientMessage();

    virtual void printDebug(QDebug &dbg) const = 0;
    virtual void serializeTo(QIODevice *device) const = 0;

    inline MessageType getMsgType() const
    {
        return msgType;
    }

    inline quint32 getPayload() const
    {
        return payload;
    }

protected:

    MessageType msgType; // TODO this is common to server and client messages
    quint32 payload;
};

// ++++++++++++++++++++++++++++++++++++++=

class ClientAuthUserMessage : public ClientMessage
{
public:
    ClientAuthUserMessage(const QString &userName, const QByteArray &challenge,
                          quint32 protocolVersion, const QString &password);

    static ClientAuthUserMessage unserializeFrom(QIODevice *device, quint32 payload);
    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;

    inline QString getUserName() const
    {
        return userName;
    }

private:
    QByteArray passwordHash;
    QString userName;
    quint32 clientCapabilites;
    quint32 protocolVersion;
    QByteArray challenge;
};

// +++++++++++++++++++++++++++++++++++++++=

class ClientSetChannel : public ClientMessage
{
public:
    explicit ClientSetChannel(const QStringList &channels);
    explicit ClientSetChannel(const QString &channelNameToRemove);

    static ClientSetChannel unserializeFrom(QIODevice *device, quint32 payload);
    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;

    inline QStringList getChannelNames() const
    {
        return channelNames;
    }
private:
    QStringList channelNames;
    quint16 volume;
    quint8 pan;
    quint8 flags;
};

// ++++++++++++++++++++++++++

// TODO merge Client and ServerKeep Alive
class ClientKeepAlive : public ClientMessage
{
public:
    ClientKeepAlive();
    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;
};

// ++++++++++++++++++++++++++++++

class ClientSetUserMask : public ClientMessage
{
public:
    explicit ClientSetUserMask(const QString &userName, quint32 channelsMask);

    static ClientSetUserMask from(QIODevice *device, quint32 payload);

    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;

private:
    QString userName;
    quint32 channelsMask;
};

// +++++++++++++++++++++++++++

class ClientToServerChatMessage : public ClientMessage
{
public:

    static ClientToServerChatMessage buildPublicMessage(const QString &message);
    static ClientToServerChatMessage buildPrivateMessage(const QString &message, const QString &destionationUserName);
    static ClientToServerChatMessage buildAdminMessage(const QString &message);

    static ClientToServerChatMessage from(QIODevice *device, quint32 payload);

    inline QString getCommand() const
    {
        return command;
    }

    inline QStringList getArguments() const
    {
        return arguments;
    }

    bool isPublicMessage() const;
    bool isAdminMessage() const;
    bool isPrivateMessage() const;

    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;

private:
    ClientToServerChatMessage(const QString &command, const QString &arg1, const QString &arg2, const QString &arg3, const QString &arg4);
    QString command;
    QStringList arguments;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/**
Offset Type        Field
0x0    uint8_t[16] GUID (binary)
0x10   uint32_t    Estimated Size
0x14   uint8_t[4]  FourCC
0x18   uint8_t     Channel Index
0x19   ...         Username (NUL-terminated)
*/

class UploadIntervalBegin : public ClientMessage
{
public:
    UploadIntervalBegin(const QByteArray &GUID, quint8 channelIndex, bool isAudioInterval);

    static UploadIntervalBegin from(QIODevice *device, quint32 payload);

    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;

    static QByteArray createGUID();

    inline QByteArray getGUID() const
    {
        return GUID;
    }

    inline QByteArray getFourCC() const
    {
        return QByteArray(&fourCC[0], 4);
    }

    inline quint32 getEstimatedSize() const
    {
        return estimatedSize;
    }

    inline quint8 getChannelIndex() const
    {
        return channelIndex;
    }

private:
    QByteArray GUID;
    quint32 estimatedSize;
    char fourCC[4];
    quint8 channelIndex;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++=

class UploadIntervalWrite : public ClientMessage
{
public:
    UploadIntervalWrite(const QByteArray &GUID, const QByteArray &encodedData, bool lastPart);

    static UploadIntervalWrite from(QIODevice *device, quint32 payload);

    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;

    inline QByteArray getEncodedData() const
    {
        return encodedData;
    }

    inline QByteArray getGUID() const
    {
        return GUID;
    }

    inline bool isLastPart() const
    {
        return lastPart;
    }

private:
    QByteArray GUID;
    QByteArray encodedData;
    bool lastPart;
};
}     // ns
} // ns

#endif
