#include "ServerMessages.h"
#include <QDebug>

ServerMessage::ServerMessage(ServerMessageType::MessageType messageType)
    :messageType(messageType){
    //
}

ServerMessage::~ServerMessage(){

}

//++++++++++++++++++++++++++++++++++

ServerAuthChallengeMessage::ServerAuthChallengeMessage(int serverKeepAlivePeriod, quint8 challenge[], QString licenceAgreement, int protocolVersion )
    :ServerMessage(ServerMessageType::AUTH_CHALLENGE),
      challenge(QString((const char*)challenge)),
      licenceAgreement(licenceAgreement),
      serverKeepAlivePeriod(serverKeepAlivePeriod),
      protocolVersion(protocolVersion)
{
    //System.arraycopy(challenge, 0, this.challenge, 0, challenge.length);
}

void ServerAuthChallengeMessage::printDebug(QDebug dbg)
{
    dbg << "ServerAuthChallengeMessage{" << endl
                  << "\t challenge=" << getChallenge() << endl
                  << "\t protocolVersion=" << getProtocolVersion()<< endl
                  << "\t serverKeepAlivePeriod=" << getServerKeepAlivePeriod() << endl
                  <<"}" << endl;
}

QDebug operator<<(QDebug dbg, ServerMessage* message)
{
    message->printDebug(dbg);
    return dbg;
}
