#include "DefaultLoginService.h"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QObject>
#include <QEventLoop>
#include "../model/JamRoom.h"
#include "LoginServiceResponse.h"

const QString DefaultLoginService::SERVER = "https://jamtaba-music.appspot.com/vs";

//+++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++

DefaultLoginService::DefaultLoginService(LoginServiceListener *listener)
     : QObject(0), LoginService(listener)
{
    pendingReply = NULL;
}

DefaultLoginService::~DefaultLoginService()
{
    //disconnect();
}

void DefaultLoginService::connectInServer(QString userName, int instrumentID, QString channelName, const NatMap &localPeerMap, int version, QString environment, int sampleRate)
{
    QUrlQuery query = HttpParamsFactory::parametersToConnect(userName, instrumentID, channelName, localPeerMap, version, environment, sampleRate);
    pendingReply = sendCommandToServer(query);
    connectNetworkReplySlots(pendingReply, LoginServiceCommands::CONNECT);

}

void DefaultLoginService::disconnect()
{
    if(isConnected()){
        qDebug() << "DISCONNECTING from server...";
        QUrlQuery query = HttpParamsFactory::parametersToDisconnect();
        pendingReply = sendCommandToServer(query);
        connectNetworkReplySlots(pendingReply, LoginServiceCommands::DISCONNECT);
    }
}

QNetworkReply* DefaultLoginService::sendCommandToServer(const QUrlQuery &query)
{
    if(pendingReply != NULL){
        pendingReply->deleteLater();
    }
    QUrl url(SERVER);
    QByteArray postData(query.toString(QUrl::EncodeUnicode).toStdString().c_str());
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded; charset=utf-8"));
    return httpClient.post(request, postData);
}


void DefaultLoginService::disconnectedSlot(){
    this->connected = false;
    qDebug() << "calling listener->disconnected()!";
    listener->disconnected();
}

void DefaultLoginService::sslErrorsSlot(QList<QSslError> errorList){
    foreach (const QSslError& error, errorList) {
        qDebug() << error;
    }
}

void DefaultLoginService::errorSlot(QNetworkReply::NetworkError /*error*/){
    if(pendingReply != NULL){
        qFatal(pendingReply->errorString().toStdString().c_str());
    }
}

void DefaultLoginService::connectNetworkReplySlots(QNetworkReply* reply, LoginServiceCommands::COMMAND command)
{
    switch (command) {
    case LoginServiceCommands::CONNECT: reply->connect(reply, SIGNAL(finished()), this, SLOT(connectedSlot()));  break;
    case LoginServiceCommands::DISCONNECT: reply->connect(reply, SIGNAL(finished()), this, SLOT(disconnectedSlot()));  break;
    default:
        break;
    }
    reply->connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),  this, SLOT(errorSlot(QNetworkReply::NetworkError)));
    reply->connect(reply, SIGNAL(sslErrors(QList<QSslError>)),  this, SLOT(sslErrorsSlot(QList<QSslError>)));
}

void DefaultLoginService::connectedSlot(){
    //qDebug() << "connected slot!" << pendingReply->readAll();
    connected = true;
    listener->connected(LoginServiceResponse( QString( pendingReply->readAll()) ));
}

