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

namespace Ninjam {
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

    void sendChatMessageToServer(const QString &message);

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

    inline Ninjam::Server *getCurrentServer() const { return currentServer.data(); }

    static inline QStringList getBotNamesList()
    {
        return botNames;
    }

signals:
    void userChannelCreated(const Ninjam::User &user, const Ninjam::UserChannel &channel);
    void userChannelRemoved(const Ninjam::User &user, const Ninjam::UserChannel &channel);
    void userChannelUpdated(const Ninjam::User &user, const Ninjam::UserChannel &channel);
    void userCountMessageReceived(quint32 users, quint32 maxUsers);
    void serverBpiChanged(quint16 currentBpi, quint16 lastBpi);
    void serverBpmChanged(quint16 currentBpm);
    void audioIntervalCompleted(const Ninjam::User &user, quint8 channelIndex, const QByteArray &encodedAudioData);
    void videoIntervalCompleted(const Ninjam::User &user, const QByteArray &encodedVideoData);
    void audioIntervalDownloading(const Ninjam::User &user, quint8 channelIndex, int bytesDownloaded);
    void disconnectedFromServer(const Ninjam::Server &server);
    void connectedInServer(const Ninjam::Server &server);
    void chatMessageReceived(const Ninjam::User &sender, const QString &message);
    void serverTopicMessageReceived(const QString &topic);
    void privateMessageReceived(const Ninjam::User &sender, const QString &message); // TODO this works? I never see a private message in my life :)
    void userEntered(const Ninjam::User &newUser);
    void userExited(const Ninjam::User &user);
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

} // namespace

#endif
