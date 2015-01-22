#ifndef DEFAULTLOGINSERVER_H
#define DEFAULTLOGINSERVER_H

#include <QObject>
#include "LoginService.h"
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QNetworkReply>


namespace LoginServiceCommands{
    enum COMMAND{
        CONNECT, DISCONNECT
    };
}

class DefaultLoginService : public QObject, public LoginService
{
    Q_OBJECT

private:
    static const QString SERVER;// = "https://jamtaba-music.appspot.com/vs";


public:

    explicit DefaultLoginService(QObject *parent = 0);
    ~DefaultLoginService();
    virtual void connect(QString userName, int instrumentID, QString channelName, const NatMap &localPeerMap, int version, QString environment, int sampleRate);
    virtual void disconnect();

private:
    QNetworkAccessManager httpClient;
    QNetworkReply* pendingReply;
    QNetworkReply* sendCommandToServer(const QUrlQuery&);

private slots:
    void connectedSlot();
    void disconnectedSlot();
    void errorSlot(QNetworkReply::NetworkError);
    void sslErrorsSlot(QList<QSslError>);
    void connectNetworkReplySlots(QNetworkReply *reply, LoginServiceCommands::COMMAND command);

};

#endif // DEFAULTLOGINSERVER_H
