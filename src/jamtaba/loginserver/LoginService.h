#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <memory>

class NatMap;

namespace Login {

class AbstractJamRoom;

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

//++++++++++++++++++

class LoginService : public QObject
{

Q_OBJECT

public:
    enum class Command{
         CONNECT, DISCONNECT
     };

signals:
    void connectedInServer(QList<Login::AbstractJamRoom*> rooms);
    void disconnectedFromServer();

    //talvez seja melhor esquecer esse evento, simplesmente entrega uma nova lista de salas
    void roomChanged(const AbstractJamRoom& room);

private:
    static const bool LOCAL_HOST_MODE = true;
    static const QString SERVER;
    static QMap<long long, std::shared_ptr<AbstractJamRoom>> rooms;
    bool connected;
    void updateFromJson(QString json);
    QList<AbstractJamRoom*> buildTheRoomsList();

private slots:
    void connectedSlot();
    void disconnectedSlot();
    void errorSlot(QNetworkReply::NetworkError);
    void sslErrorsSlot(QList<QSslError>);
    void connectNetworkReplySlots(QNetworkReply *reply, Command command);

public:
    explicit LoginService(QObject *parent=0);
    ~LoginService();
    virtual void connectInServer(QString userName, int instrumentID, QString channelName, const NatMap &localPeerMap, int version, QString environment, int sampleRate);
    virtual void disconnect();

    inline bool isConnected() const {return connected;}



private:
    QNetworkAccessManager httpClient;
    QNetworkReply* pendingReply;
    QNetworkReply* sendCommandToServer(const QUrlQuery&);
};

}

//++++++++++
