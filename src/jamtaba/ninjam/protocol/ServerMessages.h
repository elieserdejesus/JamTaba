#pragma once
#include <QString>
#include "server/parsers/ServerMessageParser.h"
#include <QDebug>

class ServerMessage {

    ServerMessageType::MessageType messageType;
public:
    ServerMessage(ServerMessageType::MessageType messageType);
    virtual ~ServerMessage();
    virtual void printDebug(QDebug dbg) = 0;
    inline ServerMessageType::MessageType getMessageType() const { return messageType;}
};
//++++++++++++++++++++++++++++++++++

class ServerAuthChallengeMessage : public ServerMessage {

    QString challenge;// = new byte[8];
    QString licenceAgreement;
    int serverKeepAlivePeriod;
    int protocolVersion;//The Protocol Version field should contain 0x00020000.

public:
    ServerAuthChallengeMessage(int serverKeepAlivePeriod, quint8 challenge[], QString licenceAgreement, int protocolVersion );

    inline QString getChallenge() const { return challenge; }
    inline int getProtocolVersion() const{ return protocolVersion; }
    inline int getServerKeepAlivePeriod() const{ return serverKeepAlivePeriod; }
    inline bool serverHasLicenceAgreement() const{ return !licenceAgreement.isNull() && !licenceAgreement.isEmpty();}
    inline QString getLicenceAgreement() const { return licenceAgreement; }
    virtual void printDebug(QDebug dbg);
};

QDebug operator<<(QDebug dbg, ServerMessage* c);
//QDataStream &operator<<(QDataStream &out, const ServerAuthChallengeMessage &message);
