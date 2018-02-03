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

    ServerMessageType type = static_cast<ServerMessageType>(header.getMessageType());
    switch (type) {
    case ServerMessageType::AUTH_CHALLENGE:
        return handleMessage<AuthChallengeMessage>(header.getPayload());
    case ServerMessageType::AUTH_REPLY:
        return handleMessage<AuthReplyMessage>(header.getPayload());
    case ServerMessageType::SERVER_CONFIG_CHANGE_NOTIFY:
        return handleMessage<ConfigChangeNotifyMessage>(header.getPayload());
    case ServerMessageType::USER_INFO_CHANGE_NOTIFY:
        return handleMessage<UserInfoChangeNotifyMessage>(header.getPayload());
    case ServerMessageType::KEEP_ALIVE:
        return handleMessage<ServerKeepAliveMessage>(header.getPayload());
    case ServerMessageType::CHAT_MESSAGE:
        return handleMessage<ServerChatMessage>(header.getPayload());
    case ServerMessageType::DOWNLOAD_INTERVAL_BEGIN:
        return handleMessage<DownloadIntervalBegin>(header.getPayload());
    case ServerMessageType::DOWNLOAD_INTERVAL_WRITE:
        return handleMessage<DownloadIntervalWrite>(header.getPayload());
    default:
        qCritical() << "Can't handle the message code " << QString::number(header.getMessageType());
    }
    return false;
}
