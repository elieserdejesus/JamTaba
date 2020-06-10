#ifndef SERVICE_H
#define SERVICE_H

#include "log/Logging.h"
#include "ninjam/Ninjam.h"

#include <QtGlobal>
#include <QScopedPointer>
#include <QTcpSocket>
#include <QByteArray>
#include <QDataStream>
#include <QStringList>

namespace ninjam
{

namespace client
{
    class Service;
    class ServerInfo;
    class ClientMessage;
    class Service;
    class ServerMessage;
    class ServerMessagesHandler;
    class ServerKeepAliveMessage;
    class AuthChallengeMessage;
    class AuthReplyMessage;
    class ConfigChangeNotifyMessage;
    class ServerKeepAliveMessage;
    class ServerToClientChatMessage;
    class UserInfoChangeNotifyMessage;
    class DownloadIntervalBegin;
    class DownloadIntervalWrite;
    class User;
    class UserChannel;
    struct ChannelMetadata;

    class Service : public QObject
    {
        Q_OBJECT

        friend class ServerMessagesHandler;

    public:

        explicit Service();
        ~Service();
        static bool isBotName(const QString &userName);

        void sendPublicChatMessage(const QString &message);
        void sendPrivateChatMessage(const QString &message, const QString &destinationUser);
        void sendAdminCommand(const QString &message);

        void setChannelReceiveStatus(const QString &userFullName, quint8 channelIndex, bool receiveChannel);

        // audio interval upload
        void sendIntervalPart(const QByteArray &GUID, const QByteArray &encodedAudioBuffer, bool isLastPart);
        void sendIntervalBegin(const QByteArray &GUID, quint8 channelIndex, bool isAudioInterval);

        void sendNewChannelsListToServer(const QList<ChannelMetadata> &channelsMetadata);
        void sendRemovedChannelIndex(int removedChannelIndex);

        QString getConnectedUserName() const;
        QString getCurrentServerLicence() const;
        float getIntervalPeriod() const;

        void startServerConnection(const QString &serverIp, int serverPort, const QString &userName,
                                   const QList<ChannelMetadata> &channels, const QString &password = "");
        void disconnectFromServer(bool emitDisconnectedSignal);

        void voteToChangeBPM(quint16 newBPM);
        void voteToChangeBPI(quint16 newBPI);

        inline ServerInfo *getCurrentServer() const { return currentServer.data(); }

        static QStringList getBotNamesList();

        long getTotalUploadTransferRate() const;
        long getTotalDownloadTransferRate() const;
        long getDownloadTransferRate(const QString userFullName, quint8 channelIndex) const;

    signals:
        void userChannelCreated(const User &user, const UserChannel &channel);
        void userChannelRemoved(const User &user, const UserChannel &channel);
        void userChannelUpdated(const User &user, const UserChannel &channel);
        void userCountMessageReceived(quint32 users, quint32 maxUsers);
        void serverBpiChanged(quint16 currentBpi, quint16 lastBpi);
        void serverBpmChanged(quint16 currentBpm);
        void serverInitialBpmBpiAvailable(quint16 bpm, quint16 bpi);
        void audioIntervalCompleted(const User &user, quint8 channelIndex, const QByteArray &encodedAudioData);
        void videoIntervalCompleted(const User &user, const QByteArray &encodedVideoData);
        void audioIntervalDownloading(const User &user, quint8 channelIndex, const QByteArray &encodedAudioData, bool isFirstPart, bool isLastPart);
        void disconnectedFromServer(const ServerInfo &server);
        void connectedInServer(const ServerInfo &server);
        void publicChatMessageReceived(const User &sender, const QString &message);
        void privateChatMessageReceived(const User &sender, const QString &message);
        void serverTopicMessageReceived(const QString &topic);
        void userEntered(const User &newUser);
        void userExited(const User &user);
        void error(const QString &msg);

    protected:
        virtual QTcpSocket *createSocket();

        // +++++= message handlers.
        virtual void process(const AuthChallengeMessage &msg);
        virtual void process(const AuthReplyMessage &msg);
        virtual void process(const ConfigChangeNotifyMessage &msg);
        virtual void process(const UserInfoChangeNotifyMessage &msg);
        virtual void process(const ServerToClientChatMessage &msg);
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

        QScopedPointer<ServerInfo> currentServer;

        bool initialized;
        QString userName;
        QString password;
        QList<ChannelMetadata> channels; // channels names and voice chat flag

        NetworkUsageMeasurer totalUploadMeasurer;
        NetworkUsageMeasurer totalDownloadMeasurer;
        QMap<QString, QMap<quint8, NetworkUsageMeasurer>> channelDownloadMeasurers; // using userFullName as key in first QMap and channel ID as key in second map

        void sendMessageToServer(const ClientMessage &message);
        void handleUserChannels(const User &remoteUser);
        bool channelIsOutdate(const User &user, const UserChannel &serverChannel);

        void setBpm(quint16 newBpm);
        void setBpi(quint16 newBpi);
        void setInitialBpmBpi(quint16 bpm, quint16 bpi);

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
} // namespace

#endif
