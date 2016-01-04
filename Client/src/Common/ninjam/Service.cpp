#include "Service.h"
#include "Server.h"
#include "User.h"
#include "protocol/ServerMessages.h"
#include "protocol/ClientMessages.h"
#include <algorithm>
#include <QHostAddress>
#include <QDateTime>
#include <QDataStream>

#include <QThread>
#include <cassert>

#include "log/Logging.h"

using namespace Ninjam;

const QStringList Service::botNames = buildBotNamesList();

// ++++++++++++++++++++++++++++

class Service::Download
{
private:
    quint8 channelIndex;
    QString userFullName;
    QString GUID;
    QByteArray vorbisData;
    static int instances;
public:
    Download(QString userFullName, quint8 channelIndex, QString GUID) :
        channelIndex(channelIndex),
        userFullName(userFullName),
        GUID(GUID)
    {
        instances++;
        qCDebug(jtNinjamProtocol) << "Download constructor instances: " << instances;
    }

    Download()
    {
        qCritical() << "using the default constructor!";
    }

    ~Download()
    {
        instances--;
        qCDebug(jtNinjamProtocol) << "Download destructor instances: " << instances;
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

    inline QString getGUI() const
    {
        return GUID;
    }

    inline QByteArray getVorbisData() const
    {
        return vorbisData;
    }
};
int Ninjam::Service::Download::instances = 0;

// ++++++++++++++++++++++++++++++++++++++++

Service::Service() :
    lastSendTime(0),
    initialized(false)
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(socketReadSlot()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
            SLOT(socketErrorSlot(QAbstractSocket::SocketError)));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(socketDisconnectSlot()));
    connect(&socket, SIGNAL(connected()), this, SLOT(socketConnectedSlot()));
}

Service::~Service()
{
    disconnect(&socket, SIGNAL(readyRead()), this, SLOT(socketReadSlot()));
    disconnect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
               SLOT(socketErrorSlot(QAbstractSocket::SocketError)));
    disconnect(&socket, SIGNAL(disconnected()), this, SLOT(socketDisconnectSlot()));
    disconnect(&socket, SIGNAL(connected()), this, SLOT(socketConnectedSlot()));

    if (socket.isValid() && socket.isOpen())
        socket.disconnectFromHost();
}

void Service::sendAudioIntervalPart(QByteArray GUID, QByteArray encodedAudioBuffer, bool isLastPart)
{
    qCDebug(jtNinjamProtocol) << "sending audio interval part";
    if (!initialized)
        return;
    sendMessageToServer(ClientIntervalUploadWrite(GUID, encodedAudioBuffer, isLastPart));
}

void Service::sendAudioIntervalBegin(QByteArray GUID, quint8 channelIndex)
{
    qCDebug(jtNinjamProtocol) << "sending audio interval begin";
    if (!initialized)
        return;
    sendMessageToServer(ClientUploadIntervalBegin(GUID, channelIndex, this->userName));
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ServerMessage *Service::createServerMessage(quint8 messageTypeCode, quint32 payload)
{
    ServerMessageType messageType = static_cast<ServerMessageType>(messageTypeCode);
    switch (messageType) {
    case ServerMessageType::AUTH_CHALLENGE:
        return new ServerAuthChallengeMessage(payload);

    case ServerMessageType::AUTH_REPLY:
        return new ServerAuthReplyMessage(payload);

    case ServerMessageType::USER_INFO_CHANGE_NOTIFY:
        return new UserInfoChangeNotifyMessage(payload);

    case ServerMessageType::SERVER_CONFIG_CHANGE_NOTIFY:
        return new ServerConfigChangeNotifyMessage(payload);

    case ServerMessageType::CHAT_MESSAGE:
        return new ServerChatMessage(payload);

    case ServerMessageType::KEEP_ALIVE:
        return new ServerKeepAliveMessage();

    case ServerMessageType::DOWNLOAD_INTERVAL_BEGIN:
        return new DownloadIntervalBegin(payload);

    case ServerMessageType::DOWNLOAD_INTERVAL_WRITE:
        return new DownloadIntervalWrite(payload);
    }
    qCritical() << "Error creating a ServerMessage instance for the type " << messageTypeCode
                << " payload: " << payload;
    return nullptr;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Service::socketReadSlot()
{
    if (socket.bytesAvailable() < 5) {
        qCDebug(jtNinjamProtocol) << "not have enough bytes to read message header (5 bytes)";
        return;
    }

    qCDebug(jtNinjamProtocol) << "socket read slot";

    QDataStream stream(&socket);
    stream.setByteOrder(QDataStream::LittleEndian);

    static quint8 messageTypeCode;
    static quint32 payload;
    while (socket.bytesAvailable() >= 5) {// this loop consume all messages. Every ninjam message contains a 5 bytes header.
        if (!lastMessageWasIncomplete)
            stream >> messageTypeCode >> payload;
        }
        if (socket.bytesAvailable() >= (int)payload) {// message payload is available to read
            lastMessageWasIncomplete = false;

            ServerMessage *message = createServerMessage(messageTypeCode, payload);
            if (message) {
                message->readFrom(stream);
                invokeMessageHandler(message);
                delete message;
            }
            if (needSendKeepAlive())
                sendMessageToServer(ClientKeepAlive());
        } else {
            qCDebug(jtNinjamProtocol) << "incomplete message!";
            lastMessageWasIncomplete = true;
            break;// skip and wait until receive more bytes
        }
    }
}

void Service::socketErrorSlot(QAbstractSocket::SocketError e)
{
    Q_UNUSED(e);
    qCWarning(jtNinjamProtocol) << QString(socket.errorString());
    currentServer.reset(nullptr);
    emit error(socket.errorString());
}

void Service::socketConnectedSlot()
{
    qCDebug(jtNinjamProtocol) << "socket connected on " << socket.peerName();
}

void Service::socketDisconnectSlot()
{
    this->initialized = false;
    qCDebug(jtNinjamProtocol) << "socket disconnected from " << socket.peerName();
    emit disconnectedFromServer(*currentServer);
}

bool Service::isBotName(QString userName)
{
    userName = userName.trimmed();
    return botNames.contains(userName);
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
        return newUserName;
    qCritical() << "not initialized, newUserName is not available!";
    return "";
}

float Service::getIntervalPeriod()
{
    if (currentServer)
        return (float)60000 / currentServer->getBpm() * currentServer->getBpi();
    return 0;
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

void Service::sendChatMessageToServer(QString message)
{
    sendMessageToServer(ChatMessage(message));
}

void Service::sendMessageToServer(const ClientMessage &message)
{
    QByteArray outBuffer;
    message.serializeTo(outBuffer);

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
    long ellapsedSeconds = (int)(QDateTime::currentMSecsSinceEpoch() - lastSendTime)/1000;
    return ellapsedSeconds >= serverKeepAlivePeriod;
}

void Service::handle(const UserInfoChangeNotifyMessage &msg)
{
    QSet<QString> users = QSet<QString>::fromList(msg.getUsersNames());
    foreach (QString userFullName, users) {
        if (!currentServer->containsUser(userFullName)) {
            User newUser(userFullName);
            currentServer->addUser(newUser);
            emit userEnterInTheJam(newUser);
        }
        handleUserChannels(userFullName, msg.getUserChannels(userFullName));
    }

    // enable new users channels
    sendMessageToServer(ClientSetUserMask(msg.getUsersNames()));
}

void Service::handle(const DownloadIntervalBegin &msg)
{
    if (!msg.downloadShouldBeStopped() && msg.isValidOggDownload()) {
        quint8 channelIndex = msg.getChannelIndex();
        QString userFullName = msg.getUserName();
        QString GUID = msg.getGUID();
        downloads.insert(GUID, Download(userFullName, channelIndex, GUID));
    }
}

void Service::handle(const DownloadIntervalWrite &msg)
{
    if (downloads.contains(msg.getGUID())) {
        Download &download = downloads[msg.getGUID()];
        download.appendVorbisData(msg.getEncodedAudioData());
        User *user = currentServer->getUser(download.getUserFullName());
        if (msg.downloadIsComplete()) {
            emit audioIntervalCompleted(*user, download.getChannelIndex(), download.getVorbisData());
            downloads.remove(msg.getGUID());
        } else {
            emit audioIntervalDownloading(*user, download.getChannelIndex(), msg.getEncodedAudioData().size());
        }
    } else {
        qCritical("GUID is not in map!");
    }
}

void Service::handle(const ServerKeepAliveMessage & /*msg*/)
{
    sendMessageToServer(ClientKeepAlive());
}

void Service::handle(const ServerAuthChallengeMessage &msg)
{
    ClientAuthUserMessage msgAuthUser(userName, msg.getChallenge(),
                                      msg.getProtocolVersion(), password);
    sendMessageToServer(msgAuthUser);
    serverLicence = msg.getLicenceAgreement();
    serverKeepAlivePeriod = msg.getServerKeepAlivePeriod();
}

void Service::sendNewChannelsListToServer(QStringList channelsNames)
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

void Service::handle(const ServerAuthReplyMessage &msg)
{
    if (msg.userIsAuthenticated()) {
        this->newUserName = msg.getNewUserName();
        sendMessageToServer(ClientSetChannel(channels));
        quint8 serverMaxChannels = msg.getMaxChannels();
        QString serverIp = socket.peerName();
        quint16 serverPort = socket.peerPort();
        currentServer.reset(new Server(serverIp, serverPort, serverMaxChannels));
    }
    // when user is not authenticated the socketErrorSlot is called and dispatch an error signal
}

void Service::startServerConnection(QString serverIp, int serverPort, QString userName,
                                    QStringList channels, QString password)
{
    initialized = lastMessageWasIncomplete = false;
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
    if (currentServer == nullptr)
        throw ("currentServer == null");
    if (currentServer->setBpm(newBpm) && initialized)
        emit serverBpmChanged(currentServer->getBpm());
}

void Service::setBpi(quint16 bpi)
{
    if (currentServer == nullptr)
        throw ("currentServer == null");
    quint16 lastBpi = currentServer->getBpi();
    if (currentServer->setBpi(bpi) && initialized)
        emit serverBpiChanged(currentServer->getBpi(), lastBpi);
}

// +++++++++++++ SERVER MESSAGE HANDLERS +++++++++++++=
void Service::handleUserChannels(QString userFullName, QList<UserChannel> channelsInTheServer)
{
    // check for new channels
    User *user = this->currentServer->getUser(userFullName);
    foreach (UserChannel c, channelsInTheServer) {
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

// verifica se o canal do usuario esta diferente do canal que veio do servidor
bool Service::channelIsOutdate(const User &user, const UserChannel &serverChannel)
{
    if (user.getFullName() != serverChannel.getUserFullName())
        throw ("The user in channel is illegal!");
    UserChannel userChannel = user.getChannel(serverChannel.getIndex());
    if (userChannel.getName() != serverChannel.getName())
        return true;
    if (userChannel.getFlags() != serverChannel.getFlags())
        return true;
    return false;
}

// +++++++++++++ CHAT MESSAGES ++++++++++++++++++++++

void Service::handle(const ServerChatMessage &msg)
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
        emit userLeaveTheJam(User(userLeavingTheServer));
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

            currentServer->setTopic(topicText);

            // server licence is received when the hand shake with server is started
            currentServer->setLicence(serverLicence);
            emit connectedInServer(*currentServer);
            emit chatMessageReceived(Ninjam::User(currentServer->getHostName()), topicText);
        }
        break;
    }
    case ChatCommandType::USERCOUNT:
    {
        int users = msg.getArguments().at(0).toInt();
        int maxUsers = msg.getArguments().at(1).toInt();
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

void Service::invokeMessageHandler(ServerMessage *message)
{
    switch (message->getMessageType()) {
    case ServerMessageType::AUTH_CHALLENGE:
        handle((ServerAuthChallengeMessage &)*message);
        break;
    case ServerMessageType::AUTH_REPLY:
        handle((ServerAuthReplyMessage &)*message);
        break;
    case ServerMessageType::SERVER_CONFIG_CHANGE_NOTIFY:
        handle((ServerConfigChangeNotifyMessage &)*message);
        break;
    case ServerMessageType::USER_INFO_CHANGE_NOTIFY:
        handle((UserInfoChangeNotifyMessage &)*message);
        break;
    case ServerMessageType::CHAT_MESSAGE:
        handle((ServerChatMessage &)*message);
        break;
    case ServerMessageType::KEEP_ALIVE:
        handle((ServerKeepAliveMessage &)*message);
        break;
    case ServerMessageType::DOWNLOAD_INTERVAL_BEGIN:
        handle((DownloadIntervalBegin &)*message);
        break;
    case ServerMessageType::DOWNLOAD_INTERVAL_WRITE:
        handle((DownloadIntervalWrite &)*message);
        break;

    default:
        qCCritical(jtNinjamProtocol) << "receive a not implemented yet message!";
    }
}

QString Service::getCurrentServerLicence() const
{
    return serverLicence;
}
