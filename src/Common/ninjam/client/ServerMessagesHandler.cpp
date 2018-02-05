#include "ServerMessagesHandler.h"
#include "ServerMessages.h"

using namespace ninjam;
using namespace ninjam::client;

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
    currentHeader = MessageHeader();
}

void ServerMessagesHandler::handleAllMessages()
{
    Q_ASSERT(device);
    while (device->bytesAvailable() >= 5) { // consume all messages. Every ninjam message contains a 5 bytes header.
        if (!currentHeader.isValid())
            currentHeader = MessageHeader::from(device);

        bool successfullyProcessed = executeMessageHandler(currentHeader);
        if (successfullyProcessed)
            currentHeader = MessageHeader(); // invalidate the current message header, a new header will be readed from socket in the next loop iteration
        else
            break; // an incomplete message was founded, break and wait until receive more bytes. The currentHeader will be used in the next iteration.
    }
}

bool ServerMessagesHandler::executeMessageHandler(const MessageHeader &header)
{
    Q_ASSERT(header.isValid());

    MessageType type = static_cast<MessageType>(header.getMessageType());
    switch (type) {
    case MessageType::AuthChallenge:
        return handleMessage<AuthChallengeMessage>(header.getPayload());
    case MessageType::AuthReply:
        return handleMessage<AuthReplyMessage>(header.getPayload());
    case MessageType::ServerConfigChangeNotify:
        return handleMessage<ConfigChangeNotifyMessage>(header.getPayload());
    case MessageType::UserInfoChangeNorify:
        return handleMessage<UserInfoChangeNotifyMessage>(header.getPayload());
    case MessageType::KeepAlive:
        return handleMessage<ServerKeepAliveMessage>(header.getPayload());
    case MessageType::ChatMessage:
        return handleMessage<ServerChatMessage>(header.getPayload());
    case MessageType::DownloadIntervalBegin:
        return handleMessage<DownloadIntervalBegin>(header.getPayload());
    case MessageType::DownloadIntervalWrite:
        return handleMessage<DownloadIntervalWrite>(header.getPayload());
    default:
        qCritical() << "Can't handle the message code " << QString::number(header.getMessageType());
    }
    return false;
}
