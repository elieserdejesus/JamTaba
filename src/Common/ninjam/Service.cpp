#include "Service.h"
#include "Server.h"
#include "User.h"
#include "UserChannel.h"
#include "ServerMessages.h"
#include "ClientMessages.h"
#include "log/Logging.h"
#include <QDataStream>
#include <QDateTime>
#include <QTcpSocket>
#include "ServerMessagesHandler.h"

using namespace Ninjam;

const QStringList Service::botNames = buildBotNamesList();

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

    }

    Download()//this constructor is necessary to use Download in a QMap without pointers
    {

    }

    ~Download()
    {

    }

    inline void appendVorbisData(const QByteArray &data)
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
    initialized(false),
    socket(nullptr),
    messagesHandler(new ServerMessagesHandler(this))
{

}

Service::~Service()
{
    if(!socket)
        return;

    disconnect(socket, SIGNAL(readyRead()), this, SLOT(handleAllReceivedMessages()));
    disconnect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
               SLOT(handleSocketError(QAbstractSocket::SocketError)));
    disconnect(socket, SIGNAL(disconnected()), this, SLOT(handleSocketDisconnection()));
    disconnect(socket, SIGNAL(connected()), this, SLOT(handleSocketConnection()));

    if (socket->isValid() && socket->isOpen())
        socket->disconnectFromHost();
}

void Service::setupSocketSignals()
{
    Q_ASSERT(socket);
    connect(socket, SIGNAL(readyRead()), this, SLOT(handleAllReceivedMessages()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
            SLOT(handleSocketError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(handleSocketDisconnection()));
    connect(socket, SIGNAL(connected()), this, SLOT(handleSocketConnection()));
}

void Service::sendAudioIntervalPart(const QByteArray &GUID, const QByteArray &encodedAudioBuffer,
                                    bool isLastPart)
{
    if (!initialized)
        return;
    sendMessageToServer(ClientIntervalUploadWrite(GUID, encodedAudioBuffer, isLastPart));
}

void Service::sendAudioIntervalBegin(const QByteArray &GUID, quint8 channelIndex)
{
    if (!initialized)
        return;
    sendMessageToServer(ClientUploadIntervalBegin(GUID, channelIndex, this->userName));
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//this slot is invoked when socket receive new data
void Service::handleAllReceivedMessages()
{
    messagesHandler->handleAllMessages();
    if(needSendKeepAlive()){
        sendMessageToServer(ClientKeepAlive());
    }
}

void Service::clear(){
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
    if(currentServer){
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
    names.append("Jambot");
    names.append("ninjamer.com");
    names.append("ninbot");
    names.append("ninbot.com");
    names.append("MUTANTLAB");
    names.append("mutantlab.com");
    names.append("LiveStream");
    names.append("localhost");
    names.append("ninjamers.servebeer.com");
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
    sendMessageToServer(ChatMessage(text));
}

void Service::voteToChangeBPM(quint16 newBPM)
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
    if(!socket)
        return;

    QByteArray outBuffer;
    outBuffer << message;

    int totalDataToSend = outBuffer.size();
    int dataSended = 0;
    int bytesWrited = -1;
    do {
        bytesWrited = socket->write(outBuffer.data() + dataSended, totalDataToSend - dataSended);
        if (bytesWrited > 0)
            dataSended += bytesWrited;
    } while (dataSended < totalDataToSend && bytesWrited != -1);

    if (bytesWrited > 0) {
        socket->flush();
        lastSendTime = QDateTime::currentMSecsSinceEpoch();
    } else {
        qCritical() << "Bytes not writed in socket!";
    }

    Q_ASSERT(message.getPayload() + 5 == (uint)outBuffer.size());
}

bool Service::needSendKeepAlive() const
{
    long ellapsedSeconds = (QDateTime::currentMSecsSinceEpoch() - lastSendTime)/1000;
    return ellapsedSeconds >= serverKeepAlivePeriod;
}

void Service::process(const UserInfoChangeNotifyMessage &msg)
{
    foreach (const User &user, msg.getUsers()) {
        if (!currentServer->containsUser(user)) {
            currentServer->addUser(user);
        }

        handleUserChannels(user);

        // enable receive for all user channels
        foreach (const UserChannel &channel, user.getChannels()) {
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
        foreach (const UserChannel &channel, user.getChannels()) {
            if (channel.isActive() || (channel.getIndex() == channelIndex && receiveChannel))
                channelsMask |= 1 << channel.getIndex();
        }
        sendMessageToServer(ClientSetUserMask(userFullName, channelsMask));
    }
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
        User user = currentServer->getUser(download.getUserFullName());
        if (user.getChannel(download.getChannelIndex()).isActive()) {
            if (msg.downloadIsComplete()) {
                emit audioIntervalCompleted(user, download.getChannelIndex(), download.getVorbisData());
                downloads.remove(msg.getGUID());
            } else {
                emit audioIntervalDownloading(user, download.getChannelIndex(), msg.getEncodedAudioData().size());
            }
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
    Q_ASSERT(removedChannelIndex >= 0 && removedChannelIndex < channels.size());
    channels.removeAt(removedChannelIndex);
    sendMessageToServer(ClientSetChannel(channels));
}

void Service::process(const ServerAuthReplyMessage &msg)
{
    if (msg.userIsAuthenticated() && socket) {
        userName = msg.getNewUserName(); // replace the user name with the (possible) new name generated by the ninjam server
        sendMessageToServer(ClientSetChannel(channels));
        quint8 serverMaxChannels = msg.getMaxChannels();
        QString serverIp = socket->peerName();
        quint16 serverPort = socket->peerPort();
        currentServer.reset(new Server(serverIp, serverPort, serverMaxChannels));
    }
    // when user is not authenticated the socketErrorSlot is called and dispatch an error signal
}

void Service::startServerConnection(const QString &serverIp, int serverPort,
                                    const QString &userName, const QStringList &channels,
                                    const QString &password)
{

    clear();//reset some internal state

    if(!socket){
        socket = createSocket();//createSocket is protected and can be overrided to create a custom socket for test purpouses.
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
    User localUser = currentServer->getUser(remoteUser.getFullName());
    foreach (const UserChannel &serverChannel, remoteUser.getChannels()) {
        if (serverChannel.isActive()) {
            if (!localUser.hasChannel(serverChannel.getIndex())) {
                currentServer->addUserChannel(serverChannel);
                emit userChannelCreated(localUser, serverChannel);
            } else {// check for channel updates
                if (localUser.hasChannels()) {
                    if (channelIsOutdate(localUser, serverChannel)) {
                        currentServer->updateUserChannel(serverChannel);
                        emit userChannelUpdated(localUser, serverChannel);
                    }
                }
            }
        } else {
            currentServer->removeUserChannel(serverChannel);
            emit userChannelRemoved(localUser, serverChannel);
        }
    }
}

// check if the local stored channel and the server channel are different
bool Service::channelIsOutdate(const User &user, const UserChannel &serverChannel)
{
    if (user.getFullName() == serverChannel.getUserFullName()) {
        if(user.hasChannel(serverChannel.getIndex())){
            UserChannel userChannel = user.getChannel(serverChannel.getIndex());
            return userChannel.getName() != serverChannel.getName();
        }
    }
    return false;
}

// +++++++++++++ CHAT MESSAGES ++++++++++++++++++++++

void Service::process(const ServerChatMessage &msg)
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
        emit chatMessageReceived(User(messageSender), messageText);
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
            emit serverTopicMessageReceived(topicText);
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

void Service::process(const ServerConfigChangeNotifyMessage &msg)
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
