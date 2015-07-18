#pragma once

#include <QTcpSocket>
#include <memory>
//#include <QDebug>
//#include <QObject>
//#include <QBuffer>

//#include "nvwa/debug_new.h"

#include "../ninjam/User.h"
#include "../ninjam/UserChannel.h"


namespace Ninjam {

class PublicServersParser;
class Server;
class MixedPublicServersParser;

class ServerMessageParser;
class ServerMessageParserFactory;

class ServerMessage;
class ServerKeepAliveMessage;
class ServerAuthChallengeMessage;
class ServerAuthReplyMessage;
class ServerConfigChangeNotifyMessage;
class UserInfoChangeNotifyMessage;
class ServerKeepAliveMessage;
class ServerChatMessage;
class DownloadIntervalBegin;
class DownloadIntervalWrite;

class ClientMessage;

class User;
class UserChannel;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Service : public QObject{//mantive esta classe herdando de QObject para facilitar o uso de QTcpSocket

    Q_OBJECT

public:
    //static const bool USE_LOCAL_HOST = false;//debug
    //static QList<Service> getPublicServersInfos();
    static Service* getInstance();
    static bool isBotName(QString userName) ;

    void sendChatMessageToServer(QString message);

    //audio interval upload
    void sendAudioIntervalPart(QByteArray GUID, QByteArray encodedAudioBuffer, bool isLastPart);
    void sendAudioIntervalBegin(QByteArray GUID, quint8 channelIndex);
    //void stopTransmitting(char* GUID[], quint8 userChannel);

    void sendNewChannelsListToServer(QStringList channelsNames);
    void sendRemovedChannelIndex(int removedChannelIndex);

    QString getConnectedUserName() ;
    QString getCurrentServerLicence() const;
    float getIntervalPeriod() ;

    void startServerConnection(QString serverIp, int serverPort, QString userName, QStringList channels, QString password = "");
    void disconnectFromServer();

    ~Service();

    static inline QStringList getBotNamesList(){ return botNames; }

signals:
    void userChannelCreated(Ninjam::User user, Ninjam::UserChannel channel);
    void userChannelRemoved(Ninjam::User user, Ninjam::UserChannel channel);
    void userChannelUpdated(Ninjam::User user, Ninjam::UserChannel channel);
    void userCountMessageReceived(int users, int maxUsers);
    void serverBpiChanged(short currentBpi, short lastBpi);
    void serverBpmChanged(short currentBpm);
    void audioIntervalCompleted(Ninjam::User user, int channelIndex, QByteArray encodedAudioData);
    void audioIntervalDownloading(Ninjam::User, int channelIndex, int bytesDownloaded );
    void disconnectedFromServer(const Ninjam::Server& server);
    void connectedInServer(const Ninjam::Server& server);
    void chatMessageReceived(Ninjam::User sender, QString message);
    void privateMessageReceived(Ninjam::User sender, QString message);
    void userEnterInTheJam(Ninjam::User newUser);
    void userLeaveTheJam(Ninjam::User user);
    void error(QString msg);

private:
    static const long DEFAULT_KEEP_ALIVE_PERIOD = 3000;
    static std::unique_ptr<PublicServersParser> publicServersParser;// = new MixedPublicServersParser();
    Service();
    QTcpSocket socket;
    QByteArray byteArray;

    static const QStringList botNames;
    static QStringList buildBotNamesList();

    //GUID, AudioInterval
    long lastSendTime;//time stamp of last send
    long serverKeepAlivePeriod;
    QString serverLicence;

    static std::unique_ptr<Service> serviceInstance;// = new NinjaMService();// new TestService();

    QString newUserName;//name received from server when connected

    std::unique_ptr<Server> currentServer;
    bool running;// = false;
    bool initialized;// = false;
    QString userName;
    QString password;
    QStringList channels;//channels names

    void sendMessageToServer(ClientMessage* message) ;
    void handleUserChannels(QString userFullName, QList<UserChannel> channelsInTheServer);
    bool channelIsOutdate(const User &user, const UserChannel &serverChannel);

    void setBpm(quint16 newBpm);
    void setBpi(quint16 newBpi);

    //+++++= message handlers ++++
    void invokeMessageHandler(const ServerMessage& message) ;
    void handle(const ServerAuthChallengeMessage& msg);
    void handle(const ServerAuthReplyMessage& msg);
    void handle(const ServerConfigChangeNotifyMessage& msg);
    void handle(const UserInfoChangeNotifyMessage& msg);
    void handle(const ServerChatMessage& msg);
    void handle(const ServerKeepAliveMessage& msg);
    void handle(const DownloadIntervalBegin& msg);
    void handle(const DownloadIntervalWrite& msg);
    //++++++++++++=

    class Download {
    private:
        quint8 channelIndex;
        QString userFullName;
        QString GUID;
        QByteArray vorbisData;
    public:
        Download(QString userFullName, quint8 channelIndex, QString GUID)
            :channelIndex(channelIndex), userFullName(userFullName), GUID(GUID){
        }
        Download(){
            qWarning() << "using the default constructor!";
        }

        ~Download(){

        }

        inline void appendVorbisData(QByteArray data){ this->vorbisData.append(data); }

        inline quint8 getChannelIndex() const{return channelIndex;}
        inline QString getUserFullName() const{return userFullName;}
        inline QString getGUI() const{return GUID;}
        inline QByteArray getVorbisData() const{return vorbisData;}

    };

    //using GUID as key
    QMap<QString, Download*> downloads;

    bool needSendKeepAlive() const;

private slots:
    void socketReadSlot();
    void socketErrorSlot(QAbstractSocket::SocketError error);
    void socketDisconnectSlot();
    void socketConnectedSlot();
public:
    void voteToChangeBPM(int newBPM) ;

    void voteToChangeBPI(int newBPI) ;

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


    public void changeTopic(String newTopic) {
        throw new RuntimeException("not implemented");
    }

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

    /*

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
    private boolean needSendKeepAliveToServer() {
        return System.currentTimeMillis() - lastSendTime >= serverKeepAlivePeriod;
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

*/
};
}//namespace
