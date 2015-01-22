#pragma once

#include <QString>
#include <QMap>
#include <QList>
#include <QObject>
#include <QUrl>
#include <QUrlQuery>

//++++++++++++++++++++++++++++++++++++++++++++++++++++

class LoginServiceResponse{

};

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

//+++++++++++++++++++++++++++++++++++
class LoginService {

protected:
    bool connected;
    
public:
    LoginService();
    virtual ~LoginService(){}

    //void connectInJamRoom(AbstractJamRoom jamRoom, QString password) = 0;

    //void disconnectFromRealTimeRoom(long jamRoomID) = 0;

    virtual void connect(QString userName, int instrumentID, QString channelName, const NatMap& localPeerMap, int version, QString environment, int sampleRate)  = 0;

    virtual void disconnect() = 0;

    inline bool isConnected(){return this->connected;}

    //void createJamRoom(QString jamRoomName, int maxUsers) = 0;

    //Peer getLocalPeer() = 0;

    //QString getServerIp() = 0;

    //void refreshNow() = 0;

    //void changeInstrument(int instrumentID) = 0;
    //void changeChannelName(QString newChannelName) = 0;
    
    //void setNinjamPlayingStatus(bool status);

};

//++++++++++
