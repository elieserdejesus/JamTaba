#ifndef _NINJAM_H_
#define _NINJAM_H_

#include <QString>
#include <QDataStream>

namespace ninjam {

enum class MessageType : quint8
{
    AuthChallenge = 0x00,               // received after connect in server
    ClientAuthUser = 0x80,              // sended to server after receive AUTH_CHALLENGE
    AuthReply = 0x01,                   // received after send to auth challenge
    ServerConfigChangeNotify = 0x02,    // received when BPI or BPM change
    UserInfoChangeNorify = 0x03,        // received when user add/remove channels or rename a channel
    DownloadIntervalBegin = 0x04,       // received when server is notifiyng about the start of a new audio interval stream
    DownloadIntervalWrite = 0x05,       // received for every audio interval chunk. We receive a lot of these messages while jamming.
    KeepAlive = 0xfd,                   // server requesting a keepalive. If Jamtaba not respond the server will disconnect.
    ChatMessage = 0xc0                  // received when users are sending chat messages
};

class MessageHeader
{
public:
    MessageHeader(); // invalid/incomplete message header

    static MessageHeader from(QIODevice *device);

    inline bool isValid() const
    {
        return messageTypeCode != 0xff;
    }

    inline quint8 getMessageType() const
    {
        return messageTypeCode;
    }

    inline quint32 getPayload() const
    {
        return payload;
    }

private:
    MessageHeader(quint8 type, quint32 payload);


    quint8 messageTypeCode = 0xff;
    quint32 payload = 0;
};

void serializeString(const QString &string, QDataStream &stream);
void serializeByteArray(const QByteArray &array, QDataStream &stream);

QString extractString(QDataStream &stream);         // ninjam strings are NUL(\0) terminated
QString extractString(QDataStream &stream, quint32 size);

} // namespace

#endif
