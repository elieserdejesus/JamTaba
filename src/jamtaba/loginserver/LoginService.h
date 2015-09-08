#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "../ninjam/Server.h"

class NatMap;
class QTimer;

namespace Login {

/*
    O login service fica responsável por manter a sincronização
    do modelo com o webservice.
    se alguém entra ou sai de uma sala isso gera um evento roomChanged.
    os listeners pegam a nova lista de usuários e exibem. É mais simples
    remover todos os usuários da GUI e adicioná-los novamente.
    Dessa forma mantenho essa sincronização totalmente separada do NinjamService
    Uma coisa são os dados que preciso para mostrar as salas, outra coisa
    é o usuário tocando em uma sala.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++
class UserInfo{
public:
    UserInfo(long long id, QString name, QString ip);
    //bool isBot();
    inline QString getIp() const{return ip;}
    inline QString getName() const{return name;}
private:
    long long id;
    QString name;
    QString ip;

};
//+++++++++++++++++++++++++++++++++++++++++++++++++++
enum class RoomTYPE{NINJAM, REALTIME};

class RoomInfo{
public:
    RoomInfo(long long id, QString roomName, int roomPort, RoomTYPE roomType, int maxUsers, QList<UserInfo> users, int maxChannels=0, QString streamUrl="");
    RoomInfo(QString roomName, int roomPort, RoomTYPE roomType, int maxUsers, int maxChannels=0);
    RoomInfo(const RoomInfo& other);
    ~RoomInfo(){}
    inline QString getName() const{return name;}
    inline RoomTYPE getType() const{return type;}
    bool isEmpty() const;
    inline bool isFull() const{return users.size() >= maxUsers;}
    inline int getPort() const{return port;}
    //bool containsBotsOnly() const;
    inline QList<UserInfo> getUsers() const{return users;}
    inline bool hasStream() const{return !streamUrl.isEmpty();}
    inline long long getID() const{return id;}
    inline QString getStreamUrl() const{return streamUrl;}
    inline int getMaxChannels() const {return maxChannels;}
    int getNonBotUsersCount() const;
protected:
    long long id;
    QString name;
    int port;
    RoomTYPE type;
    int maxUsers;
    int maxChannels;
    QList<UserInfo> users;
    QString streamUrl;

};

//+++++++++++++++++++++++++++++++++++++++++++++++++++
class LoginService : public QObject
{

Q_OBJECT

public:

    explicit LoginService(QObject *parent=0);
    ~LoginService();
    virtual void connectInServer(QString userName, int instrumentID, QString channelName, const NatMap &localPeerMap, QString version, QString environment, int sampleRate);
    virtual void disconnectFromServer();
    inline bool isConnected() const {return connected;}

signals:
    void roomsListAvailable(QList<Login::RoomInfo> publicRooms);
    void disconnectedFromServer();
    void incompatibilityWithServerDetected();
    void newVersionAvailableForDownload();
    void errorWhenConnectingToServer();
private:

    enum Command{
         CONNECT, DISCONNECT, REFRESH_ROOMS_LIST
     };

    QNetworkAccessManager httpClient;
    QNetworkReply* pendingReply;
    QNetworkReply* sendCommandToServer(const QUrlQuery&);
    static const bool LOCAL_HOST_MODE = false;
    static const QString SERVER;
    bool connected;
    void handleJson(QString json);

    RoomInfo buildRoomInfoFromJson(QJsonObject json);

    static const int REFRESH_PERIOD = 30000;
    QTimer* refreshTimer;

private slots:
    void connectedSlot();
    void disconnectedSlot();
    void roomsListReceivedSlot();

    void errorSlot(QNetworkReply::NetworkError);
    //void sslErrorsSlot(QNetworkReply*,QList<QSslError>);
    void connectNetworkReplySlots(QNetworkReply *reply, Command command);

    void refreshTimerSlot();

};

}

//++++++++++
