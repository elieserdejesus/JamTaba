#include "NinjamService.h"
#include <QApplication>
#include "protocol/ClientMessages.h"
#include "protocol/server/parsers/ServerMessageParser.h"
#include "protocol/ServerMessages.h"
#include <QTcpSocket>
#include <QCryptographicHash>

void customLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char* args[])
{
    QApplication application(argc, args);

    NinjamService* service = NinjamService::getInstance();
    QStringList channels("channel test");
    service->startServerConnection("localhost", 2049, "test", channels);


    return application.exec();
}

//++++++++++++++++++++++++++++++++++

/*
    QTcpSocket socket;
    quint8 msgCode;
    quint32 payload;

    socket.connectToHost("localhost", 2049);
    if(!socket.waitForConnected()){
        qFatal("não conectou");
    }
    qDebug() << "conectou!";

    if(!socket.waitForReadyRead()){
        qFatal("não conseguiu ler os bytes");
    }

    QDataStream stream(socket.readAll());
    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> msgCode;
    stream >> payload;

    //auth challenge
    ServerMessageParser* authChallengeParser = ServerMessageParser::getParser(ServerMessageType::AUTH_CHALLENGE);
    ServerAuthChallengeMessage* authChallengeMsg = (ServerAuthChallengeMessage*)authChallengeParser->parse(stream, payload);
    qDebug() << authChallengeMsg;

    ClientAuthUserMessage authUserMessage("test", authChallengeMsg->getChallenge(), authChallengeMsg->getProtocolVersion() );
    QByteArray out;
    authUserMessage.serializeTo(out);
    int writed = socket.write(out);
    qDebug() << "enviou " << writed << "bytes";
    qDebug() << &authUserMessage;

    if(!socket.waitForBytesWritten()){
        qFatal("não escreveu os bytes");
    }

    if(!socket.waitForReadyRead()){
        qFatal("não conseguiu ler");
    }

    QDataStream stream2(socket.readAll());
    stream2.setByteOrder(QDataStream::LittleEndian);
    stream2 >> msgCode;
    stream2 >> payload;
    ServerAuthReplyMessage* serverAuthReply = (ServerAuthReplyMessage*)ServerMessageParser::getParser(ServerMessageType::AUTH_REPLY)->parse(stream2, payload);
    qDebug() << serverAuthReply;

    if (serverAuthReply->userIsAuthenticated()) {
        qDebug() << "Users authenticated in server:" << serverAuthReply;
        QString newUserName = serverAuthReply->getNewUserName();

        ClientSetChannel setChannel("test channel");
        out.clear();
        setChannel.serializeTo(out);
        writed = socket.write(out);
        if(!socket.waitForBytesWritten()){
            qFatal("não deu!");
        }
        qDebug() << "writed:" << writed;
        qDebug() << &setChannel;

        if(!socket.waitForReadyRead()){
            qFatal("falhou no final!");
        }

    } else {
        throw serverAuthReply->getErrorMessage();
    }
*/

void customLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "%s \n", localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "\n\nWARNING: %s (%s) [%s:%u]\n\n", localMsg.constData(), context.function, context.file, context.line);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "\n\nCRITICAL: %s (%s) [%s:%u]\n\n", localMsg.constData(), context.function, context.file, context.line);
        break;
    case QtFatalMsg:
        fprintf(stderr, "\n\nFATAL: %s (%s) [%s:%u]\n\n", localMsg.constData(), context.function, context.file, context.line);
        abort();
    }
    fflush(stderr);
}

