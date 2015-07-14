#include "ClientMessages.h"
#include "../User.h"
#include <QCryptographicHash>
#include <QIODevice>
#include <cassert>
#include <QDebug>
//#include <QUuid>
//++++++++++++++

using namespace Ninjam;

ClientMessage::ClientMessage(quint8 msgCode, quint32 payload)
    :msgType(msgCode), payload(payload){

}

void ClientMessage::serializeString(const QString &str, QDataStream &stream){
    //serializeByteArray(QByteArray(str.toStdString().c_str()), stream);

    serializeByteArray(str.toUtf8(), stream);
    stream << quint8('\0'); // NUL TERMINATED
}

void ClientMessage::serializeByteArray(const QByteArray &array, QDataStream &stream){
    //qDebug() << "serializando " << array.size() << " bytes para " << array <<endl;
    for (int i = 0; i < array.size(); ++i) {
        stream << quint8(array[i]);
    }
}

//++++++++++++++++++++++++++++++++++++++=
//class ClientAuthUser
/*
     //message type 0x80

     Offset Type        Field
     0x0    uint8_t[20] Password Hash (binary hash value)
     0x14   ...         Username (NUL-terminated)
     x+0x0  uint32_t    Client Capabilities
     x+0x4  uint32_t    Client Version

     The Password Hash field is calculated by SHA1(SHA1(username + ":" + password) + challenge).
     If the user acknowledged a license agreement from the server then Client Capabilities bit 0 is set.
     The server responds with Server Auth Reply.

    //message lenght = 20 bytes password hash + user name lengh + 4 bytes client capabilites + 4 bytes client version
*/

ClientAuthUserMessage::ClientAuthUserMessage(QString userName, QByteArray challenge, quint32 protocolVersion, QString password)
    : ClientMessage(0x80, 0),
      userName(userName),
      clientCapabilites(1),
      protocolVersion(protocolVersion),
      challenge(challenge)
{
    if (!password.isNull() && !password.isEmpty()) {
        this->userName = userName;
    }
    else{
        this->userName = "anonymous:" + userName;
    }

    QCryptographicHash sha1(QCryptographicHash::Sha1);
    sha1.addData( userName.toStdString().c_str(), userName.size() );
    sha1.addData(":", 1);
    sha1.addData(password.toStdString().c_str(), password.size());
    QByteArray passHash = sha1.result();
    sha1.reset();
    sha1.addData(passHash);
    sha1.addData(challenge.constData(), 8);
    this->passwordHash = sha1.result();
    this->payload = 29 + this->userName.size();
}

void ClientAuthUserMessage::serializeTo(QByteArray& buffer) {
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << msgType << payload;
    serializeByteArray(passwordHash, stream);
    serializeString(userName, stream);
    stream << clientCapabilites;
    stream << protocolVersion;
}
//+++++++++++++++++++++++++++++++++++++++++++
void ClientAuthUserMessage::printDebug(QDebug dbg) const
{
    dbg << "SEND ClientAuthUserMessage{  userName:" << userName << " challenge:" << challenge <<"}" << endl;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ClientSetChannel::ClientSetChannel(QString channelName)//handling just one channel at moment
    : ClientMessage(0x82, 0), volume(0), pan(0), flags(0)
{
    payload = 2;
    channelNames.append(channelName);
    for (int i = 0; i < channelNames.size(); i++) {
        payload += (channelNames[i].size() + 1) + 2 + 1 + 1;//NUL + volume(short) + pan(byte) + flags(byte)
    }
}

void ClientSetChannel::serializeTo(QByteArray &buffer){
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    //payload = 0;
    stream << msgType << payload;
    //++++++++
    stream << quint16(4); //byteBuffer.putShort((short) 4);//parameter size (4 bytes - volume (2 bytes) + pan (1 byte) + flags (1 byte))
    for (int i = 0; i < channelNames.size(); ++i) {
        serializeString(channelNames[i], stream);// //byteBuffer.put(channelNames[i].getBytes("utf-8"));
        stream << volume;// byteBuffer.putShort(volume);
        stream << pan;//byteBuffer.put(pan);
        stream << flags;//byteBuffer.put(flags);
    }
}

void ClientSetChannel::printDebug(QDebug dbg) const{
    dbg << "SEND ClientSetChannel{ payloadLenght=" << payload << " channelName=" << channelNames << '}' << endl;
}

//+++++++++++++++++++++
ClientKeepAlive::ClientKeepAlive()
    :ClientMessage(0xfd, 0)
{

}

void ClientKeepAlive::serializeTo(QByteArray &buffer){
    //just the header bytes, no payload
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << msgType << payload;
}

void ClientKeepAlive::printDebug(QDebug dbg) const{
    dbg << "SEND {Client KeepAlive}" << endl;
}
//+++++++++++++++++
ClientSetUserMask::ClientSetUserMask(QList<QString> users)
    :ClientMessage(0x81, 0)
{
    payload = 4 * users.size();//4 bytes (int) flag
    foreach (QString userFullName , users) {
        usersFullNames.append(userFullName);
        payload += userFullName.size() + 1;
    }
}

void ClientSetUserMask::serializeTo(QByteArray &buffer)
{
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << msgType;
    stream << payload;
    //++++++++++++  END HEADER ++++++++++++
    foreach (QString userName , usersFullNames) {
        ClientMessage::serializeString(userName, stream);
        stream << FLAG;
    }
}

void ClientSetUserMask::printDebug(QDebug dbg) const
{
    dbg << "SEND ClientSetUserMask{ userNames=" << usersFullNames << " flag=" << FLAG << '}';
}

//+++++++++++++++++++++++++++++

ChatMessage::ChatMessage(QString text)
    : ClientMessage(0xc0, 0), text(text), command("MSG")
{
    payload = text.toUtf8().size() + 1 + command.length() + 1;
}

void ChatMessage::serializeTo(QByteArray &buffer){
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << msgType;
    stream << payload;
    ClientMessage::serializeString(command, stream);
    ClientMessage::serializeString(text, stream);
}

void ChatMessage::printDebug(QDebug dbg) const{
    dbg << "SEND ChatMessage{ payload: " << payload << " " << "command=" << command << " text=" << text << '}';
}

//+++++++++++++++++++++++++

ClientUploadIntervalBegin::ClientUploadIntervalBegin(QByteArray GUID, quint8 channelIndex, QString userName)
    :ClientMessage( 0x83, 16 + 4 + 4 + 1 + userName.size()),
      GUID(GUID),
      estimatedSize(0),
      fourCC{'O', 'G', 'G', 'v'},
      channelIndex(channelIndex),
      userName(userName)
{

}

void ClientUploadIntervalBegin::serializeTo(QByteArray &buffer){
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    //quint32 payload = 16 + 4 + 4 + 1 + userName.size();
    stream << msgType;
    stream << payload;
    stream.writeRawData(GUID, 16);
    stream << estimatedSize;
    stream.writeRawData(fourCC, 4);
    stream << channelIndex;
    stream.writeRawData(userName.toStdString().c_str(), userName.size());

    if((quint32)buffer.size() != payload + 5){
        qCritical() << "wrong size!";
    }
}

void ClientUploadIntervalBegin::printDebug(QDebug dbg) const{
    dbg << "SEND ClientUploadIntervalBegin{ GUID "  << QString(GUID) << " fourCC" << QString(fourCC) << "channelIndex: " << channelIndex << "userName:" << userName << "}";
}

//+++++++++++++++++++++
ClientIntervalUploadWrite::ClientIntervalUploadWrite(QByteArray GUID, QByteArray encodedAudioBuffer, bool isLastPart)
    :ClientMessage(0x84, 16 + 1 + encodedAudioBuffer.size()),
    GUID(GUID),
      encodedAudioBuffer(encodedAudioBuffer),
    isLastPart(isLastPart)
{

}

void ClientIntervalUploadWrite::serializeTo(QByteArray &buffer){
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << msgType;
    stream << payload;

    stream.writeRawData(GUID.data(), 16);
    quint8 intervalCompleted = isLastPart ? (quint8) 1 : (quint8) 0;//If the Flag field bit 0 is set then the upload is complete.
    stream << intervalCompleted;
    stream.writeRawData( encodedAudioBuffer.data(), encodedAudioBuffer.size() );

    assert(buffer.size() == payload + 5);
}


 void ClientIntervalUploadWrite::printDebug(QDebug dbg) const{
    dbg << "SEND ClientIntervalUploadWrite{" << "GUID=" << QString(GUID) << ", encodedAudioBuffer= " << payload << " bytes, isLastPart=" << isLastPart << '}';
}


//+++++++++++++++++++++++++++
QDebug Ninjam::operator<<(QDebug dbg, ClientMessage* message)
{
    message->printDebug(dbg);
    return dbg;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
