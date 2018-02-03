#include "Ninjam.h"

namespace ninjam {

MessageHeader::MessageHeader(quint8 type, quint32 payload) :
    messageTypeCode(type),
    payload(payload)
{

}

MessageHeader::MessageHeader() :
    MessageHeader(0xff, 0) // invalid header
{

}

MessageHeader MessageHeader::from(QIODevice *device)
{
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::LittleEndian);

    quint8 type;
    quint32 payload;

    stream >> type >> payload;

    return MessageHeader(type, payload);
}

void serializeString(const QString &string, QDataStream &stream)
{
    QByteArray dataArray = string.toUtf8();
    stream.writeRawData(dataArray.data(), dataArray.size());

    stream << quint8('\0'); // NUL TERMINATED
}

void serializeByteArray(const QByteArray &array, QDataStream &stream)
{
    for (int i = 0; i < array.size(); ++i) {
        stream << quint8(array[i]);
    }
}

} // namespace


