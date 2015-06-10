#include "ServerMessages.h"
#include <QDebug>
#include "../User.h"

using namespace Ninjam;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
//+++++++++++++++++++++  SERVER MESSAGE (Base class) ++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
ServerMessage::ServerMessage(ServerMessageType messageType)
    :messageType(messageType){
    //
}

ServerMessage::~ServerMessage(){ }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
//+++++++++++++++++++++  SERVER AUTH CHALLENGE+++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
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
//+++++++++++++++++++++  SERVER KEEP ALIVE ++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
ServerKeepAliveMessage::ServerKeepAliveMessage()
    :ServerMessage(ServerMessageType::KEEP_ALIVE)
{
    //
}

void ServerKeepAliveMessage::printDebug(QDebug dbg) const{
    dbg << "RECEIVED ServerKeepAlive{ }";
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
//+++++++++++++++++++++  SERVER CONFIG CHANGE NOTIFY ++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
ConfigChangeNotifyMessage::ConfigChangeNotifyMessage(quint16 bpm, quint16 bpi)
    :ServerMessage(ServerMessageType::CONFIG_CHANGE_NOTIFY), bpm(bpm), bpi(bpi)
{
    //
}

void ConfigChangeNotifyMessage::printDebug(QDebug dbg) const{
    dbg << "RECEIVE ConfigChangeNotify{ bpm=" << bpm << ", bpi=" << bpi << "}" << endl;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
//+++++++++++++++++++++  SERVER USER INFO CHANGE NOTIFY +++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
UserInfoChangeNotifyMessage::UserInfoChangeNotifyMessage()
    :ServerMessage(ServerMessageType::USER_INFO_CHANGE_NOTIFY)
{

}

UserInfoChangeNotifyMessage::UserInfoChangeNotifyMessage(QMap<User*, QList<UserChannel*>> allUsersChannels)
    :ServerMessage(ServerMessageType::USER_INFO_CHANGE_NOTIFY), usersChannels(allUsersChannels)
{

    //insere o NinjamUserChannel em um shared_ptr para que esses canais sejam automaticamente deletados
//    foreach (NinjamUser* user, allUsersChannels.keys()) {
//        std::vector< std::shared_ptr< NinjamUserChannel>> channels;//vai funcionar? channels não será deletado ao final do escopo?
//        usersChannels.insert(user,  channels);
//        foreach (NinjamUserChannel* channel, allUsersChannels[user]) {
//            channels.push_back(std::shared_ptr<NinjamUserChannel>(channel));
//        }
//    }
}


void UserInfoChangeNotifyMessage::printDebug(QDebug dbg) const{
    dbg << "UserInfoChangeNotify{\n";
    for (User* user : usersChannels.keys()) {
        dbg << "\t" << user << "\n";
    }
    dbg << "}";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++ SERVER CHAT MESSAGE +++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ServerChatMessage::ServerChatMessage(QString command, QStringList arguments)
    :ServerMessage(ServerMessageType::CHAT_MESSAGE),
      commandType(commandTypeFromString(command)),
      arguments(arguments)
{
    //
}

ChatCommandType ServerChatMessage::commandTypeFromString(QString string){
    //"MSG", "PRIVMSG", "TOPIC", "JOIN", "PART", "USERCOUNT"
    if(string == "MSG") return ChatCommandType::MSG;
    if(string == "PRIVMSG") return ChatCommandType::PRIVMSG;
    if(string == "TOPIC") return ChatCommandType::TOPIC;
    if(string == "JOIN") return ChatCommandType::JOIN;
    if(string == "PART") return ChatCommandType::PART;
    /*if(string == "USERCOUNT")*/ return ChatCommandType::USERCOUNT;
}

void ServerChatMessage::printDebug(QDebug dbg) const{
    dbg << "RECEIVE ServerChatMessage{ command=" << (std::uint8_t)commandType << " arguments=" << arguments << "}" << endl;
}

//++++++++++++++++++++++++++++++++++++++++++++++

DownloadIntervalBegin::DownloadIntervalBegin(quint32 estimatedSize, quint8 channelIndex, QString userName, quint8 fourCC[], QByteArray GUID)
    :ServerMessage(ServerMessageType::DOWNLOAD_INTERVAL_BEGIN),
      GUID(GUID),
      estimatedSize(estimatedSize),
      channelIndex(channelIndex),
      userName(userName)
{
    for (int i = 0; i < 4; ++i) {
        this->fourCC[i] = fourCC[i];
    }
    isValidOgg = fourCC[0] == 'O' && fourCC[1] == 'G' && fourCC[2] == 'G' && fourCC[3] == 'v';
}

void DownloadIntervalBegin::printDebug(QDebug dbg) const
{
    dbg << "DownloadIntervalBegin{ " <<endl
        << "\tfourCC='"<< fourCC[0] << fourCC[1] << fourCC[2] << fourCC[3] << endl
        << "\tGUID={"<< GUID << "} " << endl
        << "\tisValidOggDownload="<< isValidOggDownload() << endl
        << "\tdownloadShoudBeStopped="<< downloadShouldBeStopped() << endl
        << "\tdownloadIsComplete="<< downloadIsComplete() << endl
        << "\testimatedSize=" << estimatedSize << endl
        << "\tchannelIndex=" << channelIndex  << endl
        << "\tuserName=" << userName << endl <<"}" << endl;
}


void DownloadIntervalWrite::printDebug(QDebug dbg) const
{
    dbg << "RECEIVE DownloadIntervalWrite{ flags='" << flags << "' GUID={" << GUID << "} downloadIsComplete=" << downloadIsComplete() << ", audioData=" << encodedAudioData.size() << " bytes }";
}

DownloadIntervalWrite::DownloadIntervalWrite(QByteArray GUID, quint8 flags, QByteArray encodedAudioData)
    :ServerMessage(ServerMessageType::DOWNLOAD_INTERVAL_WRITE),
      GUID(GUID),
      flags(flags),
      encodedAudioData(encodedAudioData)
{
    //
}

//++++++++++++++++++

QDebug Ninjam::operator<<(QDebug dbg, ServerMessage* message)
{
    message->printDebug(dbg);
    return dbg;
}
