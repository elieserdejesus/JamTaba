#ifndef SERVICE_H
#define SERVICE_H

class QTcpSocket;
class QByteArray;
class QDataStream;
class QString;
class QObject;

namespace Ninjam {
class PublicServersParser;
class Server;
class ClientMessage;
class Service;
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
class User;
class UserChannel;

struct MessageHeader{
    quint8 messageTypeCode;
    quint32 payload;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Service : public QObject
{
    Q_OBJECT

public:

    explicit Service();
    ~Service();
    static bool isBotName(const QString &userName);

    void sendChatMessageToServer(const QString &message);

    // audio interval upload
    void sendAudioIntervalPart(const QByteArray &GUID, const QByteArray &encodedAudioBuffer, bool isLastPart);
    void sendAudioIntervalBegin(const QByteArray &GUID, quint8 channelIndex);

    void sendNewChannelsListToServer(const QStringList &channelsNames);
    void sendRemovedChannelIndex(int removedChannelIndex);

    QString getConnectedUserName();
    QString getCurrentServerLicence() const;
    float getIntervalPeriod();

    void startServerConnection(const QString &serverIp, int serverPort, const QString &userName,
                               const QStringList &channels, const QString &password = "");
    void disconnectFromServer(bool emitDisconnectedSignal);

    void voteToChangeBPM(int newBPM);
    void voteToChangeBPI(int newBPI);

    static inline QStringList getBotNamesList()
    {
        return botNames;
    }

signals:
    void userChannelCreated(const Ninjam::User &user, const Ninjam::UserChannel &channel);
    void userChannelRemoved(const Ninjam::User &user, const Ninjam::UserChannel &channel);
    void userChannelUpdated(const Ninjam::User &user, const Ninjam::UserChannel &channel);
    void userCountMessageReceived(int users, int maxUsers);
    void serverBpiChanged(short currentBpi, short lastBpi);
    void serverBpmChanged(short currentBpm);
    void audioIntervalCompleted(const Ninjam::User &user, int channelIndex, const QByteArray &encodedAudioData);
    void audioIntervalDownloading(const Ninjam::User &, int channelIndex, int bytesDownloaded);
    void disconnectedFromServer(const Ninjam::Server &server);
    void connectedInServer(const Ninjam::Server &server);
    void chatMessageReceived(const Ninjam::User &sender, const QString &message);
    void privateMessageReceived(const Ninjam::User &sender, const QString &message); //TODO this works? I never see a private message in my life :)
    void userEntered(const Ninjam::User &newUser);
    void userExited(const Ninjam::User &user);
    void error(const QString &msg);

private slots:
    void handleAllReceivedMessages();
    void handleSocketError(QAbstractSocket::SocketError error);
    void handleSocketDisconnection();
    void handleSocketConnection();

private:

    // +++++= message handlers.
    void process(const ServerAuthChallengeMessage &msg);
    void process(const ServerAuthReplyMessage &msg);
    void process(const ServerConfigChangeNotifyMessage &msg);
    void process(const UserInfoChangeNotifyMessage &msg);
    void process(const ServerChatMessage &msg);
    void process(const ServerKeepAliveMessage &msg);
    void process(const DownloadIntervalBegin &msg);
    void process(const DownloadIntervalWrite &msg);
    // ++++++++++++=

    MessageHeader* extractMessageHeader(QDataStream &stream);

    template<class MessageClazz> //MessageClazz will be 'translated' to some class derived from ServerMessage
    bool handleMessage(QDataStream &stream, quint32 payload){
        bool allMessageDataIsAvailable = socket.bytesAvailable() >= payload;
        if (allMessageDataIsAvailable) {
            MessageClazz message(payload);
            stream >> message;
            process(message);//calling overload versions of 'process'
            return true; //the message was handled
        }
        return false;//the message was not handled
    }

    bool executeMessageHandler(MessageHeader *header, QDataStream &stream);

    QScopedPointer<MessageHeader> currentHeader;//the last messageHeader readed from socket

    static const long DEFAULT_KEEP_ALIVE_PERIOD = 3000;
    static std::unique_ptr<PublicServersParser> publicServersParser; // TODO use QScopedPointer ?

    QTcpSocket socket;

    static const QStringList botNames;
    static QStringList buildBotNamesList();

    long lastSendTime;// time stamp of last send
    long serverKeepAlivePeriod;
    QString serverLicence;

    QScopedPointer<Server> currentServer;

    bool initialized;
    QString userName;
    QString password;
    QStringList channels;// channels names

    void sendMessageToServer(const ClientMessage &message);
    void handleUserChannels(const QString &userFullName, const QList<UserChannel> &channelsInTheServer);
    bool channelIsOutdate(const User &user, const UserChannel &serverChannel);

    void setBpm(quint16 newBpm);
    void setBpi(quint16 newBpi);

    class Download; //using a nested class here. This class is for internal purpouses only.
    QMap<QString, Download> downloads;// using GUID as key

    bool needSendKeepAlive() const;

    void clear();
};

QDataStream &operator >>(QDataStream &stream, MessageHeader &header);

}// namespace

#endif
