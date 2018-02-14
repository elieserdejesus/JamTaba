#include "Ninjam.h"

#include <QDateTime>

namespace ninjam {

NetworkUsageMeasurer::NetworkUsageMeasurer() :
    totalBytesTransfered(0),
    lastMeasureTimeStamp(0),
    transferRate(0)
{

}

void NetworkUsageMeasurer::addTransferedBytes(qint64 bytesTransfered)
{
    totalBytesTransfered += bytesTransfered;

    const qint64 now = QDateTime::currentMSecsSinceEpoch();

    if (!lastMeasureTimeStamp)
        lastMeasureTimeStamp = now;

    const qint64 ellapsedTime = now - lastMeasureTimeStamp;

    if (ellapsedTime >= 1000) {
        transferRate = totalBytesTransfered / (ellapsedTime / 1000.0);
        totalBytesTransfered = 0;
        lastMeasureTimeStamp = now;
    }
}

long NetworkUsageMeasurer::getTransferRate() const
{
    return transferRate;
}

// ---------------------------------------------------------------

MessageHeader::MessageHeader(quint8 type, quint32 payload) :
    messageType(static_cast<MessageType>(type)),
    payload(payload)
{

}

MessageHeader::MessageHeader() :
    MessageHeader(static_cast<quint8>(MessageType::Invalid), 0) // invalid header
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

QString extractString(QDataStream &stream)
{
    quint8 byte;
    QByteArray byteArray;
    while (!stream.atEnd()) {
        stream >> byte;
        if (byte == '\0')
            break;
        byteArray.append(byte);
    }
    return QString::fromUtf8(byteArray.data(), byteArray.size());
}

QString extractString(QDataStream &stream, quint32 size)
{
    return QString::fromUtf8(stream.device()->read(size));
}

} // namespace


