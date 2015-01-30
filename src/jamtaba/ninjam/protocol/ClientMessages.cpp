#include "ClientMessages.h"

#include <QCryptographicHash>
#include <QIODevice>
#include <QDebug>
//++++++++++++++
void ClientMessage::serializeString(const QString &str, QDataStream &stream){
    serializeByteArray(QByteArray(str.toStdString().c_str()), stream);
    stream << quint8('\0'); // NUL TERMINATED
}

void ClientMessage::serializeByteArray(const QByteArray &array, QDataStream &stream){
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
    :
      userName(userName),
      clientCapabilites(1),
      messageType(0x80),
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
    this->payloadLenght = 29 + this->userName.size();
}

void ClientAuthUserMessage::serializeTo(QByteArray& buffer) {
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << messageType;
    stream << payloadLenght;
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
ClientSetChannel::ClientSetChannel(QString channelName)
    :volume(0), pan(0), flags(0), payloadLenght(0)
{
    payloadLenght = 2;
    channelNames.append(channelName);
    for (int i = 0; i < channelNames.size(); i++) {//handling just one channel at moment
        payloadLenght += (channelNames[i].size() + 1) + 2 + 1 + 1;//NUL + volume(short) + pan(byte) + flags(byte)
    }
}

void ClientSetChannel::serializeTo(QByteArray &buffer){
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << messageType;
    stream << payloadLenght;
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
    dbg << "SEND ClientSetChannel{ payloadLenght=" << payloadLenght << " channelName=" << channelNames << '}' << endl;
}

//+++++++++++++++++++++
QDebug operator<<(QDebug dbg, ClientMessage* message)
{
    message->printDebug(dbg);
    return dbg;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
