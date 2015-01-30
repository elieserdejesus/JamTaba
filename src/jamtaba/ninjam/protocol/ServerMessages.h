#pragma once
#include <QString>
#include "server/parsers/ServerMessageParser.h"
#include <QDebug>

class ServerMessage {

    ServerMessageType::MessageType messageType;
public:
    ServerMessage(ServerMessageType::MessageType messageType);
    virtual ~ServerMessage();
    virtual void printDebug(QDebug dbg) const = 0;
    inline ServerMessageType::MessageType getMessageType() const { return messageType;}
};
//++++++++++++++++++++++++++++++++++

class ServerAuthChallengeMessage : public ServerMessage {

    quint8 challenge[];// = new byte[8];
    QString licenceAgreement;
    int serverKeepAlivePeriod;
    int protocolVersion;//The Protocol Version field should contain 0x00020000.

public:
    ServerAuthChallengeMessage(int serverKeepAlivePeriod, quint8 challenge[], QString licenceAgreement, int protocolVersion );

    inline QByteArray getChallenge() const { return QByteArray((const char*)challenge); }
    inline int getProtocolVersion() const{ return protocolVersion; }
    inline int getServerKeepAlivePeriod() const{ return serverKeepAlivePeriod; }
    inline bool serverHasLicenceAgreement() const{ return !licenceAgreement.isNull() && !licenceAgreement.isEmpty();}
    inline QString getLicenceAgreement() const { return licenceAgreement; }
    virtual void printDebug(QDebug dbg) const;
};
//++++++++++++++++++++++++++++++++
class ServerAuthReplyMessage : public ServerMessage{
private:
    quint8 flag;
    QString message;
    quint8 maxChannels;

public:
    ServerAuthReplyMessage(quint8 flag, quint8 maxChannels, QString responseMessage) ;
    virtual void printDebug(QDebug debug) const;
    inline QString getErrorMessage() const { return message;}

    inline QString getNewUserName() const{
        if(!userIsAuthenticated()){
            qCritical("user is note authenticated!");
        }
        return message;
    }
    inline bool userIsAuthenticated() const{ return flag == 1; }
    inline quint8 getMaxChannels() const{ return maxChannels; }

};
//+++++++++++++++++++++++++++++++
QDebug operator<<(QDebug dbg, ServerMessage* c);
//QDataStream &operator<<(QDataStream &out, const ServerAuthChallengeMessage &message);
