#include "ServerMessagesHandler.h"
#include "ServerMessages.h"

using ninjam::client::ServerMessagesHandler;
using ninjam::client::Service;
using ninjam::client::MessageHeader;

ServerMessagesHandler::ServerMessagesHandler(Service *service) :
    service(service),
    device(nullptr)
{
    //
}

ServerMessagesHandler::~ServerMessagesHandler()
{
    //
}

void ServerMessagesHandler::initialize(QIODevice *device)
{
    Q_ASSERT(device);
    this->device = device;
    stream.setDevice(device);
    stream.setByteOrder(QDataStream::LittleEndian);
    currentHeader.reset(nullptr);
}

void ServerMessagesHandler::handleAllMessages()
{
    Q_ASSERT(device);
    while (device->bytesAvailable() >= 5) { // consume all messages. Every ninjam message contains a 5 bytes header.
        if (!currentHeader)
            currentHeader.reset(extractNextMessageHeader());

        if (!currentHeader) // just to avoid the possibility of a null header
            return;

        bool successfullyProcessed = executeMessageHandler(currentHeader.data());
        if (successfullyProcessed)
            currentHeader.reset(); // a new header will be readed from socket in the next loop iteration
        else
            break; // an incomplete message was founded, break and wait until receive more bytes. The currentHeader will be used in the next iteration.
    }
}

MessageHeader *ServerMessagesHandler::extractNextMessageHeader()
{
    Q_ASSERT(device);
    if (device->bytesAvailable() < 5) {
        qCDebug(jtNinjamProtocol) << "have not enough bytes to read message header (5 bytes)";
        return nullptr;
    }
    MessageHeader *header = new MessageHeader();
    stream >> header;
    return header;
}

bool ServerMessagesHandler::executeMessageHandler(MessageHeader *header)
{
    if (!header)
        return false;

    ServerMessageType type = static_cast<ServerMessageType>(header->messageTypeCode);
    switch (type) {
    case ServerMessageType::AUTH_CHALLENGE:
        return handleMessage<ServerAuthChallengeMessage>(header->payload);
    case ServerMessageType::AUTH_REPLY:
        return handleMessage<ServerAuthReplyMessage>(header->payload);
    case ServerMessageType::SERVER_CONFIG_CHANGE_NOTIFY:
        return handleMessage<ServerConfigChangeNotifyMessage>(header->payload);
    case ServerMessageType::USER_INFO_CHANGE_NOTIFY:
        return handleMessage<UserInfoChangeNotifyMessage>(header->payload);
    case ServerMessageType::KEEP_ALIVE:
        return handleMessage<ServerKeepAliveMessage>(header->payload);
    case ServerMessageType::CHAT_MESSAGE:
        return handleMessage<ServerChatMessage>(header->payload);
    case ServerMessageType::DOWNLOAD_INTERVAL_BEGIN:
        return handleMessage<DownloadIntervalBegin>(header->payload);
    case ServerMessageType::DOWNLOAD_INTERVAL_WRITE:
        return handleMessage<DownloadIntervalWrite>(header->payload);
    default:
        qCritical() << "Can't handle the message code " << QString::number(header->messageTypeCode);
    }
    return false;
}

QDataStream &ninjam::client::operator >>(QDataStream &stream, ninjam::client::MessageHeader *header)
{
    if (header)
        stream >> header->messageTypeCode >> header->payload;

    return stream;
}
