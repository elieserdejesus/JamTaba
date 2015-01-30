#include "NinjamService.h"
#include "NinjamServer.h"
#include "protocol/server/parsers/ServerMessageParser.h"
#include "protocol/ServerMessages.h"
#include "protocol/ClientMessages.h"
#include <QHostAddress>
#include <QDateTime>


std::shared_ptr<NinjamService> NinjamService::serviceInstance;


NinjamService::NinjamService()
    : socket(nullptr),
      //inputBuffer(nullptr),
      running(false),
      initialized(false)
{

}

NinjamService::~NinjamService(){
    qDebug() << "NinjamService destructor";
}

void NinjamService::socketReadSlot(){
    if(socket->state() != QTcpSocket::ConnectedState){
        qCritical("Disconnecting, socket state != connected");
        disconnectFromServer(false);//not a normal disconnection
    }

    QByteArray inputBuffer = socket->readAll();
    //inputBuffer->buffer().append(socket->readAll());
    if(inputBuffer.size() < 5){
        qDebug() << "not have enough bytes to read message header (5 bytes)";
        return;
    }

    quint8 messageTypeCode;
    quint32 payloadLenght;
    QDataStream stream(inputBuffer);
    stream.setByteOrder(QDataStream::LittleEndian);

    int processed = 0;
    while (inputBuffer.size() - processed > 5 && socket->state() == QTcpSocket::ConnectedState){//consume all messages
        stream >> messageTypeCode;
        stream >> payloadLenght;
        processed += 5;
        qDebug() << "Recebeu código"<< messageTypeCode << " payload:"<< payloadLenght << endl;
        if (inputBuffer.size() - processed >= (int)payloadLenght) {
            ServerMessageParser* parser = ServerMessageParser::getParser( static_cast<ServerMessageType::MessageType>(messageTypeCode));
            ServerMessage* message = parser->parse(stream, payloadLenght);
            invokeMessageHandler(message, stream);
            //delete message;
            processed += payloadLenght;
        } else {//bytesAvailable < payloadLenght, not enough bytes in socket
            qCritical("not enough bytes, wait to receive the other bytes");
            qDebug() << "bytes available:"<< inputBuffer.size() << endl;
            break;
        }
    }
    //if (needSendKeepAliveToServer()) {
    //       sendMessageToServer(new ClientKeepAlive());
    //    }

}

void NinjamService::socketErrorSlot(QAbstractSocket::SocketError error)
{
    qDebug() << "Socket error: " << error;
}

void NinjamService::socketDisconnectSlot()
{
    qFatal("Socket disconnected");
}

/*
    QList<NinjamServer> NinjamService::getPublicServersInfos() {
        if (publicServersParser == nullptr) {
            publicServersParser = new MixedPublicServersParser();
        }
        QList<NinjaMServer*> servers;
        if (!USE_LOCAL_HOST) {
            servers = new ArrayList<NinjaMServer>(publicServersParser.getPublicServers());
        } else {
            NinjamServer localHostServer = NinjamServer.getServer("localhost", 2049);
            localHostServer.setMaxUsers(8);
            servers.append(&localHostServer);
        }
        return servers;
    }
*/

NinjamService* NinjamService::getInstance() {
    if(serviceInstance.get() == nullptr){
        serviceInstance = std::shared_ptr<NinjamService>(new NinjamService());
    }
    return serviceInstance.get();
}

bool NinjamService::isBotName(QString userName) {
    userName = userName.trimmed().toLower();
    return (userName == "jambot") ||
            (userName == "ninbot") ||
            (userName == "MUTANTLAB") ||
            (userName == "LiveStream");
}

QString NinjamService::getConnectedUserName() {
    if (initialized) {
        return newUserName;
    }
    qCritical() << "not initialized, newUserName is not available!";
    return "";
}

float NinjamService::getIntervalPeriod() {
    if (currentServer != nullptr) {
        return (float)60000 / currentServer->getBpm() * currentServer->getBpi();
    }
    return 0;
}

void NinjamService::buildNewSocket()   {
    if(socket != nullptr){
        if(socket->isOpen()){
            socket->close();
        }
        delete socket;
    }
    //if(inputBuffer != nullptr){delete inputBuffer;}

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadSlot()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketErrorSlot(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnectSlot()));
    //inputBuffer = new QBuffer(socket);
    //inputBuffer->open(QIODevice::ReadOnly);
    //outputStream = QDataStream(socket);
    //if(!socket->waitForConnected()){
    //    qFatal("socket can't connect!");
    // }
}

void NinjamService::sendMessageToServer(ClientMessage *message)
{
    QByteArray outBuffer;
    message->serializeTo(outBuffer);
    int bytesWrited = socket->write(outBuffer);
    if(bytesWrited <= 0){
        qFatal("não escreveu os bytes");
    }
    //qDebug() << "escreveu " << bytesWrited << " bytes no socket";
    lastSendTime = QDateTime::currentMSecsSinceEpoch();
    qDebug() << message;
}

void NinjamService::handle(ServerAuthChallengeMessage *msg)
{
    ClientAuthUserMessage msgAuthUser(this->userName, msg->getChallenge(), msg->getProtocolVersion());
    sendMessageToServer(&msgAuthUser);
}

void NinjamService::handle(ServerAuthReplyMessage *msg)
{
    ClientSetChannel msgSetChannel(channels[0]);//just the first channel at moment
    sendMessageToServer(&msgSetChannel);
}

void NinjamService::startServerConnection(QString serverIp, int serverPort, QString userName, QStringList channels, QString password){
    initialized = running = false;
    this->userName = userName;
    this->password = password;
    this->channels = channels;

    qDebug() << "Starting connection with "<< serverIp << ":"<< serverPort;
    if (socket != nullptr && socket->isOpen()) {
        socket->close();
    }
    buildNewSocket();
    socket->connectToHost(serverIp, serverPort);
}

void NinjamService::disconnectFromServer(bool normalDisconnection)
{

}
/*
    public static void getServerStats(String server, int port, String statsUser, String statsPassword) throws NinjaMConnectionException {
        NinjaMService statsService = new NinjaMService();
        statsService.addListener(new NinjaMServiceListenerAdapter() {


            public void connectedInServer(NinjaMServer server) {
                System.out.println(server);
                System.out.println("users: " + server.getUsers().size());
                for (NinjaMUser user : server.getUsers()) {
                    System.out.println(user);
                }
            }

            @Override
            public void userEnterInTheJam(NinjaMUser newUser) {
                System.out.println("new user: " + newUser);
            }

            @Override
            public void userChannelCreated(NinjaMUser user, UserChannel channel) {
                System.out.println("user channel: " + user.getName());
            }

            @Override
            public void chatMessageReceived(NinjaMUser sender, String message) {
                System.out.println(message);
            }

            @Override
            public void usercountMessageReceived(int users, int maxUsers) {
                System.out.println("users: " + users + "  maxUsers: " + maxUsers);
            }

            @Override
            public void privateMessageReceived(NinjaMUser sender, String message) {
                System.out.println(message);
            }

        });
        ServerAuthChallenge authChallenge = statsService.startServerConnection(server, port);
        statsService.accomplishServerConnection(statsUser, "", statsPassword, authChallenge);
        try {
            Thread.sleep(5000);
        } catch (InterruptedException e) {

        }
        statsService.disconnect();
    }


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

    private void setBpm(short bpm) {
        if (currentServer == null) {
            throw new IllegalStateException("currentServer == null");
        }
        if (currentServer.setBpm(bpm) && initialized) {
            synchronized (listeners) {
                for (NinjaMServiceListener l : listeners) {
                    l.serverBpmChanged(currentServer.getBpm());
                }
            }
        }
    }

    private void setBpi(short bpi) {
        if (currentServer == null) {
            throw new IllegalStateException("currentServer == null");
        }
        short lastBpi = currentServer.getBpi();
        if (currentServer.setBpi(bpi) && initialized) {
            synchronized (listeners) {
                for (NinjaMServiceListener l : listeners) {
                    l.serverBpiChanged(currentServer.getBpi(), lastBpi);
                }
            }
        }
    }
    */

//+++++++++++++ SERVER MESSAGE HANDLERS +++++++++++++=
//int message = 0;
/*
    private: void handle(ServerKeepAlive msg) {
        //if (System.currentTimeMillis() - lastSendTime >= serverKeepAlivePeriod) {
        if (needSendKeepAliveToServer()) {
            sendMessageToServer(new ClientKeepAlive());
        }
        //sendChatMessage("keep alive sended");
        //}
    }

    private void handle(ConfigChangeNotify msg) {
        short bpi = msg.getBpi();
        short bpm = msg.getBpm();
        if (bpi != currentServer.getBpi()) {
            setBpi(bpi);
        }
        if (bpm != currentServer.getBpm()) {
            setBpm(bpm);
        }

    }

    private void handleTopic(ServerChatMessage msg) {
        String userName = msg.getArguments().get(0);
        String topicText = msg.getArguments().get(1);
        if (!initialized) {
            initialized = true;
            List<NinjaMServiceListener> ls = new ArrayList<NinjaMServiceListener>(listeners);
            for (NinjaMServiceListener l : ls) {
                l.connectedInServer(currentServer);
            }
        }
        currentServer.setTopic(topicText);
//        for (NinjaMServiceListener l : listeners) {
//            l.topicMessageReceived(NinjaMUser.getUser(userName), topicText);
//        }
    }

    private void handle(UserInfoChangeNotify msg) throws NinjaMSendException {
        Map<NinjaMUser, List<UserChannel>> allUsersChannels = msg.getUsersChannels();
        Set<NinjaMUser> users = allUsersChannels.keySet();

        for (NinjaMUser user : users) {
            if (!currentServer.containsUser(user)) {
                currentServer.addUser(user);
                synchronized (listeners) {
                    for (NinjaMServiceListener l : listeners) {
                        l.userEnterInTheJam(user);
                    }
                }
            }

            handleUserChannels(user, allUsersChannels.get(user));
        }
//        if (users.isEmpty()) {
//            System.out.println("nenhum usuario no servidor?");
//        }

        sendMessageToServer(new ClientSetUserMask(msg.getUsers()));//enable new users channels
    }

    private void handleUserChannels(NinjaMUser user, Collection<UserChannel> channelsInTheServer) {
        Set<UserChannel> userCurrentChannels = user.getChannels();
        //check for new channels
        for (UserChannel c : channelsInTheServer) {
            //System.out.println(c);
            if (c.isActive()) {
                if (!userCurrentChannels.contains(c)) {
                    user.addChannel(c);
                    fireUserChannelCreated(user, c);
                } else {//check for channel updates
                    if (user.hasChannels()) {
                        UserChannel userChannel = user.getChannel(c.getIndex());
                        if (channelIsOutdate(user, c)) {
                            userChannel.setName(c.getName());
                            userChannel.setFlags(c.getFlags());
                            fireUserChannelUpdated(user, userChannel);
                        }
                    }
                }
            } else {
                user.removeChannel(c);
                fireUserChannelRemoved(user, c);
            }
        }
    }

    //verifica se o canal do usuario esta diferente do canal que veio do servidor
    private boolean channelIsOutdate(NinjaMUser user, UserChannel serverChannel) {
        if (!user.equals(serverChannel.getUser())) {
            throw new IllegalArgumentException("The user in channel is illegal!");
        }
        UserChannel userChannel = user.getChannel(serverChannel.getIndex());
        if (!userChannel.getName().equals(serverChannel.getName())) {
            return true;
        }
        if (userChannel.getFlags() != serverChannel.getFlags()) {
            return true;
        }
        return false;
    }

    private void fireUserChannelRemoved(NinjaMUser user, UserChannel userChannel) {
        synchronized (listeners) {
            for (NinjaMServiceListener l : listeners) {
                l.userChannelRemoved(user, userChannel);
            }
        }
    }

    private void fireUserChannelUpdated(NinjaMUser user, UserChannel userChannel) {
        synchronized (listeners) {
            for (NinjaMServiceListener l : listeners) {
                l.userChannelUpdated(user, userChannel);
            }
        }
    }

    private void fireUserChannelCreated(NinjaMUser user, UserChannel channel) {
        synchronized (listeners) {
            for (NinjaMServiceListener l : listeners) {
                l.userChannelCreated(user, channel);
            }
        }
    }

    private class Download {

        final byte channelIndex;
        final NinjaMUser user;
        final String GUID;

        public Download(NinjaMUser user, byte channelIndex, String GUID) {
            this.channelIndex = channelIndex;
            this.user = user;
            this.GUID = GUID;
        }

    }

    private Map<String, Download> downloads = new HashMap<String, Download>();

    private void handle(DownloadIntervalBegin msg) throws NinjaMSendException {

        if (!msg.downloadShouldBeStopped() && msg.isValidOggDownload()) {
            byte channelIndex = msg.getChannelIndex();
            String userFullName = msg.getUserName();
            String GUID = msg.getGUID();
            downloads.put(GUID, new Download(NinjaMUser.getUser(userFullName), channelIndex, GUID));
        }
    }

    private synchronized void handle(DownloadIntervalWrite msg) throws NinjaMSendException {
        if (downloads.containsKey(msg.getGUID())) {
            Download download = downloads.get(msg.getGUID());
            //System.out.println("last part: " + msg.downloadIsComplete());
            for (NinjaMServiceListener l : listeners) {
                l.audioIntervalPartAvailable(download.user, download.channelIndex, msg.getEncodedAudioData(), msg.downloadIsComplete());
            }
            if (msg.downloadIsComplete()) {
                downloads.remove(msg.getGUID());
            }
        } else {
            LOGGER.severe("GUID is not in map!");
        }
    }

    //+++++++++++++ CHAT MESSAGES ++++++++++++++++++++++
    private void handleMsg(ServerChatMessage msg) {
        String messageSender = msg.getArguments().get(0);
        String messageText = msg.getArguments().get(1);
        synchronized (listeners) {
            for (NinjaMServiceListener l : listeners) {
                l.chatMessageReceived(NinjaMUser.getUser(messageSender), messageText);
            }
        }
    }

    private void handlePart(ServerChatMessage msg) {
        String userLeavingTheServer = msg.getArguments().get(0);
        LOGGER.log(Level.INFO, "{0} removed", userLeavingTheServer);
        synchronized (listeners) {
            NinjaMUser userLeaving = NinjaMUser.getUser(userLeavingTheServer);
            for (NinjaMServiceListener l : listeners) {
                l.userLeaveTheJam(userLeaving);
            }
        }
    }

    private void handlePrivMsg(ServerChatMessage msg) {
        String messageSender = msg.getArguments().get(0);
        String messageText = msg.getArguments().get(1);
        synchronized (listeners) {
            for (NinjaMServiceListener l : listeners) {
                l.privateMessageReceived(NinjaMUser.getUser(messageSender), messageText);
            }
        }
    }

    private void handleJoin(ServerChatMessage msg) {
        //---------
    }

    private void handleUsercount(ServerChatMessage msg) {
        try {
            int users = Integer.parseInt(msg.getArguments().get(0));
            int maxUsers = Integer.parseInt(msg.getArguments().get(1));
            synchronized (listeners) {
                for (NinjaMServiceListener l : listeners) {
                    l.usercountMessageReceived(users, maxUsers);
                }
            }
        } catch (Exception e) {
            LOGGER.log(Level.WARNING, e.getMessage(), e);
        }
    }

    private void handle(ServerChatMessage msg) {
        switch (msg.getCommand()) {
            case JOIN:
                handleJoin(msg);
                break;
            case MSG:
                handleMsg(msg);
                break;
            case PART:
                handlePart(msg);
                break;
            case PRIVMSG:
                handlePrivMsg(msg);
                break;
            case TOPIC:
                handleTopic(msg);
                break;
            case USERCOUNT:
                handleUsercount(msg);
                break;
            default:
                throw new IllegalArgumentException("chat message type not implemented");
        }
    }

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
void NinjamService::invokeMessageHandler(ServerMessage *message, QDataStream &stream){
    qDebug() << message;
    switch (message->getMessageType()) {
    case ServerMessageType::AUTH_CHALLENGE:
        handle((ServerAuthChallengeMessage*)message);
        break;
    case ServerMessageType::AUTH_REPLY:
        handle((ServerAuthReplyMessage*)message);
        break;
        /*
            case KEEP_ALIVE:
                handle((ServerKeepAlive) message);
                break;
            case CONFIG_CHANGE_NOTIFY:
                handle((ConfigChangeNotify) message);
                break;
            case USER_INFO_CHANGE_NOTIFY:
                handle((UserInfoChangeNotify) message);
                break;

            case CHAT_MESSAGE:
                handle((ServerChatMessage) message);
                break;
            case DOWNLOAD_INTERVAL_BEGIN:
                handle((DownloadIntervalBegin) message);
                break;
            case DOWNLOAD_INTERVAL_WRITE:
                handle((DownloadIntervalWrite) message);
                break;
                */
    default:
        qCritical("not implement yet: " + message->getMessageType());
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

