#pragma once

#include <QList>
#include <QTcpSocket>
#include <memory>
#include <QDebug>
#include <QObject>
#include <QBuffer>

class NinjamPublicServersParser;
class NinjamServer;
class MixedPublicServersParser;
class NinjamServiceListener;
class ServerAuthChallengeMessage;
class ServerAuthReplyMessage;
class ServerMessage;
class ClientMessage;
class ServerMessageParser;
class ServerMessageParserFactory;
class NinjamUser;
class NinjamUserChannel;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class NinjamService : public QObject{

    Q_OBJECT

signals:
    void serverChallengeAvailable(QString challenge);
    void userChannelCreated(const NinjamUser& user, const NinjamUserChannel& channel);
    void userChannelRemoved(const NinjamUser& user, const NinjamUserChannel& channel);
    void userChannelUpdated(const NinjamUser& user, const NinjamUserChannel& channel);
    void usercountMessageReceived(int users, int maxUsers);
    void serverBpiChanged(short currentBpi, short lastBpi);
    void serverBpmChanged(short currentBpm);
    void audioIntervalPartAvailable( const NinjamUser& user, int channelIndex, QByteArray encodedAudioData, bool lastPartOfInterval);
    void disconnectedFromServer(bool normalDisconnection);
    void connectedInServer(const NinjamServer& server);
    void chatMessageReceived(const NinjamUser& sender, QString message);
    void privateMessageReceived(const NinjamUser& sender, QString message);
    void userEnterInTheJam(const NinjamUser& newUser);
    void userLeaveTheJam(const NinjamUser& user);

public:
    static const bool USE_LOCAL_HOST = false;//debug
    static QList<NinjamServer> getPublicServersInfos();
    static NinjamService* getInstance();
    static bool isBotName(QString userName) ;

    QString getConnectedUserName() ;
    float getIntervalPeriod() ;

    void startServerConnection(QString serverIp, int serverPort, QString userName, QStringList channels, QString password = "");
    //void accomplishServerConnection(QString userName, QString channelName, ServerAuthChallengeMessage authChallengeMessage) ;
    //void accomplishServerConnection(QString userName, QString channelName, QString userPassword, ServerAuthChallengeMessage authChallengeMessage) ;
    void disconnectFromServer(bool normalDisconnection=true);

    ~NinjamService();
private:
    static const long DEFAULT_KEEP_ALIVE_PERIOD = 3000;
    static NinjamPublicServersParser* publicServersParser;// = new MixedPublicServersParser();
    NinjamService();

    QList<std::shared_ptr<NinjamServiceListener>> listeners;
    QTcpSocket* socket;
    //QBuffer* inputBuffer;
    //QDataStream outputStream;

    //GUID, AudioInterval
    long lastSendTime;//time stamp of last send
    long serverKeepAlivePeriod;

    static std::shared_ptr<NinjamService> serviceInstance;// = new NinjaMService();// new TestService();

    QString newUserName;//name received from server when connected

    NinjamServer* currentServer;
    bool running;// = false;
    bool initialized;// = false;
    QString userName;
    QString password;
    QStringList channels;//channels names

    void buildNewSocket()  ;

    void sendMessageToServer(ClientMessage* message) ;

    //+++++= message handlers ++++
    void invokeMessageHandler(ServerMessage* message, QDataStream& stream) ;
    void handle(ServerAuthChallengeMessage* msg);
    void handle(ServerAuthReplyMessage* msg);

private slots:
    void socketReadSlot();
    void socketErrorSlot(QAbstractSocket::SocketError error);
    void socketDisconnectSlot();
public:

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
    private synchronized void sendMessageToServer(ClientMessage message) throws NinjaMSendException {
        //System.out.println(message);
        try {
            sendBuffer.clear();
            message.serializeTo(sendBuffer);
            sendBuffer.flip();

            int bytesSended = socketChannel.write(sendBuffer);
            sendedBytes.accumulate(bytesSended);

            lastSendTime = System.currentTimeMillis();

        } catch (Exception ex) {
            LOGGER.log(Level.SEVERE, null, ex);
            disconnect(false);//not a normal disconnection
        }
    }

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

    private void invokeMessageHandler(final ServerMessage message) throws NinjaMSendException {
        switch (message.getMessageType()) {
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
            default:
                throw new IllegalArgumentException("not implement yet: " + message);
        }
    }
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



    private static void testNinjamService() throws NinjaMConnectionException {
        NinjaMService service = NinjaMService.getInstance();
        service.addListener(new NinjaMServiceListener() {

            @Override
            public void userChannelCreated(NinjaMUser user, UserChannel channel) {
                System.out.println("canal criado: " + user);
            }

            @Override
            public void userChannelRemoved(NinjaMUser user, UserChannel channel) {
                System.out.println("canal removido: " + user);
            }

            @Override
            public void userChannelUpdated(NinjaMUser user, UserChannel channel) {
                System.out.println("canal atualizado: " + user);
            }

            @Override
            public void usercountMessageReceived(int users, int maxUsers) {
                throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
            }

            @Override
            public void serverBpiChanged(short currentBpi, short lastBpi) {
                System.out.println("bpi changed: " + currentBpi);
            }

            @Override
            public void serverBpmChanged(short currentBpm) {
                System.out.println("bpm changed: " + currentBpm);
            }

            @Override
            public void audioIntervalPartAvailable(NinjaMUser user, byte channelIndex, ByteBuffer encodedAudioData, boolean lastPartOfInterval) {

            }

            @Override
            public void disconnectedFromServer(boolean normalDisconnection) {
                System.out.println("Disconnected listener");
            }

            @Override
            public void connectedInServer(NinjaMServer server) {
                System.out.println("conectado no servidor " + server);
                for (NinjaMUser ninjaMUser : server.getUsers()) {
                    System.out.println(ninjaMUser);
                }
            }

            @Override
            public void chatMessageReceived(NinjaMUser sender, String message) {
                System.out.println("chat message: " + message);
            }

            @Override
            public void privateMessageReceived(NinjaMUser sender, String message) {
                throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
            }

//            @Override
//            public void topicMessageReceived(NinjaMUser sender, String topicMessage) {
//                System.out.println("topic received: " + sender + " - " + topicMessage);
//            }
            @Override
            public void userEnterInTheJam(NinjaMUser newUser) {
                System.out.println("user enter in jam: " + newUser);
            }

            @Override
            public void userLeaveTheJam(NinjaMUser user) {
                System.out.println("user leave the jam: " + user);
            }
        });

        //List<NinjaMServer> servers = NinjaMService.getPublicServersInfos();
        //for (NinjaMServer s : servers) {
        //  if (s.isActive() && !s.containsBotOnly()) {
        NinjaMServer s = NinjaMServer.getServer("localhost", 2049);
        //NinjaMServer s = NinjaMServer.getServer("ninbot.com", 2049);
        //System.out.println(s);
        //NinjaMServer s = NinjaMServer.getServer("jammers.ddns.net", 2049);
        ServerAuthChallenge challenge = service.startServerConnection(s.getHostName(), s.getPort());
        service.accomplishServerConnection("test", "channel name", challenge);
        //    break;
        //}
        //}
    }

    public static void main(String args[]) throws IOException, NinjaMConnectionException {
        testNinjamService();
//        testGetServerStats();
    }

    public static void testGetServerStats() throws NinjaMConnectionException {
        //NinjaMService.getServerStats("ninjamers.servebeer.com", 2049, "bobs", "yauncle");
        NinjaMService.getServerStats("localhost", 2049, "teste", "123123");
    }

    private static void testPublicServersParsing() throws IOException {
        List<NinjaMServer> servers = NinjaMService.getPublicServersInfos();
        for (NinjaMServer server : servers) {
            System.out.println(server);
        }
    }

    //+++++++++++++++++++++
    private static class DirectByteBufferPool implements ReusableObjectPool<ByteBuffer> {

        private final int maxBufersSize;
        private Stack<ByteBuffer> pool = new Stack<ByteBuffer>();

        public DirectByteBufferPool(int maxBytesInBuffers) {
            this.maxBufersSize = maxBytesInBuffers;
        }

        @Override
        public synchronized ByteBuffer pickFromPool() {
            if (pool.isEmpty()) {
                pool.push(ByteBuffer.allocateDirect(maxBufersSize));
            }
            return pool.pop();
        }

        @Override
        public synchronized void backToPool(ByteBuffer buffer) {
            pool.push(buffer);
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
};
