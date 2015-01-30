#pragma once

#include <QByteArray>
#include <QString>
#include <QDebug>

//+++++++++++++++++++++++++++
class ClientMessage {

public:
    virtual void serializeTo(QByteArray& buffer) = 0;
    virtual ~ClientMessage(){}
    void virtual printDebug(QDebug dbg) const = 0;
protected:
    static void serializeString(const QString& str, QDataStream& stream);
    static void serializeByteArray(const QByteArray& array, QDataStream& stream);
};

QDebug operator<<(QDebug dbg, ClientMessage* message);

//++++++++++++++++++++++++++++++++++++++=
//++++++++++++++++++++++++++++++++++++++=
class ClientAuthUserMessage : public ClientMessage {
private:
    QByteArray passwordHash;
    QString userName;

    quint32 clientCapabilites;
    quint8 messageType;
    quint32 payloadLenght;
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
    static const quint8 messageType = 0x82;
    quint32 payloadLenght;
    QStringList channelNames;// = {"channel" + '\0'};//, "Canal 2" + '\0'};
    quint16 volume;// = 0;
    quint8 pan;// = 0;
    quint8 flags;// = 0;//????

public:
    virtual void serializeTo(QByteArray &buffer);
    virtual void printDebug(QDebug dbg) const;
    ClientSetChannel(QString channelName);
};
