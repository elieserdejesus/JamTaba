#ifndef _SERVER_SERVER_
#define _SERVER_SERVER_

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QList>
#include <QTimer>

#include "ninjam/Ninjam.h"
#include "ninjam/client/User.h"

#include <functional>

namespace ninjam { namespace client {
    class ClientToServerChatMessage;
    class UserChannel;
}}

namespace ninjam {

namespace server {

using ninjam::client::User;
using ninjam::client::UserChannel;
using ninjam::client::ClientToServerChatMessage;

class RemoteUser : public User
{
public:
    RemoteUser();
    void setLastKeepAliveToNow();
    quint64 getLastKeepAliveReceived() const;
    MessageHeader getCurrentHeader() const;
    void setCurrentHeader(MessageHeader header);
    void setFullName(const QString &fullName);
    void updateChannels(const QList<UserChannel> &newChannels);

    inline bool receivedInitialServerInfos() const
    {
        return receivedServerInfos;
    }

    inline void setReceivedServerInfos()
    {
        receivedServerInfos = true;
    }

private:
    MessageHeader currentHeader;
    quint64 lastKeepAliveReceived;
    bool receivedServerInfos;
};

inline void RemoteUser::setCurrentHeader(MessageHeader header)
{
    currentHeader = header;
}

inline MessageHeader RemoteUser::getCurrentHeader() const
{
    return currentHeader;
}

inline quint64 RemoteUser::getLastKeepAliveReceived() const
{
    return lastKeepAliveReceived;
}

class Voting : public QObject {

    Q_OBJECT

public:
    Voting(QObject *parent = nullptr);
    ~Voting();
    void start(quint16 newValue, quint8 requiredVotes, quint64 expiration = 60000);
    void registerVote(const QString &userFullName, quint16 value);
    bool isRunning() const;

signals:
    void expired(quint16 voteValue);
    void incremented(quint16 votingValue, quint16 currentVotes, quint16 requiredVotes, quint64 expirationTime);
    void accepted(quint16 acceptedValue);

private:
    quint16 value;
    QTimer *timer;
    quint8 requiredVotes;
    QSet<QString> voters;

    void reset();
};

class Server : public QObject
{
    Q_OBJECT

public:
    Server();
    virtual ~Server();
    virtual void start(quint16 port);
    void shutdown();

    bool isStarted() const;

    quint16 getPort() const;
    QString getIP() const;

    quint16 getBpi() const;
    quint16 getBpm() const;
    QString getTopic() const;
    QString getLicence() const;
    quint8 getMaxUsers() const;
    quint8 getMaxChannels() const;

    QStringList getConnectedUsersNames() const;

    quint64 getDownloadTransferRate() const;
    quint64 getUploadTransferRate() const;

signals:
    void serverStarted();
    void errorStartingServer(const QString &errorMessage);
    void serverStopped();
    void incommingConnection(const QString &incommingIP);
    void userEntered(const QString &userName);
    void userLeave(const QString &userName);

protected:
    void sendAuthChallenge(QTcpSocket *device);

protected slots:
    virtual void handleNewConnection();
    void handleAcceptError(QAbstractSocket::SocketError socketError);
    void processReceivedBytes();
    void handleDisconnection();
    void handleClientSocketError(QAbstractSocket::SocketError error);

    void bpiVotingExpired(quint16 bpiValue);
    void bpiVotingAccepted(quint16 acceptedValue);
    void bpiVotingIncremented(quint16 votingValue, quint16 currentVotes, quint16 requiredVotes, quint64 expirationTime);

    void bpmVotingExpired(quint16 bpmValue);
    void bpmVotingAccepted(quint16 acceptedValue);
    void bpmVotingIncremented(quint16 votingValue, quint16 currentVotes, quint16 requiredVotes, quint64 expirationTime);

private:
    QTcpServer tcpServer;
    QMap<QTcpSocket *, RemoteUser> remoteUsers; // connected clients

    quint16 bpm;
    quint16 bpi;
    QString topic;
    QString licence;
    quint8 maxUsers;
    quint8 maxChannels;
    quint16 keepAlivePeriod;

    NetworkUsageMeasurer totalUploadMeasurer;
    NetworkUsageMeasurer totalDownloadMeasurer;

    struct VotingSettings
    {
        qreal trheshold;
        quint64 expirationPeriod;
    };

    VotingSettings votingSettings;
    QMap<quint16, Voting *> bpmVotings;
    QMap<quint16, Voting *> bpiVotings;

    void broadcastUserChanges(const QString userFullName, const QList<UserChannel> &userChannels);
    void sendConnectedUsersTo(QTcpSocket *socket);
    void broadcastPublicChatMessage(const ClientToServerChatMessage &receivedMessage, const QString &userFullName);
    void broadcastVotingSystemMessage(const QString &message);
    void broadcastServerMessage(const QString &serverMessage, QTcpSocket *exclude);

    void processBpiVoteMessage(const ClientToServerChatMessage &msg, const QString &userFullName);
    void processBpmVoteMessage(const ClientToServerChatMessage &msg, const QString &userFullName);
    void processVoteMessage(const QString &userFullName, quint16 voteValue, quint16 currentValue, QMap<quint16, Voting *> &votings, std::function<Voting *()> createVoting);
    Voting *createBpiVoting();
    Voting *createBpmVoting();

    void processClientAuthUserMessage(QTcpSocket *socket, const MessageHeader &header);
    void processClientSetChannel(QTcpSocket *socket, const MessageHeader &header);
    void processUploadIntervalBegin(QTcpSocket *socket, const MessageHeader &header);
    void processUploadIntervalWrite(QTcpSocket *socket, const MessageHeader &header);
    void processChatMessage(QTcpSocket *socket, const MessageHeader &header);
    void processKeepAlive(QTcpSocket *socket, const MessageHeader &header);
    void processClientSetUserMask(QTcpSocket *socket, const MessageHeader &header);

    void sendServerInitialInfosTo(QTcpSocket *socket);

    void sendPrivateMessage(const QString &sender, const ClientToServerChatMessage &receivedMessage);
    void processAdminCommand(const QString &cmd);
    void setTopic(const QString &newTopic);
    void setBpi(quint16 newBpi);
    void setBpm(quint16 newBpm);

    void disconnectClient(QTcpSocket *socket);

    QString generateUniqueUserName(const QString &userName) const; // return sanitized and unique username

    void updateKeepAliveInfos();

    static QHostAddress getBestHostAddress();
};

inline quint64 Server::getDownloadTransferRate() const
{
    return totalDownloadMeasurer.getTransferRate();
}

inline quint64 Server::getUploadTransferRate() const
{
    return totalUploadMeasurer.getTransferRate();
}

inline quint8 Server::getMaxChannels() const
{
    return maxChannels;
}

inline quint8 Server::getMaxUsers() const
{
    return maxUsers;
}

inline QString Server::getLicence() const
{
    return licence;
}

inline QString Server::getTopic() const
{
    return topic;
}

inline quint16 Server::getBpi() const
{
    return bpi;
}

inline quint16 Server::getBpm() const
{
    return bpm;
}

} // ns server
} // ns ninjam

#endif
