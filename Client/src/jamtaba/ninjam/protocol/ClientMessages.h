#pragma once

#include <QByteArray>
#include <QString>
#include <QDebug>



namespace Ninjam {

class User;


//+++++++++++++++++++++++++++
class ClientMessage {

public:
    ClientMessage(quint8 msgCode, quint32 payload);
    virtual ~ClientMessage(){}
    virtual void serializeTo(QByteArray& buffer) = 0;
    void virtual printDebug(QDebug dbg) const = 0;

    inline quint8 getMsgType() const{return msgType;}
    inline quint32 getPayload() const{return payload;}
protected:
    static void serializeString(const QString& str, QDataStream& stream);
    static void serializeByteArray(const QByteArray& array, QDataStream& stream);
    quint8 msgType;
    quint32 payload;
};


//++++++++++++++++++++++++++++++++++++++=
//++++++++++++++++++++++++++++++++++++++=
class ClientAuthUserMessage : public ClientMessage {
private:
    QByteArray passwordHash;
    QString userName;
    quint32 clientCapabilites;
    quint32 protocolVersion;
    QByteArray challenge;
public:
    ClientAuthUserMessage(QString userName, QByteArray challenge, quint32 protocolVersion, QString password = "");
    void serializeTo(QByteArray& buffer) ;
    void virtual printDebug(QDebug dbg) const;
};
//+++++++++++++++++++++++++++++++++++++++=
class ClientSetChannel : public ClientMessage {

private:
    //static const quint8 messageType = 0x82;
    //quint32 payloadLenght;
    QStringList channelNames;// = {"channel" + '\0'};//, "Canal 2" + '\0'};
    quint16 volume;// = 0;
    quint8 pan;// = 0;
    quint8 flags;// = 0;//????

public:
    virtual void serializeTo(QByteArray &stream);
    virtual void printDebug(QDebug dbg) const;
    explicit ClientSetChannel(QString channelName);
};
//++++++++++++++++++++++++++
class ClientKeepAlive : public ClientMessage{

public:
    ClientKeepAlive();
    virtual void serializeTo(QByteArray &stream);
    virtual void printDebug(QDebug dbg) const;

};
//++++++++++++++++++++++++++++++

class ClientSetUserMask : public ClientMessage {

private:
    QStringList usersFullNames;
    static const quint32 FLAG = 0xFFFFFFFF;
public:
    explicit ClientSetUserMask(QList<QString> users) ;
    virtual void serializeTo(QByteArray &stream);
    virtual void printDebug(QDebug dbg) const;

};

//+++++++++++++++++++++++++++
class ChatMessage : public ClientMessage{
public:
    explicit ChatMessage(QString text);
    virtual void serializeTo(QByteArray &stream);
    virtual void printDebug(QDebug dbg) const;
private:
    QString text;
    QString command;
};

QDebug operator<<(QDebug dbg, Ninjam::ClientMessage* message);

}


