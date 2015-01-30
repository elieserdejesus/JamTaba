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
      //challenge(QString((const char*)challenge)),
      licenceAgreement(licenceAgreement),
      serverKeepAlivePeriod(serverKeepAlivePeriod),
      protocolVersion(protocolVersion)
{
    for (int i = 0; i < 8; ++i) {
        this->challenge[i] = challenge[i];
    }
    //System.arraycopy(challenge, 0, this.challenge, 0, challenge.length);
}

void ServerAuthChallengeMessage::printDebug(QDebug dbg) const
{
    dbg << "RECEIVED ServerAuthChallengeMessage{" << endl
                  << "\t challenge=" << getChallenge() << endl
                  << "\t protocolVersion=" << getProtocolVersion()<< endl
                  << "\t serverKeepAlivePeriod=" << getServerKeepAlivePeriod() << endl
                  <<"}" << endl;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
//+++++++++++++++++++++  SERVER AUTH REPLY ++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
ServerAuthReplyMessage::ServerAuthReplyMessage(quint8 flag, quint8 maxChannels, QString responseMessage)
    :ServerMessage(ServerMessageType::AUTH_REPLY),
      flag(flag), message(responseMessage), maxChannels(maxChannels)
{
    //
}

void ServerAuthReplyMessage::printDebug(QDebug debug) const{
    debug << "RECEIVED ServerAuthReply{ flag=" << flag << " errorMessage='" << message << "' maxChannels=" << maxChannels << '}' << endl;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
QDebug operator<<(QDebug dbg, ServerMessage* message)
{
    message->printDebug(dbg);
    return dbg;
}
