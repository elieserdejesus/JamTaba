#pragma once

#include <QByteArray>
#include <QString>
#include <QStringList>

namespace Ninjam {
class User;

// +++++++++++++++++++++++++++
class ClientMessage
{
public:
    ClientMessage(quint8 msgCode, quint32 payload);
    virtual ~ClientMessage()
    {
    }

    virtual void serializeTo(QByteArray &buffer) = 0;
    void virtual printDebug(QDebug dbg) const = 0;

    inline quint8 getMsgType() const
    {
        return msgType;
    }

    inline quint32 getPayload() const
    {
        return payload;
    }

protected:
    static void serializeString(const QString &str, QDataStream &stream);
    static void serializeByteArray(const QByteArray &array, QDataStream &stream);
    quint8 msgType;
    quint32 payload;
};

// ++++++++++++++++++++++++++++++++++++++=
// ++++++++++++++++++++++++++++++++++++++=
class ClientAuthUserMessage : public ClientMessage
{
private:
    QByteArray passwordHash;
    QString userName;
    quint32 clientCapabilites;
    quint32 protocolVersion;
    QByteArray challenge;
public:
    ClientAuthUserMessage(QString userName, QByteArray challenge, quint32 protocolVersion,
                          QString password);
    void serializeTo(QByteArray &buffer);
    void virtual printDebug(QDebug dbg) const;
};
// +++++++++++++++++++++++++++++++++++++++=
class ClientSetChannel : public ClientMessage
{
public:
    virtual void serializeTo(QByteArray &stream);
    virtual void printDebug(QDebug dbg) const;
    ClientSetChannel(QStringList channels);
    ClientSetChannel(QString channelNameToRemove);
private:
    QStringList channelNames;
    quint16 volume;
    quint8 pan;
    quint8 flags;
};
// ++++++++++++++++++++++++++
class ClientKeepAlive : public ClientMessage
{
public:
    ClientKeepAlive();
    virtual void serializeTo(QByteArray &stream);
    virtual void printDebug(QDebug dbg) const;
};
// ++++++++++++++++++++++++++++++

class ClientSetUserMask : public ClientMessage
{
private:
    QStringList usersFullNames;
    static const quint32 FLAG = 0xFFFFFFFF;
public:
    explicit ClientSetUserMask(QList<QString> users);
    virtual void serializeTo(QByteArray &stream);
    virtual void printDebug(QDebug dbg) const;
};

// +++++++++++++++++++++++++++
class ChatMessage : public ClientMessage
{
public:
    explicit ChatMessage(QString text);
    virtual void serializeTo(QByteArray &stream);
    virtual void printDebug(QDebug dbg) const;
private:
    QString text;
    QString command;
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
class ClientUploadIntervalBegin : public ClientMessage
{
private:
    QByteArray GUID;
    quint32 estimatedSize;
    char fourCC[4];
    quint8 channelIndex;
    QString userName;
public:
    ClientUploadIntervalBegin(QByteArray GUID, quint8 channelIndex, QString userName);

    static QByteArray newGUID();

    virtual void serializeTo(QByteArray &stream);
    virtual void printDebug(QDebug dbg) const;

    inline QByteArray getGUID() const
    {
        return GUID;
    }
};

// ++++++++++++++++++++++++++++++++++++++++++++++++=
class ClientIntervalUploadWrite : public ClientMessage
{
private:
    QByteArray GUID;
    QByteArray encodedAudioBuffer;
    bool isLastPart;
public:
    ClientIntervalUploadWrite(QByteArray GUID, QByteArray encodedAudioBuffer, bool isLastPart);
    virtual void serializeTo(QByteArray &buffer);
    virtual void printDebug(QDebug dbg) const;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QDebug operator<<(QDebug dbg, Ninjam::ClientMessage *message);
}
