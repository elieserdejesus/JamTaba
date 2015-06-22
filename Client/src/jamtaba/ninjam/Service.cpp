#include "Service.h"
#include "Server.h"
#include "User.h"
#include "protocol/ServerMessageParser.h"
#include "protocol/ServerMessages.h"
#include "protocol/ClientMessages.h"
#include <algorithm>
#include <QHostAddress>
#include <QDateTime>
#include <assert.h>

using namespace Ninjam;

std::unique_ptr<Service> Service::serviceInstance;


Service::Service()
    :
      running(false),
      initialized(false)
{

}

Service::~Service(){
    qDebug() << "NinjamService destructor";
    socket.disconnectFromHost();
}

void Service::socketReadSlot(){
    if(socket.bytesAvailable() < 5){
        qDebug() << "not have enough bytes to read message header (5 bytes)";
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
            //qDebug() << message;
            invokeMessageHandler(message);
            if(needSendKeepAlive()){
                ClientKeepAlive clientKeepAliveMessage;
                sendMessageToServer((ClientMessage*)&clientKeepAliveMessage);
            }
        }
        else{
            //qWarning() << "incomplete message!";
            lastMessageWasIncomplete = true;
            break;
        }
    }
}

void Service::socketErrorSlot(QAbstractSocket::SocketError /*e*/)
{
    //qDebug() << "Socket error: " << error;
    emit error("Socket error");
}

void Service::socketDisconnectSlot()
{
    //qFatal("Socket disconnected");
    emit disconnectedFromServer(false);
}


Service* Service::getInstance() {
    if(!serviceInstance){
        serviceInstance = std::unique_ptr<Service>(new Service());
    }
    return serviceInstance.get();
}

bool Service::isBotName(QString userName) {
    userName = userName.trimmed().toLower();
    return (userName == "jambot") ||
            (userName == "ninbot") ||
            (userName == "MUTANTLAB") ||
            (userName == "LiveStream");
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

void Service::buildNewSocket()   {
    if(socket.isOpen()){
        socket.close();
    }

    //socket = QTcpSocket(this);
    connect(&socket, SIGNAL(readyRead()), this, SLOT(socketReadSlot()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketErrorSlot(QAbstractSocket::SocketError)));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(socketDisconnectSlot()));

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

void Service::sendMessageToServer(ClientMessage *message)
{
    QByteArray outBuffer;
    message->serializeTo(outBuffer);
    int bytesWrited = socket.write(outBuffer);
    socket.flush();
    if(bytesWrited <= 0){
        qFatal("não escreveu os bytes");
    }
    lastSendTime = QDateTime::currentMSecsSinceEpoch();
    //qDebug() << message;
    assert((int)message->getPayload() + 5 == outBuffer.size());
}

bool Service::needSendKeepAlive() const{
    long ellapsedSeconds = (QDateTime::currentMSecsSinceEpoch() - lastSendTime)/1000;
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
        if (msg.downloadIsComplete()) {
            User* user = currentServer->getUser(download->getUserFullName());
            emit audioIntervalAvailable(*user, download->getChannelIndex(), download->getVorbisData());
            delete download;
            downloads.remove(msg.getGUID());
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
    ClientAuthUserMessage msgAuthUser(this->userName, msg.getChallenge(), msg.getProtocolVersion());
    sendMessageToServer(&msgAuthUser);
}

void Service::handle(const ServerAuthReplyMessage& msg){
    if(msg.userIsAuthenticated()){
        ClientSetChannel setChannelMsg(this->channels[0]);
        sendMessageToServer(&setChannelMsg);
    }
    else{
        emit error("Can't authenticate in server");
        disconnectFromServer(false);
    }
}

void Service::startServerConnection(QString serverIp, int serverPort, QString userName, QStringList channels, QString password){
    initialized = running = false;
    this->userName = userName;
    this->password = password;
    this->channels = channels;

    //qDebug() << "Starting connection with "<< serverIp << ":"<< serverPort;
    buildNewSocket();
    socket.connectToHost(serverIp, serverPort);
    if(!socket.waitForConnected()){
        emit error("can't connect in " + serverIp + ":" + serverPort);
    }

    //the old current server is deleted by unique_ptr
    currentServer.reset( new Server(serverIp, serverPort));

}

void Service::disconnectFromServer(bool /*normalDisconnection*/)
{
    qWarning() << "disconnected from server";
}
/*


    void sendNewUserInfos(QString newChannelName) {
        sendMessageToServer(new ClientSetChannel(newChannelName));
    }

    void sendChatMessage(QString message){
        enqueueMessageToSend(new ChatMessage(message));
    }

    public void sendPrivateChatMessage(String message, String user) throws NinjaMSendException {
        enqueueMessageToSend(new PrivateChatMessage(message, user));
    }

    public void voteToChangeBPM(int newBPM) {
        throw new RuntimeException("not implemented");
    }

    public void voteToChangeBPI(int newBPI) {
        throw new RuntimeException("not implemented");
    }

    public void changeTopic(String newTopic) {
        throw new RuntimeException("not implemented");
    }
    */

//FileChannel channel = null;
//int intervalo = 0;

/*

    private void enqueueMessageToSend(ClientMessage message) {
        try {
            messagesToSend.put(message);
        } catch (Exception ex) {
            LOGGER.log(Level.SEVERE, ex.getMessage(), ex);
        }
    }

    private class SendMessageTask implements Runnable {

        @Override
        public void run() {
            try {
                while (!Thread.currentThread().isInterrupted()) {
                    ClientMessage message = messagesToSend.take();
                    sendMessageToServer(message);
                }
            } catch (InterruptedException iEx) {
                //
            } catch (Exception e) {
                LOGGER.log(Level.SEVERE, e.getMessage(), e);
                disconnect(false);//not a normal disconnection
            }
        }
    }
*/
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
        } else {
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

            emit connectedInServer(*currentServer);

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


/*
    public long getReceivedBytesPerSecond() {
        return receivedBytes.getAccumulatedValueInPeriod();
    }

    public long getSendedBytesPerSecond() {
        return sendedBytes.getAccumulatedValueInPeriod();
    }

    public NinjaMServer getCurrentServer() {
        return currentServer;
    }

    private void disconnect(boolean normalDisconnection) {
        if (running) {
            messageHandlerTask.cancel(true);
            messageReceiveTask.cancel(true);
            messageSendTask.cancel(true);
            running = false;
            downloads.clear();
            LOGGER.info("disconnecting...");
            synchronized (listeners) {
                for (NinjaMServiceListener l : listeners) {
                    l.disconnectedFromServer(normalDisconnection);
                }
            }
        }
    }

    public void disconnect() {
        disconnect(true);
    }

    private void closeSocketChannel() {
        if (socketChannel != null && socketChannel.isOpen()) {
            try {
                socketChannel.close();
                socketChannel = null;
            } catch (IOException ex) {
                LOGGER.log(Level.SEVERE, ex.getMessage(), ex);
            }
        }

    }

    public void sendAudioIntervalPart(ByteBuffer encodedAudioBuffer, byte GUID[], boolean isLastPart) throws NinjaMSendException {
        enqueueMessageToSend(new ClientIntervalUploadWrite(GUID, encodedAudioBuffer, isLastPart));
    }

    public void sendAudioIntervalBegin(byte[] currentGUID, String userName, byte userChannelIndex) throws NinjaMSendException {
        enqueueMessageToSend(new ClientUploadIntervalBegin(currentGUID, userChannelIndex, userName));
    }

    public void stopTransmitting(byte GUID[], byte userChannel) throws NinjaMSendException {
        enqueueMessageToSend(new ClientUploadIntervalBegin(userChannel, newUserName));
    }

    //++++++++++++++++++++++++++
    private class MessageReceiveTask implements Runnable {

        private final ByteBuffer receiveBuffer = ByteBuffer.allocateDirect(1024 * 1024).order(ByteOrder.LITTLE_ENDIAN);

        @Override
        public void run() {
            running = true;
            LOGGER.info("Starting receiver thread to ninjam service");
            while (!Thread.currentThread().isInterrupted() && running) {
                try {
                    if (socketChannel.read(receiveBuffer) < 0 || !running) {//end of stream?
                        LOGGER.severe("Disconnecting in ReceiverTask, socket read return <= 0");
                        disconnect(false);//not a normal disconnection
                    }

                    receiveBuffer.flip();
                    while (receiveBuffer.hasRemaining()) {
                        if (receiveBuffer.remaining() < 5) {
                            break;
                        }
                        byte messageTypeCode = receiveBuffer.get();
                        int payloadLenght = receiveBuffer.getInt();
                        if (receiveBuffer.remaining() >= payloadLenght) {
                            ServerMessageParser parser = ServerMessageParserFactory.createParser(ServerMessageType.fromTypeCode(messageTypeCode));
                            final ServerMessage message = parser.parse(receiveBuffer, payloadLenght);
                            //LOGGER.log(Level.INFO, "receive {0}", message);
                            //System.out.println(message);
                            receivedMessages.put(message);//enqueue to process by another thread
                            receivedBytes.accumulate(payloadLenght + 5);
                        } else {
                            receiveBuffer.position(receiveBuffer.position() - 5);
                            break;//not enough bytes, wait to receive the other bytes
                        }
                    }
                    if (!receiveBuffer.hasRemaining()) {
                        receiveBuffer.clear();
                    } else {//keep the non handled bytes to the next parsing
                        receiveBuffer.compact();
                        LOGGER.log(Level.INFO, "waiting more bytes");
                    }
                    if (needSendKeepAliveToServer()) {
                        sendMessageToServer(new ClientKeepAlive());
                    }
                } catch (InterruptedException e) {
                    //
                } catch (AsynchronousCloseException iEx) {
                    //System.out.println("Message Receive Task interrupted");
                } catch (Exception e) {
                    LOGGER.log(Level.SEVERE, e.getMessage(), e);
                    running = false;
                    disconnect(false);//not a normal disconnection
                    break;
                }
            }

        }

    }

    private boolean needSendKeepAliveToServer() {
        return System.currentTimeMillis() - lastSendTime >= serverKeepAlivePeriod;
    }

    private class MessageHandlerTask implements Runnable {

        @Override
        public void run() {
            try {
                while (!Thread.currentThread().isInterrupted()) {
                    invokeMessageHandler(receivedMessages.take());
                }
            } catch (InterruptedException iEx) {
                //System.out.println("Message Handler Task interrupted");
            } catch (Exception e) {
                LOGGER.log(Level.SEVERE, e.getMessage(), e);
            }
        }

    }
*/

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
        qCritical("receive a not implemented yet message!");
    }
}




/*
//++++++

    public void close() {
        publicServersParser.shutdown();
        publicServersParser = null;
        LOGGER.info("Closing ninjamservice...");
        closeSocketChannel();
        if (serviceThread != null) {
            LOGGER.info("closing NinjamService serviceThread...");
            serviceThread.shutdownNow();
            serviceThread = null;
            LOGGER.info("NinjamService serviceThread closed!");
        }
        LOGGER.info("NInjaM service closed sucessfull!");

    }

    public void addListener(NinjaMServiceListener l) {
        synchronized (listeners) {
            if (!listeners.contains(l)) {
                listeners.add(l);
                //System.out.println("listener " + l.getClass().getName() + " added in NinjamService");
                //for (NinjaMServiceListener lstnr : listeners) {
                //  System.out.println("\t" + lstnr.getClass().getName());
                //}
            }
            //else{
            //  LOGGER.log(Level.SEVERE, "listener {0} not added in NinjamService!", l.getClass().getName());
            //}
        }
    }

    public void removeListener(NinjaMServiceListener l) {
        synchronized (listeners) {
            listeners.remove(l);
            //System.out.println("listener " + l.getClass().getName() + " removed from NinjamService");
        }
    }



    public static byte[] newGUID() {

        UUID id = UUID.randomUUID();
        ByteBuffer b = ByteBuffer.wrap(new byte[16]).order(ByteOrder.LITTLE_ENDIAN);
        b.putLong(id.getMostSignificantBits());
        b.putLong(id.getLeastSignificantBits());
        return b.array();
//        String idString = Long.toHexString(id.getLeastSignificantBits() + id.getMostSignificantBits()).toUpperCase();
//        if (idString.length() < 16) {
//            int diff = 16 - idString.length();
//            byte bytes[] = new byte[16];
//            byte stringBytes[] = idString.getBytes();
//            System.arraycopy(stringBytes, 0, bytes, diff, stringBytes.length);
//            for (int i = 0; i < diff; i++) {
//                bytes[i] = (byte) 'A';
//            }
//            return bytes;
//
//        }

        //return idString.getBytes();
    }
*/

