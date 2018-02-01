#ifndef SERVICE_H
#define SERVICE_H

#include <QtGlobal>
#include <QScopedPointer>
#include <QObject>
#include <QTcpSocket>
#include "log/Logging.h"
//#include "ServerMessageProcessor.h"

class QTcpSocket;
class QByteArray;
class QDataStream;
class QString;
class QObject;
class QStringList;

namespace ninjam {

class Server;
class ClientMessage;
class Service;
class ServerMessage;
class ServerMessagesHandler;
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

class Service : public QObject
{
    Q_OBJECT

    friend class ServerMessagesHandler;

public:

    explicit Service();
    ~Service();
    static bool isBotName(const QString &userName);

    void sendPublicChatMessage(const QString &message);
    void sendPrivateChatMessage(const QString &message);
    void sendAdminCommand(const QString &message);

    void setChannelReceiveStatus(const QString &userFullName, quint8 channelIndex, bool receiveChannel);

    // audio interval upload
    void sendIntervalPart(const QByteArray &GUID, const QByteArray &encodedAudioBuffer, bool isLastPart);
    void sendIntervalBegin(const QByteArray &GUID, quint8 channelIndex, bool isAudioInterval);

    void sendNewChannelsListToServer(const QStringList &channelsNames);
    void sendRemovedChannelIndex(int removedChannelIndex);

    QString getConnectedUserName() const;
    QString getCurrentServerLicence() const;
    float getIntervalPeriod() const;

    void startServerConnection(const QString &serverIp, int serverPort, const QString &userName,
                               const QStringList &channels, const QString &password = "");
    void disconnectFromServer(bool emitDisconnectedSignal);

    void voteToChangeBPM(quint16 newBPM);
    void voteToChangeBPI(quint16 newBPI);

    inline ninjam::Server *getCurrentServer() const { return currentServer.data(); }

    static QStringList getBotNamesList();

    long getTotalUploadTransferRate() const;
    long getTotalDownloadTransferRate() const;
    long getDownloadTransferRate(const QString userFullName, quint8 channelIndex) const;

signals:
    void userChannelCreated(const ninjam::User &user, const ninjam::UserChannel &channel);
    void userChannelRemoved(const ninjam::User &user, const ninjam::UserChannel &channel);
    void userChannelUpdated(const ninjam::User &user, const ninjam::UserChannel &channel);
    void userCountMessageReceived(quint32 users, quint32 maxUsers);
    void serverBpiChanged(quint16 currentBpi, quint16 lastBpi);
    void serverBpmChanged(quint16 currentBpm);
    void audioIntervalCompleted(const ninjam::User &user, quint8 channelIndex, const QByteArray &encodedAudioData);
    void videoIntervalCompleted(const ninjam::User &user, const QByteArray &encodedVideoData);
    void audioIntervalDownloading(const ninjam::User &user, quint8 channelIndex, int bytesDownloaded);
    void disconnectedFromServer(const ninjam::Server &server);
    void connectedInServer(const ninjam::Server &server);
    void publicChatMessageReceived(const ninjam::User &sender, const QString &message);
    void privateChatMessageReceived(const ninjam::User &sender, const QString &message);
    void serverTopicMessageReceived(const QString &topic);
    void userEntered(const ninjam::User &newUser);
    void userExited(const ninjam::User &user);
    void error(const QString &msg);

protected:
    virtual QTcpSocket *createSocket();

    // +++++= message handlers.
    virtual void process(const ServerAuthChallengeMessage &msg);
    virtual void process(const ServerAuthReplyMessage &msg);
    virtual void process(const ServerConfigChangeNotifyMessage &msg);
    virtual void process(const UserInfoChangeNotifyMessage &msg);
    virtual void process(const ServerChatMessage &msg);
    virtual void process(const ServerKeepAliveMessage &msg);
    virtual void process(const DownloadIntervalBegin &msg);
    virtual void process(const DownloadIntervalWrite &msg);

private slots:
    void handleAllReceivedMessages();
    void handleSocketError(QAbstractSocket::SocketError error);
    void handleSocketDisconnection();
    void handleSocketConnection();

private:
    QScopedPointer<ServerMessagesHandler> messagesHandler;

    static const long DEFAULT_KEEP_ALIVE_PERIOD = 3000;

    QTcpSocket* socket;

    static const QStringList botNames;
    static QStringList buildBotNamesList();

    long lastSendTime; // time stamp of last send
    long serverKeepAlivePeriod;
    QString serverLicence;

    QScopedPointer<Server> currentServer;

    bool initialized;
    QString userName;
    QString password;
    QStringList channels; // channels names

    // statistics
    class NetworkUsageMeasurer
    {
    public:
        NetworkUsageMeasurer();
        void addTransferedBytes(long totalBytesTransfered);
        long getTransferRate() const;

    private:
        long totalBytesTransfered;
        qint64 lastMeasureTimeStamp;
        long transferRate;
    };

    NetworkUsageMeasurer totalUploadMeasurer;
    NetworkUsageMeasurer totalDownloadMeasurer;
    QMap<QString, QMap<quint8, NetworkUsageMeasurer>> channelDownloadMeasurers; // using userFullName as key in first QMap and channel ID as key in second map

    void sendMessageToServer(const ClientMessage &message);
    void handleUserChannels(const User &remoteUser);
    bool channelIsOutdate(const User &user, const UserChannel &serverChannel);

    void setBpm(quint16 newBpm);
    void setBpi(quint16 newBpi);

    class Download; // using a nested class here. This class is for internal purpouses only.
    QMap<QByteArray, Download> downloads; // using GUID as key

    bool needSendKeepAlive() const;

    void clear();

    void setupSocketSignals();

};

inline long Service::getTotalDownloadTransferRate() const
{
    return totalDownloadMeasurer.getTransferRate();
}

inline long Service::getTotalUploadTransferRate() const
{
    return totalUploadMeasurer.getTransferRate();
}

inline QStringList Service::getBotNamesList()
{
    return botNames;
}

} // namespace

#endif
