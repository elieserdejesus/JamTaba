#pragma once

#include <QByteArray>
#include <QString>
#include <QCryptographicHash>
#include <QIODevice>

//namespace ClientMessageType {
//    enum MessageType{
//        AUTH_USER = (byte) 0x80,
//        KEEP_ALIVE = (byte) 0xfd,
//        UPLOAD_INTERVAL_BEGIN = (byte) 0x83,
//        UPLOAD_INTERVAL_WRITE = (byte) 0x84
//    };
//}
//+++++++++++++++++++++++++++

class ClientMessage {

public:
    virtual void serializeTo(QByteArray& buffer) = 0;
    virtual ~ClientMessage(){}
};
//++++++++++++++++++++++++++++++++++++++=
//++++++++++++++++++++++++++++++++++++++=
class ClientAuthUser : public ClientMessage {
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
     */
private:
    QByteArray passwordHash;
    QString userName;

    quint32 clientCapabilites;
    quint8 messageType;
    quint32 payloadLenght;
    quint32 protocolVersion;

    //message lenght = 20 bytes password hash + user name lengh + 4 bytes client capabilites + 4 bytes client version
public:
    ClientAuthUser(QString userName, QString challenge, quint32 protocolVersion, QString password = "")
        :userName(userName), clientCapabilites(1), messageType(0x80), protocolVersion(protocolVersion)
    {
        if (!password.isNull() && !password.isEmpty()) {
            this->userName = userName;
        }
        else{
            this->userName = "anonymous:" + userName;
        }

        QCryptographicHash sha1(QCryptographicHash::Sha1);
        sha1.addData(QString(this->userName + ":" + password).toUtf8());
        QByteArray passHash = sha1.result();
        //byte passHash[] = encrypt((this->userName + ":" + password).getBytes("UTF-8"));//  DigestUtils.sha1Hex(this->userName + ":" + password);

        QByteArray hashToEncrypt;
        hashToEncrypt.append(passHash);
        hashToEncrypt.append(challenge);

        //byte[] hashToEncrypt = new byte[passHash.length + challenge.length];
        //System.arraycopy(passHash, 0, hashToEncrypt, 0, passHash.length);
        //System.arraycopy(challenge, 0, hashToEncrypt, passHash.length, challenge.length);

        sha1.reset();
        sha1.addData(hashToEncrypt);
        this->passwordHash = sha1.result();

        está desconectando, então acho que meu hash sha1 não está funcionando.

        //this->passwordHash = encrypt(hashToEncrypt);//DigestUtils.sha1(firstPass + new String(challenge, "UTF-8"));
        this->payloadLenght = 29 + this->userName.size();//passwordHash.length + this->userName.length() + 4 + 4;
    }

    void serializeTo(QByteArray& buffer) {
        QDataStream stream(&buffer, QIODevice::WriteOnly);
        Q_ASSERT(buffer.size() == 0);

        stream << messageType;//byteBuffer.put(messageType);
        Q_ASSERT(buffer.size() == 1);

        stream << payloadLenght;//byteBuffer.putInt(payloadLenght);
        Q_ASSERT(buffer.size() == 5);

        for (int i = 0; i < 20; ++i) {//byteBuffer.put(passwordHash);//20 bytes
            stream << (quint8)passwordHash[i];
        }
        QString str("buffer.size() = " + QString::number(buffer.size()));
        Q_ASSERT_X(buffer.size() == 25, "ERRO", str.toStdString().c_str());

        QByteArray nulTerminatedName = (userName + '\0').toUtf8();
        //byteBuffer.put(nulTerminatedName.getBytes("UTF-8"));
        for (int i = 0; i < nulTerminatedName.size(); ++i) {
            stream << (quint8)nulTerminatedName[i];
        }

        Q_ASSERT(buffer.size() == 25 + nulTerminatedName.size());

        stream << clientCapabilites;//byteBuffer.putInt(clientCapabilites);
        Q_ASSERT(buffer.size() == 25 + nulTerminatedName.size() + 4);

        stream << protocolVersion; //byteBuffer.putInt(protocolVersion);
        Q_ASSERT(buffer.size() == 25 + nulTerminatedName.size() + 4 + 4);

        Q_ASSERT( buffer.size() == payloadLenght + 5);
    }



};
