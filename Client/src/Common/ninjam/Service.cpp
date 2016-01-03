#include "Service.h"
#include "Server.h"
#include "User.h"
#include "UserChannel.h"
#include "protocol/ServerMessages.h"
#include "protocol/ClientMessages.h"
#include "log/Logging.h"
#include <QDataStream>

using namespace Ninjam;

const QStringList Service::botNames = buildBotNamesList();

// ++++++++++++++++++++++++++++

ServerMessageVisitor::ServerMessageVisitor(Service *service) :
    service(service)
{
}

void ServerMessageVisitor::visitAuthChallenge(const ServerAuthChallengeMessage &msg)
{
    service->processAuthChallenge(msg);
}

void ServerMessageVisitor::visitAuthReply(const ServerAuthReplyMessage &msg)
{
    service->processAuthReply(msg);
}

void ServerMessageVisitor::visitChatMessage(const ServerChatMessage &msg)
{
    service->processChatMessage(msg);
}

void ServerMessageVisitor::visitConfigChangeNotify(const ServerConfigChangeNotifyMessage &msg)
{
    service->processConfigChangeNotify(msg);
}

void ServerMessageVisitor::visitDownloadIntervalBegin(const DownloadIntervalBegin &msg)
{
    service->processDownloadIntervalBegin(msg);
}

void ServerMessageVisitor::visitDownloadIntervalWrite(const DownloadIntervalWrite &msg)
{
    service->processDownloadIntervalWrite(msg);
}

void ServerMessageVisitor::visitKeepAlive(const ServerKeepAliveMessage &msg)
{
    service->processKeepAlive(msg);
}

void ServerMessageVisitor::visitUserInfoChangeNotify(const UserInfoChangeNotifyMessage &msg)
{
    service->processUserInfoChangeNotify(msg);
}

// ++++++++++++++++++++++++++++
/**
    This is a nested class used to bind the downloaded audio data (encoded in ogg vorbis) with a GUID (global unique ID),
an user name and a channel index (users can use more than one channel). When a dowload is finished (the ninjam audio interval is
fully downloaded) we need emit a signal, and in this moment we need the user name and the channel index.
*/
class Service::Download
{

public:
    Download(const QString &userFullName, quint8 channelIndex, const QByteArray &GUID) :
        channelIndex(channelIndex),
        userFullName(userFullName),
        GUID(GUID)
    {
        qCDebug(jtNinjamProtocol) << "Download constructor ";
    }

    Download()//this constructor is necessary to use Download in a QMap without pointers
    {

    }

    ~Download()
    {

    }

    inline void appendVorbisData(QByteArray data)
    {
        this->vorbisData.append(data);
    }

    inline quint8 getChannelIndex() const
    {
        return channelIndex;
    }

    inline QString getUserFullName() const
    {
        return userFullName;
    }

    inline QByteArray getGUI() const
    {
        return GUID;
    }

    inline QByteArray getVorbisData() const
    {
        return vorbisData;
    }

private:
    quint8 channelIndex;
    QString userFullName;
    QByteArray GUID; //Global Unique ID
    QByteArray vorbisData;
};

// ++++++++++++++++++++++++++++++++++++++++

Service::Service() :
    lastSendTime(0),
    initialized(false)
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(handleAllReceivedMessages()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
            SLOT(handleSocketError(QAbstractSocket::SocketError)));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(handleSocketDisconnection()));
    connect(&socket, SIGNAL(connected()), this, SLOT(handleSocketConnection()));
}

Service::~Service()
{
    disconnect(&socket, SIGNAL(readyRead()), this, SLOT(handleAllReceivedMessages()));
    disconnect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
               SLOT(handleSocketError(QAbstractSocket::SocketError)));
    disconnect(&socket, SIGNAL(disconnected()), this, SLOT(handleSocketDisconnection()));
    disconnect(&socket, SIGNAL(connected()), this, SLOT(handleSocketConnection()));

    if (socket.isValid() && socket.isOpen())
        socket.disconnectFromHost();
}

void Service::sendAudioIntervalPart(const QByteArray &GUID, const QByteArray &encodedAudioBuffer,
                                    bool isLastPart)
{
    qCDebug(jtNinjamProtocol) << "sending audio interval part";
    if (!initialized)
        return;
    sendMessageToServer(ClientIntervalUploadWrite(GUID, encodedAudioBuffer, isLastPart));
}

void Service::sendAudioIntervalBegin(const QByteArray &GUID, quint8 channelIndex)
{
    qCDebug(jtNinjamProtocol) << "sending audio interval begin";
    if (!initialized)
        return;
    sendMessageToServer(ClientUploadIntervalBegin(GUID, channelIndex, this->userName));
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MessageHeader* Service::extractMessageHeader(QDataStream &stream){
    if (socket.bytesAvailable() < 5) {
        qCDebug(jtNinjamProtocol) << "have not enough bytes to read message header (5 bytes)";
        return nullptr;
    }
    MessageHeader* header = new MessageHeader();
    stream >> header;
    return header;
}

bool Service::executeMessageHandler(MessageHeader *header, QDataStream &stream)
{
    if(!header){
        return false;
    }

    switch (header->messageTypeCode) {
    case ServerMessageType::AUTH_CHALLENGE: return handleMessage<ServerAuthChallengeMessage>(stream, header->payload);
    case ServerMessageType::AUTH_REPLY: return handleMessage<ServerAuthReplyMessage>(stream, header->payload);
    case ServerMessageType::SERVER_CONFIG_CHANGE_NOTIFY: return handleMessage<ServerConfigChangeNotifyMessage>(stream, header->payload);
    case ServerMessageType::USER_INFO_CHANGE_NOTIFY: return handleMessage<UserInfoChangeNotifyMessage>(stream, header->payload);
    case ServerMessageType::KEEP_ALIVE: return handleMessage<ServerKeepAliveMessage>(stream, header->payload);
    case ServerMessageType::CHAT_MESSAGE: return handleMessage<ServerChatMessage>(stream, header->payload);
    case ServerMessageType::DOWNLOAD_INTERVAL_BEGIN: return handleMessage<DownloadIntervalBegin>(stream, header->payload);
    case ServerMessageType::DOWNLOAD_INTERVAL_WRITE: return handleMessage<DownloadIntervalWrite>(stream, header->payload);
    default:
        qCritical() << "Can't handle the message code " << QString::number(header->messageTypeCode);
    }
    return false;
}

//this slot is invoke when socket receive new data
void Service::handleAllReceivedMessages()
{
    if (socket.bytesAvailable() < 5) {
        qCDebug(jtNinjamProtocol) << "not have enough bytes to read message header (5 bytes)";
        return nullptr;
    }

    qCDebug(jtNinjamProtocol) << "socket read slot";

    QDataStream stream(&socket);
    stream.setByteOrder(QDataStream::LittleEndian);

    while(socket.bytesAvailable() >= 5){//consume all messages. Every ninjam message contains a 5 bytes header.
        if(!currentHeader){
            currentHeader.reset(extractMessageHeader(stream));
        }

        if(!currentHeader)//just to avoid the possibility of a null header
            return;

        bool successfullyProcessed = executeMessageHandler(currentHeader.data(), stream);
        if(successfullyProcessed){
            currentHeader.reset(); //a new header will be readed from socket in the next loop iteration
        }else{
            break;// an incomplete message was founded, break and wait until receive more bytes. The currentHeader will be used in the next iteration.
        }
    }
    return nullptr;
}

void Service::clear(){
    initialized = false;
    currentHeader.reset();
    currentServer.reset();
}

void Service::handleSocketError(QAbstractSocket::SocketError e)
{
    Q_UNUSED(e);
    qCWarning(jtNinjamProtocol) << QString(socket.errorString());
    clear();
    emit error(socket.errorString());
}

void Service::handleSocketConnection()
{
    qCDebug(jtNinjamProtocol) << "socket connected on " << socket.peerName();
}

void Service::handleSocketDisconnection()
{
    qCDebug(jtNinjamProtocol) << "socket disconnected from " << socket.peerName();
    emit disconnectedFromServer(*currentServer);
    clear();
}

bool Service::isBotName(const QString &userName)
{
    return botNames.contains(userName.trimmed());
}

QStringList Service::buildBotNamesList()
{
    QStringList names;
    names.append("Jambot");
    names.append("ninjamer.com");
    names.append("ninbot");
    names.append("ninbot.com");
    names.append("MUTANTLAB");
    names.append("mutantlab.com");
    names.append("LiveStream");
    names.append("localhost");
    return names;
}

QString Service::getConnectedUserName()
{
    if (initialized)
        return userName;
    qCritical() << "not initialized, newUserName is not available!";
    return "";
}

float Service::getIntervalPeriod()
{
    if (currentServer)
        return 60000.0f / currentServer->getBpm() * currentServer->getBpi();
    return 0.0f;
}

void Service::voteToChangeBPI(int newBPI)
{
    QString text = "!vote bpi " + QString::number(newBPI);
    sendMessageToServer(ChatMessage(text));
}

void Service::voteToChangeBPM(int newBPM)
{
    QString text = "!vote bpm " + QString::number(newBPM);
    sendMessageToServer(ChatMessage(text));
}

void Service::sendChatMessageToServer(const QString &message)
{
    sendMessageToServer(ChatMessage(message));
}

void Service::sendMessageToServer(const ClientMessage &message)
{
    QByteArray outBuffer;
    outBuffer << message;

    int totalDataToSend = outBuffer.size();
    int dataSended = 0;
    int bytesWrited = -1;
    do {
        bytesWrited = socket.write(outBuffer.data() + dataSended, totalDataToSend - dataSended);
        if (bytesWrited > 0)
            dataSended += bytesWrited;
    } while (dataSended < totalDataToSend && bytesWrited != -1);

    if (bytesWrited > 0) {
        socket.flush();
        lastSendTime = QDateTime::currentMSecsSinceEpoch();
    } else {
        qCCritical(jtNinjamProtocol) << "Bytes not writed in socket!";
    }

    Q_ASSERT(message.getPayload() + 5 == outBuffer.size());
}

bool Service::needSendKeepAlive() const
{
    long ellapsedSeconds = (QDateTime::currentMSecsSinceEpoch() - lastSendTime)/1000;
    return ellapsedSeconds >= serverKeepAlivePeriod;
}

void Service::process(const UserInfoChangeNotifyMessage &msg)
{
    QSet<QString> users = QSet<QString>::fromList(msg.getUsersNames());
    foreach (const QString &userFullName, users) {
        if (!currentServer->containsUser(userFullName)) {
            User newUser(userFullName);
            currentServer->addUser(newUser);
            emit userEntered(newUser);
        }
        handleUserChannels(userFullName, msg.getUserChannels(userFullName));
    }

    // enable new users channels
    sendMessageToServer(ClientSetUserMask(msg.getUsersNames()));
}

void Service::process(const DownloadIntervalBegin &msg)
{
    if (!msg.downloadShouldBeStopped() && msg.isValidOggDownload()) {
        quint8 channelIndex = msg.getChannelIndex();
        QString userFullName = msg.getUserName();
        QByteArray GUID = msg.getGUID();
        downloads.insert(GUID, Download(userFullName, channelIndex, GUID));
    }
}

void Service::process(const DownloadIntervalWrite &msg)
{
    if (downloads.contains(msg.getGUID())) {
        Download &download = downloads[msg.getGUID()];
        download.appendVorbisData(msg.getEncodedAudioData());
        User *user = currentServer->getUser(download.getUserFullName());
        if (msg.downloadIsComplete()) {
            emit audioIntervalCompleted(*user, download.getChannelIndex(),
                                        download.getVorbisData());
            downloads.remove(msg.getGUID());
        } else {
            emit audioIntervalDownloading(*user,
                                          download.getChannelIndex(),
                                          msg.getEncodedAudioData().size());
        }
    } else {
        qCritical("GUID is not in map!");
    }
}

void Service::process(const ServerKeepAliveMessage &)
{
    sendMessageToServer(ClientKeepAlive());
}

void Service::process(const ServerAuthChallengeMessage &msg)
{
    ClientAuthUserMessage msgAuthUser(userName, msg.getChallenge(),
                                      msg.getProtocolVersion(), password);
    sendMessageToServer(msgAuthUser);
    serverLicence = msg.getLicenceAgreement();
    serverKeepAlivePeriod = msg.getServerKeepAlivePeriod();
}

void Service::sendNewChannelsListToServer(const QStringList &channelsNames)
{
    this->channels = channelsNames;
    sendMessageToServer(ClientSetChannel(channels));
}

void Service::sendRemovedChannelIndex(int removedChannelIndex)
{
    assert(removedChannelIndex >= 0 && removedChannelIndex < channels.size());
    channels.removeAt(removedChannelIndex);
    sendMessageToServer(ClientSetChannel(channels));
}

void Service::process(const ServerAuthReplyMessage &msg)
{
    if (msg.userIsAuthenticated()) {
        userName = msg.getNewUserName(); // replace the user name with the (possible) new name generated by the ninjam server
        sendMessageToServer(ClientSetChannel(channels));
        quint8 serverMaxChannels = msg.getMaxChannels();
        QString serverIp = socket.peerName();
        quint16 serverPort = socket.peerPort();
        currentServer.reset(new Server(serverIp, serverPort, serverMaxChannels));
    }
    // when user is not authenticated the socketErrorSlot is called and dispatch an error signal
}

void Service::startServerConnection(const QString &serverIp, int serverPort,
                                    const QString &userName, const QStringList &channels,
                                    const QString &password)
{

    clear();//reset some internal state

    this->userName = userName;
    this->password = password;
    this->channels = channels;

    // check state to avoid a bug if user try enter in a server using double click in the button
    if (socket.state() == QTcpSocket::UnconnectedState)
        socket.connectToHost(serverIp, serverPort);
}

void Service::disconnectFromServer(bool emitDisconnectedSignal)
{
    qCDebug(jtNinjamProtocol) << "disconnecting from " << socket.peerName();
    if (socket.isOpen()) {
        if (!emitDisconnectedSignal)
            socket.blockSignals(true); // avoid generate events when disconnecting/exiting
        socket.disconnectFromHost();
    }
}

void Service::setBpm(quint16 newBpm)
{
    Q_ASSERT(currentServer);
    if (currentServer->setBpm(newBpm) && initialized)
        emit serverBpmChanged(currentServer->getBpm());
}

void Service::setBpi(quint16 bpi)
{
    Q_ASSERT(currentServer);
    quint16 lastBpi = currentServer->getBpi();
    if (currentServer->setBpi(bpi) && initialized)
        emit serverBpiChanged(currentServer->getBpi(), lastBpi);
}

// +++++++++++++ SERVER MESSAGE HANDLERS +++++++++++++=
void Service::handleUserChannels(const QString &userFullName,
                                 const QList<UserChannel> &channelsInTheServer)
{
    // check for new channels
    User *user = this->currentServer->getUser(userFullName);
    foreach (const UserChannel &c, channelsInTheServer) {
        if (c.isActive()) {
            if (!user->hasChannel(c.getIndex())) {
                user->addChannel(c);
                emit userChannelCreated(*user, c);
            } else {// check for channel updates
                if (user->hasChannels()) {
                    if (channelIsOutdate(*user, c)) {
                        user->setChannelName(c.getIndex(), c.getName());
                        user->setChannelFlags(c.getIndex(), c.getFlags());
                        emit userChannelUpdated(*user, user->getChannel(c.getIndex()));
                    }
                }
            }
        } else {
            user->removeChannel(c.getIndex());
            emit userChannelRemoved(*user, c);
        }
    }
}

// check if the local stored channel and the server channel are different
bool Service::channelIsOutdate(const User &user, const UserChannel &serverChannel)
{
    if (user.getFullName() != serverChannel.getUserFullName()) {
        UserChannel userChannel = user.getChannel(serverChannel.getIndex());
        if (userChannel.getName() != serverChannel.getName())
            return true;
        if (userChannel.getFlags() != serverChannel.getFlags())
            return true;
    }
    return false;
}

// +++++++++++++ CHAT MESSAGES ++++++++++++++++++++++

void Service::process(const ServerChatMessage &msg)
{
    switch (msg.getCommand()) {
    case ChatCommandType::JOIN:
        //
        break;
    case ChatCommandType::MSG:
    {
        QString messageSender = msg.getArguments().at(0);
        QString messageText = msg.getArguments().at(1);
        emit chatMessageReceived(User(messageSender), messageText);
        break;
    }
    case ChatCommandType::PART:
    {
        QString userLeavingTheServer = msg.getArguments().at(0);
        emit userExited(User(userLeavingTheServer));
        break;
    }
    case ChatCommandType::PRIVMSG:
    {
        QString messageSender = msg.getArguments().at(0);
        QString messageText = msg.getArguments().at(1);
        emit privateMessageReceived(User(messageSender), messageText);
        break;
    }
    case ChatCommandType::TOPIC:
    {
        QString topicText = msg.getArguments().at(1);
        if (!initialized) {
            initialized = true;

            // server licence is received when the hand shake with server is started
            currentServer->setLicence(serverLicence);
            currentServer->setTopic(topicText);
            emit connectedInServer(*currentServer);
            emit chatMessageReceived(Ninjam::User(currentServer->getHostName()), topicText);
        }
        break;
    }
    case ChatCommandType::USERCOUNT:
    {
        quint32 users = msg.getArguments().at(0).toInt();
        quint32 maxUsers = msg.getArguments().at(1).toInt();
        emit userCountMessageReceived(users, maxUsers);
        break;
    }
    default:
        qCritical("chat message type not implemented");
    }
}

void Service::handle(const ServerConfigChangeNotifyMessage &msg)
{
    quint16 bpi = msg.getBpi();
    quint16 bpm = msg.getBpm();
    if (bpi != currentServer->getBpi())
        setBpi(bpi);
    if (bpm != currentServer->getBpm())
        setBpm(bpm);
}

QString Service::getCurrentServerLicence() const
{
    return serverLicence;
}

QDataStream &Ninjam::operator >>(QDataStream &stream, MessageHeader &header)
{
    stream >> header.messageTypeCode >> header.payload;
    return stream;
}
