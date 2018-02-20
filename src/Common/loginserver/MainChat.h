#ifndef _MAIN_CHAT_H_
#define _MAIN_CHAT_H_

#include <QWebSocket>
#include <QtNetwork>

namespace login {

class MainChat : public QObject
{
    Q_OBJECT

public:
    MainChat(QObject *parent = nullptr);
    void connectWithServer(const QString &serverUrl);
    void setUserName(const QString &userName);
    void sendServerInvite(const QString &destinationUserFullName, const QString &serverIP, quint16 serverPort);

    static const QString MAIN_CHAT_URL;

public slots:
    void sendPublicMessage(const QString &content);

signals:
    void messageReceived(const QString &userName, const QString &content);
    void usersListChanged(const QStringList &userNames);
    void serverInviteReceived(const QString &senderFullName, const QString &serverIP, quint16 serverPort);
    void connected();
    void disconnected();
    void error(const QString &error);

private slots:
    void textMessageReceived(const QString &message);

private:
    QWebSocket webSocket;

    void parseReceivedData(const QJsonObject &data);
    void parseReceivedMessage(const QJsonObject &data);
    void parseServerInvite(const QJsonObject &data);
    void parseNewUsersList(const QJsonObject &data);

    QString userName;
};

} // namespace

#endif // _MAIN_CHAT_H_
