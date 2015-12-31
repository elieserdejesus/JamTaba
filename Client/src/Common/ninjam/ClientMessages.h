#ifndef CLIENT_MESSAGES_H
#define CLIENT_MESSAGES_H

#include <QtGlobal>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QDebug>

namespace Ninjam {
class User;

// +++++++++++++++++++++++++++
class ClientMessage
{
    friend QDebug &operator<<(QDebug &dbg, const ClientMessage &message);
    friend QByteArray &operator <<(QByteArray &byteArray, const ClientMessage &message);

public:
    ClientMessage(quint8 msgCode, quint32 payload);
    virtual ~ClientMessage();

    inline quint8 getMsgType() const
    {
        return msgType;
    }

    inline quint32 getPayload() const
    {
        return payload;
    }

protected:
    static void serializeString(const QString &string, QDataStream &stream);
    static void serializeByteArray(const QByteArray &array, QDataStream &stream);

    quint8 msgType;
    quint32 payload;

private:
    void virtual printDebug(QDebug &dbg) const = 0;
    virtual void serializeTo(QByteArray &buffer) const = 0;
};

// ++++++++++++++++++++++++++++++++++++++=
// ++++++++++++++++++++++++++++++++++++++=
class ClientAuthUserMessage : public ClientMessage
{

public:
    ClientAuthUserMessage(const QString &userName, const QByteArray &challenge,
                          quint32 protocolVersion, const QString &password);

private:
    QByteArray passwordHash;
    QString userName;
    quint32 clientCapabilites;
    quint32 protocolVersion;
    QByteArray challenge;

    void serializeTo(QByteArray &buffer) const override;
    void printDebug(QDebug &dbg) const override;
};
// +++++++++++++++++++++++++++++++++++++++=
class ClientSetChannel : public ClientMessage
{
public:
    ClientSetChannel(const QStringList &channels);
    ClientSetChannel(const QString &channelNameToRemove);

private:
    QStringList channelNames;
    quint16 volume;
    quint8 pan;
    quint8 flags;

    void serializeTo(QByteArray &stream) const override;
    void printDebug(QDebug &dbg) const override;
};
// ++++++++++++++++++++++++++
class ClientKeepAlive : public ClientMessage
{
public:
    ClientKeepAlive();

private:
    void serializeTo(QByteArray &stream) const override;
    void printDebug(QDebug &dbg) const override;
};
// ++++++++++++++++++++++++++++++

class ClientSetUserMask : public ClientMessage
{
public:
    explicit ClientSetUserMask(const QList<QString> &users);

private:
    QStringList usersFullNames;
    static const quint32 FLAG = 0xFFFFFFFF;

    void serializeTo(QByteArray &stream) const override;
    void printDebug(QDebug &dbg) const override;
};

// +++++++++++++++++++++++++++
class ChatMessage : public ClientMessage
{
public:
    explicit ChatMessage(const QString &text);

private:
    QString text;
    QString command;

    void serializeTo(QByteArray &stream) const override;
    void printDebug(QDebug &dbg) const override;
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
public:
    ClientUploadIntervalBegin(const QByteArray &GUID, quint8 channelIndex, const QString &userName);

    static QByteArray createGUID();

    inline QByteArray getGUID() const
    {
        return GUID;
    }

private:
    QByteArray GUID;
    quint32 estimatedSize;
    char fourCC[4];
    quint8 channelIndex;
    QString userName;

    void serializeTo(QByteArray &stream) const override;
    void printDebug(QDebug &dbg) const override;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++=
class ClientIntervalUploadWrite : public ClientMessage
{

public:
    ClientIntervalUploadWrite(const QByteArray &GUID, const QByteArray &encodedAudioBuffer,
                              bool isLastPart);

private:
    QByteArray GUID;
    QByteArray encodedAudioBuffer;
    bool isLastPart;

    void serializeTo(QByteArray &buffer) const override;
    void printDebug(QDebug &dbg) const override;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QDebug &operator<<(QDebug &dbg, const Ninjam::ClientMessage &message);

QByteArray &operator <<(QByteArray &byteArray, const Ninjam::ClientMessage &message);

}

#endif
