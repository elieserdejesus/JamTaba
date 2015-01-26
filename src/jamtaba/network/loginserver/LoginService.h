#pragma once


#include <QObject>
class NatMap;
class LoginServiceResponse;

class LoginServiceListener{
 public:
    virtual ~LoginServiceListener(){ }
    virtual void connected(LoginServiceResponse response) = 0;
    virtual void disconnected() = 0;
};

class LoginService {

protected:
    bool connected;
    LoginServiceListener* listener;

public:
    LoginService(LoginServiceListener* listener);
    virtual ~LoginService(){}

    virtual void connectInServer(QString userName, int instrumentID, QString channelName, const NatMap& localPeerMap, int version, QString environment, int sampleRate)  = 0;

    virtual void disconnect() = 0;

    inline bool isConnected(){return this->connected;}

};

//++++++++++
