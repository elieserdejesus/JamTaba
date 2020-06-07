#include "Service.h"
#include "ServerInfo.h"
#include "User.h"
#include "UserChannel.h"
#include "ServerMessages.h"
#include "ClientMessages.h"
#include "ServerMessagesHandler.h"
#include "log/Logging.h"

#include <QDataStream>
#include <QDateTime>
#include <QTcpSocket>

using namespace ninjam::client;

const QStringList Service::botNames = buildBotNamesList();

// ---------------------------------------------------------------------

/**
    This is a nested class used to bind the downloaded audio data (encoded in ogg vorbis) with a GUID (global unique ID),
    an user name and a channel index (users can use more than one channel). When a dowload is finished (the ninjam audio interval is
    fully downloaded) we need emit a signal, and in this moment we need the user name and the channel index.
*/

class Service::Download
{

public:
    Download(const QString &userFullName, quint8 channelIndex, const QByteArray &GUID, bool audio = true) :
        channelIndex(channelIndex),
        userFullName(userFullName),
        GUID(GUID),
        containsAudio(audio)
    {

    }

    Download() // this constructor is necessary to use Download in a QMap without pointers
    {
        //
    }

    ~Download()
    {
        //
    }

    inline bool isAudio() const
    {
        return containsAudio;
    }

    inline void appendEncodedData(const QByteArray &data)
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

    inline QByteArray getEncodedData() const
    {
        return vorbisData;
    }

private:
    quint8 channelIndex;
    QString userFullName;
    QByteArray GUID; // Global Unique ID
    QByteArray vorbisData;
    bool containsAudio; // audio or video?
};

// ++++++++++++++++++++++++++++++++++++++++

Service::Service() :
    lastSendTime(0),
    initialized(false),
    socket(nullptr),
    messagesHandler(new ServerMessagesHandler(this)),
    serverKeepAlivePeriod(30)
{

}

Service::~Service()
{
    if(!socket)
        return;

    disconnect(socket, SIGNAL(readyRead()), this, SLOT(handleAllReceivedMessages()));
    disconnect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError)));
    disconnect(socket, SIGNAL(disconnected()), this, SLOT(handleSocketDisconnection()));
    disconnect(socket, SIGNAL(connected()), this, SLOT(handleSocketConnection()));

    if (socket->isValid() && socket->isOpen())
        socket->disconnectFromHost();
}

void Service::setupSocketSignals()
{
    Q_ASSERT(socket);
    connect(socket, SIGNAL(readyRead()), this, SLOT(handleAllReceivedMessages()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(handleSocketDisconnection()));
    connect(socket, SIGNAL(connected()), this, SLOT(handleSocketConnection()));

    connect(socket, &QTcpSocket::bytesWritten, [&](quint64 bytesWritten){
        totalUploadMeasurer.addTransferedBytes(bytesWritten);
    });
}

void Service::sendIntervalPart(const QByteArray &GUID, const QByteArray &encodedData, bool isLastPart)
{
    if (!initialized)
        return;

    auto msg = UploadIntervalWrite(GUID, encodedData, isLastPart);
    sendMessageToServer(msg);
}

void Service::sendIntervalBegin(const QByteArray &GUID, quint8 channelIndex, bool isAudioInterval)
{
    if (!initialized)
        return;

    auto msg = UploadIntervalBegin(GUID, channelIndex, isAudioInterval);
    sendMessageToServer(msg);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//this slot is invoked when socket receive new data
void Service::handleAllReceivedMessages()
{
    qint64 bytesAvailable = socket->bytesAvailable();

    messagesHandler->handleAllMessages();
    if (needSendKeepAlive()) {
        sendMessageToServer(ClientKeepAlive());
    }

    qint64 bytesProcessed = bytesAvailable - (bytesAvailable - socket->bytesAvailable());

    totalDownloadMeasurer.addTransferedBytes(bytesProcessed);
}

void Service::clear()
{
    initialized = false;
    currentServer.reset();
}

void Service::handleSocketError(QAbstractSocket::SocketError e)
{
    Q_UNUSED(e);
    Q_ASSERT(socket);//just in case
    qCWarning(jtNinjamProtocol) << QString(socket->errorString());
    clear();
    emit error(socket->errorString());
}

void Service::handleSocketConnection()
{
    Q_ASSERT(socket);
    qCDebug(jtNinjamProtocol) << "socket connected on " << socket->peerName();
}

void Service::handleSocketDisconnection()
{
    Q_ASSERT(socket);
    qCDebug(jtNinjamProtocol) << "socket disconnected from " << socket->peerName();
    if (currentServer) {
        emit disconnectedFromServer(*currentServer);
    }
    clear();
}

bool Service::isBotName(const QString &userName)
{
    return botNames.contains(userName.trimmed());
}

QStringList Service::buildBotNamesList()
{
    QStringList names;
    names.append(QStringLiteral("Jambot"));
    names.append(QStringLiteral("ninjamer.com"));
    names.append(QStringLiteral("ninbot"));
    names.append(QStringLiteral("ninbot.com"));
    names.append(QStringLiteral("MUTANTLAB"));
    names.append(QStringLiteral("mutantlab.com"));
    names.append(QStringLiteral("LiveStream"));
    names.append(QStringLiteral("localhost"));
    names.append(QStringLiteral("dojcbot"));
    names.append(QStringLiteral("DOJCbot"));
    names.append(QStringLiteral("server@server"));
    return names;
}

QString Service::getConnectedUserName() const
{
    if (initialized)
        return userName;
    qCritical() << "not initialized, newUserName is not available!";
    return "";
}

float Service::getIntervalPeriod() const
{
    if (currentServer)
        return 60000.0f / currentServer->getBpm() * currentServer->getBpi();

    return 0.0f;
}

void Service::voteToChangeBPI(quint16 newBPI)
{
    QString text = "!vote bpi " + QString::number(newBPI);
    sendMessageToServer(ClientToServerChatMessage::buildPublicMessage(text));
}

void Service::voteToChangeBPM(quint16 newBPM)
{
    QString text = "!vote bpm " + QString::number(newBPM);
    sendMessageToServer(ClientToServerChatMessage::buildPublicMessage(text));
}

void Service::sendPrivateChatMessage(const QString &message, const QString &destinationUser)
{
    sendMessageToServer(ClientToServerChatMessage::buildPrivateMessage(message, destinationUser));
}

void Service::sendPublicChatMessage(const QString &message)
{
    sendMessageToServer(ClientToServerChatMessage::buildPublicMessage(message));
}

void Service::sendAdminCommand(const QString &message)
{
    auto msg = ClientToServerChatMessage::buildAdminMessage(message);
    sendMessageToServer(msg);
}

void Service::sendMessageToServer(const ClientMessage &message)
{
    if (!socket)
        return;

    message.serializeTo(socket);

    socket->flush();
    lastSendTime = QDateTime::currentMSecsSinceEpoch();
}

bool Service::needSendKeepAlive() const
{
    long ellapsedSeconds = (QDateTime::currentMSecsSinceEpoch() - lastSendTime)/1000;
    return ellapsedSeconds >= serverKeepAlivePeriod;
}

void Service::process(const UserInfoChangeNotifyMessage &msg)
{
    //msg.printDebug(qDebug());

    for (const User &user : msg.getUsers()) {
        if (!currentServer->containsUser(user)) {
            currentServer->addUser(user);
        }

        handleUserChannels(user);

        // enable receive for all user channels
        for (const UserChannel &channel : user.getChannels()) {
            setChannelReceiveStatus(user.getFullName(), channel.getIndex(), true);
        }
    }
}

void Service::setChannelReceiveStatus(const QString &userFullName, quint8 channelIndex, bool receiveChannel)
{
    if (currentServer->containsUser(userFullName)) {
        currentServer->updateUserChannelReceiveStatus(userFullName, channelIndex, receiveChannel);

        User user = currentServer->getUser(userFullName);
        quint32 channelsMask = 0;
        for (const UserChannel &channel : user.getChannels()) {
            if (channel.isActive() || (channel.getIndex() == channelIndex && receiveChannel))
                channelsMask |= 1 << channel.getIndex();
        }
        sendMessageToServer(ClientSetUserMask(userFullName, channelsMask));
    }
}

void Service::process(const DownloadIntervalBegin &msg)
{
    if (!msg.shouldBeStopped() && (msg.isAudio() || msg.isVideo())) {
        quint8 channelIndex = msg.getChannelIndex();
        QString userFullName = msg.getUserName();
        QByteArray GUID = msg.getGUID();
        downloads.insert(GUID, Download(userFullName, channelIndex, GUID, msg.isAudio()));
    }
}

void Service::process(const DownloadIntervalWrite &msg)
{
    if (downloads.contains(msg.getGUID())) {
        Download &download = downloads[msg.getGUID()];

        bool isFirstPart = download.getEncodedData().isEmpty();

        download.appendEncodedData(msg.getEncodedData());

        auto &measurer = channelDownloadMeasurers[download.getUserFullName()][download.getChannelIndex()];
        auto bytesReceived = msg.getEncodedData().size();
        measurer.addTransferedBytes(bytesReceived);

        User user = currentServer->getUser(download.getUserFullName());

        if (download.isAudio()) {
            if (user.getChannel(download.getChannelIndex()).isActive()) {
                if (msg.downloadIsComplete()) {
                    emit audioIntervalDownloading(user, download.getChannelIndex(), msg.getEncodedData(), isFirstPart, true); // the last chunk
                    emit audioIntervalCompleted(user, download.getChannelIndex(), download.getEncodedData()); // full interval
                    downloads.remove(msg.getGUID());
                }
                else
                    emit audioIntervalDownloading(user, download.getChannelIndex(), msg.getEncodedData(), isFirstPart, false);
             }
        }
        else if (msg.downloadIsComplete()) { // download is video
            emit videoIntervalCompleted(user, download.getEncodedData());
            downloads.remove(msg.getGUID());
        }
    } else {
        qCritical() << "GUID is not in map!";
    }
}

void Service::process(const ServerKeepAliveMessage &)
{
    sendMessageToServer(ClientKeepAlive());
}

void Service::process(const AuthChallengeMessage &msg)
{
    ClientAuthUserMessage msgAuthUser(userName, msg.getChallenge(),
                                      msg.getProtocolVersion(), password);
    sendMessageToServer(msgAuthUser);
    serverLicence = msg.getLicenceAgreement();
    serverKeepAlivePeriod = msg.getServerKeepAlivePeriod();
}

void Service::sendNewChannelsListToServer(const QList<ChannelMetadata> &channels)
{
    this->channels = channels;

    ClientSetChannel msg;
    for (auto channelMetadata : channels) {
        msg.addChannel(channelMetadata.name, ClientSetChannel::toFlags(channelMetadata.voiceChatActivated));
    }

    sendMessageToServer(msg);
}

void Service::sendRemovedChannelIndex(int removedChannelIndex)
{
    Q_ASSERT(removedChannelIndex >= 0 && removedChannelIndex < channels.size());

    channels.removeAt(removedChannelIndex);

    // send only remaining channels to server, the removed channel will be excluded in the clients
    sendNewChannelsListToServer(channels);
}

void Service::process(const AuthReplyMessage &msg)
{
    if (msg.userIsAuthenticated() && socket) {
        userName = msg.getNewUserName(); // replace the user name with the (possible) new name generated by the ninjam server
        sendMessageToServer(ClientSetChannel(channels));
        quint8 serverMaxChannels = msg.getMaxChannels();
        QString serverIp = socket->peerName();
        quint16 serverPort = socket->peerPort();
        currentServer.reset(new ServerInfo(serverIp, serverPort, serverMaxChannels));

        if (!initialized) {
            initialized = true;

            // server licence is received when the hand shake with server is started
            currentServer->setLicence(serverLicence);
            emit connectedInServer(*currentServer);
        }

    }
    // when user is not authenticated the socketErrorSlot is called and dispatch an error signal
}

void Service::startServerConnection(const QString &serverIp, int serverPort,
                                    const QString &userName, const QList<ChannelMetadata> &channels,
                                    const QString &password)
{

    clear(); // reset some internal state

    if (!socket) {
        socket = createSocket(); // createSocket is protected and can be overrided to create a custom socket for test purpouses.

        socket->setSocketOption(QAbstractSocket::LowDelayOption, 1); // low delay socket, disabling Nagle's Algorithm

        setupSocketSignals();
    }
    Q_ASSERT(socket);

    this->userName = userName;
    this->password = password;
    this->channels = channels;

    messagesHandler->initialize(socket);

    // check state to avoid a bug if user try enter in a server using double click in the button
    if (socket->state() == QTcpSocket::UnconnectedState)
        socket->connectToHost(serverIp, serverPort);
}

void Service::disconnectFromServer(bool emitDisconnectedSignal)
{
    if (socket && socket->isOpen()) {
        qCDebug(jtNinjamProtocol) << "disconnecting from " << socket->peerName();
        if (!emitDisconnectedSignal)
            socket->blockSignals(true); // avoid generate events when disconnecting/exiting
        socket->disconnectFromHost();
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
void Service::handleUserChannels(const User &remoteUser)
{
    // check for new channels
    const User &localUser = currentServer->getUser(remoteUser.getFullName());
    for (const UserChannel &serverChannel : remoteUser.getChannels()) {
        if (serverChannel.isActive()) {
            if (!localUser.hasChannel(serverChannel.getIndex())) {
                currentServer->addUserChannel(remoteUser.getFullName(), serverChannel);
                emit userChannelCreated(localUser, serverChannel);
            } else { // check for channel updates
                if (localUser.hasChannels()) {
                    if (channelIsOutdate(localUser, serverChannel)) {
                        currentServer->updateUserChannel(remoteUser.getFullName(), serverChannel);
                        emit userChannelUpdated(localUser, serverChannel);
                    }
                }
            }
        } else {
            currentServer->removeUserChannel(remoteUser.getFullName(), serverChannel);
            emit userChannelRemoved(localUser, serverChannel);
        }
    }
}

// check if the local stored channel and the server channel are different
bool Service::channelIsOutdate(const User &user, const UserChannel &serverChannel)
{
    //if (user.getFullName() == serverChannel.getUserFullName()) {
        if (user.hasChannel(serverChannel.getIndex())) {
            UserChannel userChannel = user.getChannel(serverChannel.getIndex());
            return userChannel.getName() != serverChannel.getName()
                    || userChannel.getFlags() != serverChannel.getFlags();
        }
    //}
    return false;
}

// +++++++++++++ CHAT MESSAGES ++++++++++++++++++++++

void Service::process(const ServerToClientChatMessage &msg)
{
    switch (msg.getCommand()) {
    case ChatCommandType::JOIN:
    {
        QString userName = msg.getArguments().at(0);
        if (currentServer)
            currentServer->addUser(User(userName));
        emit userEntered(User(userName));
        break;
    }
    case ChatCommandType::MSG:
    {
        QString messageSender = msg.getArguments().at(0);
        QString messageText = msg.getArguments().at(1);
        emit publicChatMessageReceived(User(messageSender), messageText);
        break;
    }
    case ChatCommandType::PART:
    {
        QString userLeavingTheServer = msg.getArguments().at(0);
        if (currentServer)
            currentServer->removeUser(userLeavingTheServer);
        emit userExited(User(userLeavingTheServer));
        break;
    }
    case ChatCommandType::PRIVMSG:
    {
        QString messageSender = msg.getArguments().at(0);
        QString messageText = msg.getArguments().at(1);

        auto isServerPrivateMessage = messageSender == "*";
        if (isServerPrivateMessage) {
            if (!messageText.isEmpty()) // discarding empty messages
                emit publicChatMessageReceived(User("server@server"), messageText);
        }
        else {
            emit privateChatMessageReceived(User(messageSender), messageText);
        }
        break;
    }
    case ChatCommandType::TOPIC:
    {
        if (!currentServer)
            return;

        QString topicText = msg.getArguments().at(1);
        currentServer->setTopic(topicText);

        emit serverTopicMessageReceived(topicText);

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

void Service::process(const ConfigChangeNotifyMessage &msg)
{
    quint16 bpi = msg.getBpi();
    quint16 bpm = msg.getBpm();

    if (bpi != currentServer->getBpi())
        setBpi(bpi);

    if (bpm != currentServer->getBpm())
        setBpm(bpm);
}

QTcpSocket * Service::createSocket()
{
    return new QTcpSocket(this);
}

QString Service::getCurrentServerLicence() const
{
    return serverLicence;
}

long Service::getDownloadTransferRate(const QString userFullName, quint8 channelIndex) const
{
    const auto &measurer = channelDownloadMeasurers[userFullName][channelIndex];
    return measurer.getTransferRate();
}
