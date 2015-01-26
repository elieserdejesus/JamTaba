#ifndef DEFAULTLOGINSERVER_H
#define DEFAULTLOGINSERVER_H

#include "LoginService.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class AbstractJamRoom;
class Peer;
class LoginServiceResponse;

class NatMap{
public:
    QString getPublicIp(){
        return "public ip";
    }

    int getPublicPort(){
        return 30000;
    }
};

//+++++++++++++++++++++++++++++++++++
class HttpParamsFactory{
public:
    static QUrlQuery parametersToConnect(QString userName, int instrumentID, QString channelName, NatMap localPeerMap, int version, QString environment, int sampleRate) {
        QUrlQuery query;
        query.addQueryItem("cmd", "CONNECT");
        query.addQueryItem("userName", userName);
        query.addQueryItem("instrumentID", QString::number(instrumentID));
        query.addQueryItem("channelName", channelName);
        query.addQueryItem("publicIp", localPeerMap.getPublicIp());
        query.addQueryItem("publicPort", QString::number(localPeerMap.getPublicPort()));
        query.addQueryItem("environment", environment );
        query.addQueryItem("version", QString::number(version));
        query.addQueryItem("sampleRate", QString::number(sampleRate));
        return query;
    }

    static QUrlQuery parametersToDisconnect() {
        QUrlQuery query;
        query.addQueryItem("cmd", "DISCONNECT");
        return query;
    }
};


namespace LoginServiceCommands{
    enum COMMAND{
        CONNECT, DISCONNECT
    };
}

class DefaultLoginService : public QObject, public LoginService
{

Q_OBJECT

private:
    static const QString SERVER;

protected slots:
    void connectedSlot();
    void disconnectedSlot();
    void errorSlot(QNetworkReply::NetworkError);
    void sslErrorsSlot(QList<QSslError>);
    void connectNetworkReplySlots(QNetworkReply *reply, LoginServiceCommands::COMMAND command);

public:
    DefaultLoginService(LoginServiceListener* listener);
    ~DefaultLoginService();
    virtual void connectInServer(QString userName, int instrumentID, QString channelName, const NatMap &localPeerMap, int version, QString environment, int sampleRate);
    virtual void disconnect();

private:
    QNetworkAccessManager httpClient;
    QNetworkReply* pendingReply;
    QNetworkReply* sendCommandToServer(const QUrlQuery&);
};

#endif // DEFAULTLOGINSERVER_H
