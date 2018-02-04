#ifndef CLIENT_MESSAGES_H
#define CLIENT_MESSAGES_H

#include <QtGlobal>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QDebug>

namespace ninjam {
namespace client {

class User;

class ClientMessage
{
public:
    ClientMessage(quint8 msgCode, quint32 payload);
    virtual ~ClientMessage();

    virtual void printDebug(QDebug &dbg) const = 0;
    virtual void serializeTo(QIODevice *device) const = 0;

    inline quint8 getMsgType() const
    {
        return msgType;
    }

    inline quint32 getPayload() const
    {
        return payload;
    }

protected:

    quint8 msgType; // TODO this is common to server and client messages
    quint32 payload;
};

// ++++++++++++++++++++++++++++++++++++++=

class ClientAuthUserMessage : public ClientMessage
{
public:
    ClientAuthUserMessage(const QString &userName, const QByteArray &challenge,
                          quint32 protocolVersion, const QString &password);

    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;

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

    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;

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
    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;
};

// ++++++++++++++++++++++++++++++

class ClientSetUserMask : public ClientMessage
{
public:
    explicit ClientSetUserMask(const QString &userName, quint32 channelsMask);
    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;

private:
    QString userName;
    quint32 channelsMask;
};

// +++++++++++++++++++++++++++

class ChatMessage : public ClientMessage
{
public:

    enum ChatMessageType
    {
        PublicMessage,              // MSG
        PrivateMessage,             // PRIVMSG
        TopicMessage,               // TOPIC
        AdminMessage                // ADMIN
    };

    ChatMessage(const QString &text, ChatMessageType type = ChatMessageType::PublicMessage);

    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;

private:
    QString text;
    QString command;
    ChatMessageType type;

    static QString getTypeCommand(ChatMessageType type);
    static QString satinizeText(const QString &msg, ChatMessageType type);
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
    ClientUploadIntervalBegin(const QByteArray &GUID, quint8 channelIndex, const QString &userName,
                              bool isAudioInterval);

    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;

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
};

// ++++++++++++++++++++++++++++++++++++++++++++++++=

class ClientIntervalUploadWrite : public ClientMessage
{
public:
    ClientIntervalUploadWrite(const QByteArray &GUID, const QByteArray &encodedData,
                              bool isLastPart);

    void serializeTo(QIODevice *device) const override;
    void printDebug(QDebug &dbg) const override;

private:
    QByteArray GUID;
    QByteArray encodedData;
    bool isLastPart;
};
}     // ns
} // ns

#endif
