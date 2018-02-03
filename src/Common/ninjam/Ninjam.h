#ifndef _NINJAM_H_
#define _NINJAM_H_

#include <QString>
#include <QDataStream>

namespace ninjam {

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

} // namespace

#endif
