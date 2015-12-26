#ifndef SERVICE_H
#define SERVICE_H

#include <QTcpSocket>
#include <memory>
#include <QLoggingCategory>

#include "ninjam/User.h"
#include "ninjam/UserChannel.h"

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
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Service : public QObject
{
    Q_OBJECT

public:

    explicit Service();
    ~Service();
    static bool isBotName(QString userName);

    void sendChatMessageToServer(QString message);

    // audio interval upload
    void sendAudioIntervalPart(QByteArray GUID, QByteArray encodedAudioBuffer, bool isLastPart);
    void sendAudioIntervalBegin(QByteArray GUID, quint8 channelIndex);

    void sendNewChannelsListToServer(QStringList channelsNames);
    void sendRemovedChannelIndex(int removedChannelIndex);

    QString getConnectedUserName();
    QString getCurrentServerLicence() const;
    float getIntervalPeriod();

    void startServerConnection(QString serverIp, int serverPort, QString userName,
                               QStringList channels, QString password = "");
    void disconnectFromServer(bool emitDisconnectedSignal);

    void voteToChangeBPM(int newBPM);
    void voteToChangeBPI(int newBPI);

    static inline QStringList getBotNamesList()
    {
        return botNames;
    }

signals:
    void userChannelCreated(Ninjam::User user, Ninjam::UserChannel channel);
    void userChannelRemoved(Ninjam::User user, Ninjam::UserChannel channel);
    void userChannelUpdated(Ninjam::User user, Ninjam::UserChannel channel);
    void userCountMessageReceived(int users, int maxUsers);
    void serverBpiChanged(short currentBpi, short lastBpi);
    void serverBpmChanged(short currentBpm);
    void audioIntervalCompleted(Ninjam::User user, int channelIndex, QByteArray encodedAudioData);
    void audioIntervalDownloading(Ninjam::User, int channelIndex, int bytesDownloaded);
    void disconnectedFromServer(const Ninjam::Server &server);
    void connectedInServer(const Ninjam::Server &server);
    void chatMessageReceived(Ninjam::User sender, QString message);
    void privateMessageReceived(Ninjam::User sender, QString message);
    void userEnterInTheJam(Ninjam::User newUser);
    void userLeaveTheJam(Ninjam::User user);
    void error(QString msg);

private:

    static const long DEFAULT_KEEP_ALIVE_PERIOD = 3000;
    static std::unique_ptr<PublicServersParser> publicServersParser; // TODO use QScopedPointer ?

    QTcpSocket socket;
    QByteArray byteArray;

    static const QStringList botNames;
    static QStringList buildBotNamesList();

    // GUID, AudioInterval
    long lastSendTime;// time stamp of last send
    long serverKeepAlivePeriod;
    QString serverLicence;

    static std::unique_ptr<Service> serviceInstance; // TODO use QScopedPointer?

    QString newUserName;// name received from server when connected

    std::unique_ptr<Server> currentServer;
    bool initialized;
    QString userName;
    QString password;
    QStringList channels;// channels names

    void sendMessageToServer(ClientMessage *message);
    void handleUserChannels(QString userFullName, QList<UserChannel> channelsInTheServer);
    bool channelIsOutdate(const User &user, const UserChannel &serverChannel);

    void setBpm(quint16 newBpm);
    void setBpi(quint16 newBpi);

    // +++++= message handlers ++++
    void invokeMessageHandler(const ServerMessage &message);
    void handle(const ServerAuthChallengeMessage &msg);
    void handle(const ServerAuthReplyMessage &msg);
    void handle(const ServerConfigChangeNotifyMessage &msg);
    void handle(const UserInfoChangeNotifyMessage &msg);
    void handle(const ServerChatMessage &msg);
    void handle(const ServerKeepAliveMessage &msg);
    void handle(const DownloadIntervalBegin &msg);
    void handle(const DownloadIntervalWrite &msg);
    // ++++++++++++=

    class Download;
    // using GUID as key
    QMap<QString, Download *> downloads;

    bool needSendKeepAlive() const;

    bool lastMessageWasIncomplete;

private slots:
    void socketReadSlot();
    void socketErrorSlot(QAbstractSocket::SocketError error);
    void socketDisconnectSlot();
    void socketConnectedSlot();
};
}// namespace

#endif
