#ifndef SERVERMESSAGEPROCESSOR_H
#define SERVERMESSAGEPROCESSOR_H

#include <QIODevice>
#include <QDataStream>
#include "log/Logging.h"
#include "Service.h"
#include "ninjam/Ninjam.h"

namespace ninjam
{

namespace client
{
    class Service;

    class ServerMessagesHandler
    {

    public:
        explicit ServerMessagesHandler(Service *service);
        virtual ~ServerMessagesHandler();
        void initialize(QIODevice *device);
        virtual void handleAllMessages();

    protected:
        QIODevice *device;
        Service *service;
        MessageHeader currentHeader; // the last messageHeader readed from socket

        bool executeMessageHandler(const MessageHeader &header);

        template<class MessageClazz> // MessageClazz will be 'translated' to some class derived from ServerMessage
        bool handleMessage(quint32 payload)
        {
            Q_ASSERT(device);
            Q_ASSERT(service);

            bool allMessageDataIsAvailable = device->bytesAvailable() >= payload;
            if (allMessageDataIsAvailable) {
                auto msg = MessageClazz::from(device, payload);
                service->process(msg); // calling overload versions of 'process'
                return true; // the message was handled
            }
            return false; // the message was not handled
        }
    };

    } // ns
} // ns

#endif // SERVERMESSAGEPROCESSOR_H
