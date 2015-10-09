#include "Service.h"
#include "Server.h"
#include "User.h"
#include "protocol/ServerMessageParser.h"
#include "protocol/ServerMessages.h"
#include "protocol/ClientMessages.h"
#include <algorithm>
#include <QHostAddress>
#include <QDateTime>
#include <QDataStream>

#include <QThread>

#include <cassert>

Q_LOGGING_CATEGORY(ninjamService, "ninjam.service")

using namespace Ninjam;

std::unique_ptr<Service> Service::serviceInstance;

const QStringList Service::botNames = buildBotNamesList();

//++++++++++++++++++++++++++++

//class Download;

class Service::Download {
private:
    quint8 channelIndex;
    QString userFullName;
    QString GUID;
    QByteArray vorbisData;
    static int instances;
public:
    Download(QString userFullName, quint8 channelIndex, QString GUID)
        :channelIndex(channelIndex), userFullName(userFullName), GUID(GUID){
        instances++;
        qCDebug(ninjamService) << "Download constructor instances: " << instances;
    }
    Download(){
        qCritical() << "using the default constructor!";
    }

    ~Download(){
        instances--;
        qCDebug(ninjamService) << "Download destructor instances: " << instances;
    }

    inline void appendVorbisData(QByteArray data){ this->vorbisData.append(data); }

    inline quint8 getChannelIndex() const{return channelIndex;}
    inline QString getUserFullName() const{return userFullName;}
    inline QString getGUI() const{return GUID;}
    inline QByteArray getVorbisData() const{return vorbisData;}

};
int Ninjam::Service::Download::instances = 0;


//++++++++++++++++++++++++++++++++++++++++


Service::Service()
    :
      lastSendTime(0),
//      running(false),
      initialized(false)

{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(socketReadSlot()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketErrorSlot(QAbstractSocket::SocketError)));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(socketDisconnectSlot()));
    connect(&socket, SIGNAL(connected()), this, SLOT(socketConnectedSlot()));
}

Service::~Service(){
    qCDebug(ninjamService) << "NinjamService destructor";
    disconnect(&socket, SIGNAL(readyRead()), this, SLOT(socketReadSlot()));
    disconnect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketErrorSlot(QAbstractSocket::SocketError)));
    disconnect(&socket, SIGNAL(disconnected()), this, SLOT(socketDisconnectSlot()));
    disconnect(&socket, SIGNAL(connected()), this, SLOT(socketConnectedSlot()));

    if(socket.isOpen()){
        socket.disconnectFromHost();
    }
}

void Service::sendAudioIntervalPart(QByteArray GUID, QByteArray encodedAudioBuffer, bool isLastPart){
    qCDebug(ninjamService) << "sending audio interval part";
    if(!initialized){
        return;
    }
    ClientIntervalUploadWrite msg(GUID, encodedAudioBuffer, isLastPart);
    sendMessageToServer(&msg);
}

void Service::sendAudioIntervalBegin(QByteArray GUID, quint8 channelIndex){
    qCDebug(ninjamService) << "sending audio interval begin";
    if(!initialized){
        return;
    }
    ClientUploadIntervalBegin msg(GUID, channelIndex, this->userName);
    sendMessageToServer(&msg);
}

//void Service::stopTransmitting(char* GUID[], quint8 userChannel){
//    enqueueMessageToSend(new ClientUploadIntervalBegin(userChannel, newUserName));
//}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Service::socketReadSlot(){
    if(socket.bytesAvailable() < 5){
        qCDebug(ninjamService) << "not have enough bytes to read message header (5 bytes)";
        return;
    }

    QDataStream stream(&socket);
    stream.setByteOrder(QDataStream::LittleEndian);

    static quint8 messageTypeCode;
    static quint32 payloadLenght;
    static bool lastMessageWasIncomplete = false;
    while (socket.bytesAvailable() >= 5){//consume all messages
        if(!lastMessageWasIncomplete){
            stream >> messageTypeCode >> payloadLenght;
        }
        if (socket.bytesAvailable() >= (int)payloadLenght) {//message payload is available to read
            lastMessageWasIncomplete = false;
            const Ninjam::ServerMessage& message = ServerMessageParser::parse(static_cast<ServerMessageType>(messageTypeCode), stream, payloadLenght) ;
            //qCDebug(ninjamService) << message;
            invokeMessageHandler(message);
            if(needSendKeepAlive()){
                ClientKeepAlive clientKeepAliveMessage;
                sendMessageToServer((ClientMessage*)&clientKeepAliveMessage);
            }
        }
        else{
            qCDebug(ninjamService) << "incomplete message!";
            lastMessageWasIncomplete = true;
            break;
        }
    }
}

void Service::socketErrorSlot(QAbstractSocket::SocketError e)
{
    Q_UNUSED(e);
    qCWarning(ninjamService) << QString(socket.errorString());
    currentServer.reset(nullptr);
    emit error(socket.errorString());
}

void Service::socketConnectedSlot(){
    qCDebug(ninjamService) << "socket connected on " << socket.peerName();

}

void Service::socketDisconnectSlot(){
    this->initialized = false;
    qCDebug(ninjamService) << "socket disconnected from " << socket.peerName();
    emit disconnectedFromServer(*currentServer);
}


Service* Service::getInstance() {
    if(!serviceInstance){
        serviceInstance = std::unique_ptr<Service>(new Service());
    }
    return serviceInstance.get();
}

bool Service::isBotName(QString userName) {
    userName = userName.trimmed();
    return botNames.contains(userName);
}

QStringList Service::buildBotNamesList(){
    QStringList names;
    names.append("Jambot");
    names.append("ninjamer.com");
    names.append("ninbot");
    names.append("ninbot.com");
    names.append("MUTANTLAB");
    names.append("LiveStream");
    names.append("localhost");
    return names;
}

QString Service::getConnectedUserName() {
    if (initialized) {
        return newUserName;
    }
    qCritical() << "not initialized, newUserName is not available!";
    return "";
}

float Service::getIntervalPeriod() {
    if (currentServer ) {
        return (float)60000 / currentServer->getBpm() * currentServer->getBpi();
    }
    return 0;
}


void Service::voteToChangeBPI(int newBPI){
    QString text = "!vote bpi " + QString::number(newBPI);
    ChatMessage message(text);
    sendMessageToServer(&message);
}

void Service::voteToChangeBPM(int newBPM){
    QString text = "!vote bpm " + QString::number(newBPM);
    ChatMessage message(text);
    sendMessageToServer(&message);
}

void Service::sendChatMessageToServer(QString message){
    ChatMessage msg(message);
    sendMessageToServer(&msg);
}

void Service::sendMessageToServer(ClientMessage *message){
    //qCDebug(ninjamService) << *message;
    QByteArray outBuffer;
    message->serializeTo(outBuffer);

    int totalDataToSend = outBuffer.size();
    int dataSended = 0;
    int bytesWrited = -1;
    do{
        bytesWrited =  socket.write(outBuffer.data() + dataSended, totalDataToSend - dataSended);
        if(bytesWrited > 0){
            dataSended += bytesWrited;
        }
    }
    while(dataSended < totalDataToSend && bytesWrited != -1);

    if(bytesWrited > 0){
        socket.flush();
        lastSendTime = QDateTime::currentMSecsSinceEpoch();
    }
    else{
        qCCritical(ninjamService) << "Bytes not writed in socket!";
    }

    if((int)message->getPayload() + 5 != outBuffer.size()){
        qCWarning(ninjamService()) << "(int)message->getPayload() + 5: " << ((int)message->getPayload() + 5) << "outbuffer.size():" << outBuffer.size();
    }

}

bool Service::needSendKeepAlive() const{
    long ellapsedSeconds = (int)(QDateTime::currentMSecsSinceEpoch() - lastSendTime)/1000;
    return ellapsedSeconds >= serverKeepAlivePeriod;
}

void Service::handle(const UserInfoChangeNotifyMessage& msg) {
    //QMap<NinjamUser*, QList<NinjamUserChannel*>> allUsersChannels = msg.getUsersChannels();
    QSet<QString> users = QSet<QString>::fromList( msg.getUsersNames());
    foreach (QString userFullName , users) {
        if (!currentServer->containsUser(userFullName)) {
            User newUser(userFullName);
            currentServer->addUser( newUser );
            emit userEnterInTheJam(newUser);
        }
        handleUserChannels(userFullName, msg.getUserChannels(userFullName));
    }

    ClientSetUserMask setUserMask(msg.getUsersNames());
    sendMessageToServer( &setUserMask );//enable new users channels
}

void Service::handle(const DownloadIntervalBegin& msg){
    if (!msg.downloadShouldBeStopped() && msg.isValidOggDownload()) {
        quint8 channelIndex = msg.getChannelIndex();
        QString userFullName = msg.getUserName();
        QString GUID = msg.getGUID();
        downloads.insert(GUID, new Download(userFullName, channelIndex, GUID));
    }

}

void Service::handle(const DownloadIntervalWrite& msg){
    if (downloads.contains(msg.getGUID())) {
        Download* download = downloads[msg.getGUID()];
        download->appendVorbisData(msg.getEncodedAudioData());
        User* user = currentServer->getUser(download->getUserFullName());
        if (msg.downloadIsComplete()) {
            emit audioIntervalCompleted(*user, download->getChannelIndex(), download->getVorbisData());
            delete download;
            downloads.remove(msg.getGUID());
        }
        else{
            emit audioIntervalDownloading(*user, download->getChannelIndex(), msg.getEncodedAudioData().size());
        }
    } else {
        qCritical("GUID is not in map!");
    }
}

void Service::handle(const ServerKeepAliveMessage& /*msg*/){
    ClientKeepAlive clientKeepAliveMessage;
    sendMessageToServer((ClientMessage*)&clientKeepAliveMessage);
}

void Service::handle(const ServerAuthChallengeMessage& msg){
    ClientAuthUserMessage msgAuthUser(this->userName, msg.getChallenge(), msg.getProtocolVersion(), this->password);
    sendMessageToServer(&msgAuthUser);
    this->serverLicence = msg.getLicenceAgreement();
    this->serverKeepAlivePeriod = msg.getServerKeepAlivePeriod();
}

void Service::sendNewChannelsListToServer(QStringList channelsNames){
    this->channels = channelsNames;
    ClientSetChannel setChannelMsg(this->channels);
    sendMessageToServer(&setChannelMsg);
}

void Service::sendRemovedChannelIndex(int removedChannelIndex){
    assert( removedChannelIndex >= 0 && removedChannelIndex < channels.size() );
    channels.removeAt(removedChannelIndex);
    ClientSetChannel setChannelMsg(this->channels);
    sendMessageToServer(&setChannelMsg);

}

void Service::handle(const ServerAuthReplyMessage& msg){
    if(msg.userIsAuthenticated()){
        this->newUserName = msg.getNewUserName();
        ClientSetChannel setChannelMsg(this->channels);
        sendMessageToServer(&setChannelMsg);
        quint8 serverMaxChannels = msg.getMaxChannels();
        QString serverIp = socket.peerName();
        quint16 serverPort = socket.peerPort();
        currentServer.reset( new Server(serverIp, serverPort, serverMaxChannels));
    }
    //when user is not authenticated the socketErrorSlot is called and dispatch an error signal
//    else{
//        emit error(msg.getErrorMessage());
//    }
}

void Service::startServerConnection(QString serverIp, int serverPort, QString userName, QStringList channels, QString password){
    initialized = false;
    this->userName = userName;
    this->password = password;
    this->channels = channels;

    if(socket.state() == QTcpSocket::UnconnectedState){//check state to avoid a bug if user try enter in a server using double click in the button
        socket.connectToHost(serverIp, serverPort);
    }
}

void Service::disconnectFromServer(){
    qCDebug(ninjamService) << "disconnecting from " << socket.peerName();
    socket.disconnectFromHost();
}

void Service::setBpm(quint16 newBpm){
    if (currentServer == nullptr) {
        throw ("currentServer == null");
    }
    if (currentServer->setBpm(newBpm) && initialized) {

        emit serverBpmChanged(currentServer->getBpm());

    }
}

void Service::setBpi(quint16 bpi) {
    if (currentServer == nullptr) {
        throw ("currentServer == null");
    }
    quint16 lastBpi = currentServer->getBpi();
    if (currentServer->setBpi(bpi) && initialized) {

        emit serverBpiChanged(currentServer->getBpi(), lastBpi);

    }
}

//+++++++++++++ SERVER MESSAGE HANDLERS +++++++++++++=
void Service::handleUserChannels(QString userFullName, QList<UserChannel> channelsInTheServer) {
    //check for new channels
    User* user = this->currentServer->getUser(userFullName);
    //qWarning() << user->getName() << " canais:" << user->getChannels().size();
    foreach (UserChannel c , channelsInTheServer) {
        if (c.isActive()) {
            if (!user->hasChannel(c.getIndex())) {
                user->addChannel(c);
                emit userChannelCreated(*user, c);

            } else {//check for channel updates
                if (user->hasChannels()) {
                    //UserChannel userChannel = user->getChannel(c.getIndex());
                    if (channelIsOutdate(*user, c)) {
                        user->setChannelName(c.getIndex(), c.getName());
                        user->setChannelFlags(c.getIndex(), c.getFlags());
                        emit userChannelUpdated(*user, user->getChannel(c.getIndex()));
                    }
                }
            }
        }
        else{
            user->removeChannel(c.getIndex());
            emit userChannelRemoved(*user, c);
        }
    }
}

//verifica se o canal do usuario esta diferente do canal que veio do servidor
bool Service::channelIsOutdate(const User& user, const UserChannel& serverChannel) {
    if (user.getFullName() != serverChannel.getUserFullName()) {
        throw ("The user in channel is illegal!");
    }
    UserChannel userChannel = user.getChannel(serverChannel.getIndex());
    if (userChannel.getName() != serverChannel.getName()) {
        return true;
    }
    if (userChannel.getFlags() != serverChannel.getFlags()) {
        return true;
    }
    return false;
}

//+++++++++++++ CHAT MESSAGES ++++++++++++++++++++++

void Service::handle(const ServerChatMessage& msg) {
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
        //QString userName = msg.getArguments().at(0);
        QString topicText = msg.getArguments().at(1);
        if (!initialized) {
            initialized = true;

            currentServer->setTopic(topicText);
            currentServer->setLicence(serverLicence);//server licence is received when the hand shake with server is started

            //serverLicence.clear();
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


void Service::handle(const ServerConfigChangeNotifyMessage& msg)
{
    quint16 bpi = msg.getBpi();
    quint16 bpm = msg.getBpm();
    if (bpi != currentServer->getBpi()) {
        setBpi(bpi);
    }
    if (bpm != currentServer->getBpm()) {
        setBpm(bpm);
    }
}

void Service::invokeMessageHandler(const ServerMessage& message){
    //qDebug() << message;
    switch (message.getMessageType()) {
    case ServerMessageType::AUTH_CHALLENGE:
        handle((ServerAuthChallengeMessage&)message);
        break;
    case ServerMessageType::AUTH_REPLY:
        handle((ServerAuthReplyMessage&)message);
        break;
    case ServerMessageType::SERVER_CONFIG_CHANGE_NOTIFY:
        handle((ServerConfigChangeNotifyMessage&) message);
        break;
    case ServerMessageType::USER_INFO_CHANGE_NOTIFY:
        handle((UserInfoChangeNotifyMessage&) message);
        break;
    case ServerMessageType::CHAT_MESSAGE:
        handle((ServerChatMessage&) message);
        break;
    case ServerMessageType::KEEP_ALIVE:
        handle((ServerKeepAliveMessage&) message);
        break;
    case ServerMessageType::DOWNLOAD_INTERVAL_BEGIN:
        handle((DownloadIntervalBegin&) message);
        break;
    case ServerMessageType::DOWNLOAD_INTERVAL_WRITE:
        handle((DownloadIntervalWrite&) message);
        break;

    default:
        qCCritical(ninjamService) << "receive a not implemented yet message!";
    }
}

QString Service::getCurrentServerLicence() const{
    return serverLicence;
}
