#ifndef SERVERMESSAGEPROCESSOR_H
#define SERVERMESSAGEPROCESSOR_H

#include <QIODevice>
#include <QDataStream>
#include "log/Logging.h"
#include "Service.h"

namespace Ninjam {
class Service;

struct MessageHeader {
    quint8 messageTypeCode;
    quint32 payload;
};

class ServerMessagesHandler
{
public:
    explicit ServerMessagesHandler(Service *service);
    void initialize(QIODevice *device);
    virtual void handleAllMessages();

protected:
    QDataStream stream;
    QIODevice *device;
    Service *service;
    QScopedPointer<MessageHeader> currentHeader;// the last messageHeader readed from socket

    bool executeMessageHandler(MessageHeader *header);

    template<class MessageClazz> // MessageClazz will be 'translated' to some class derived from ServerMessage
    bool handleMessage(quint32 payload)
    {
        Q_ASSERT(device);
        bool allMessageDataIsAvailable = device->bytesAvailable() >= payload;
        if (allMessageDataIsAvailable) {
            MessageClazz message(payload);
            stream >> message;
            if (service)
                service->process(message);// calling overload versions of 'process'
            return true; // the message was handled
        }
        return false;// the message was not handled
    }

    MessageHeader *extractNextMessageHeader();
};

QDataStream &operator >>(QDataStream &stream, MessageHeader *header);
}// namespace
#endif // SERVERMESSAGEPROCESSOR_H
