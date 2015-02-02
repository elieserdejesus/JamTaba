#include "Service.h"
#include <QApplication>
#include "protocol/ClientMessages.h"
#include "protocol/ServerMessageParser.h"
#include "protocol/ServerMessages.h"
#include "Server.h"
#include "User.h"
#include <QTcpSocket>
#include <QCryptographicHash>

//#include "nvwa/debug_new.h"

void customLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

class Listener : public Ninjam::ServiceListenerAdapter{
public:
    virtual void connectedInServer(const Ninjam::Server& server){
        qDebug() << "Evento connected in server: " << server.getHostName();
    }

    virtual void userChannelCreated(const Ninjam::User &user, const Ninjam::UserChannel &channel){
        qDebug() << "canal criado " << channel.getName() << endl << user << endl;
    }
    virtual void userChannelRemoved(const Ninjam::User &user, const Ninjam::UserChannel &channel){
        qDebug() << "canal removido " << channel.getName() << endl << user << endl;
    }
    virtual void userChannelUpdated(const Ninjam::User &user, const Ninjam::UserChannel &channel){
        qDebug() << "canal modificado " << channel.getName() << endl << user << endl;
    }
    virtual void userCountMessageReceived(int /*users*/, int /*maxUsers*/){}
    virtual void serverBpiChanged(short currentBpi, short lastBpi){
        qDebug() << "BPI CHANGED current:" << currentBpi << " old:" << lastBpi << endl;
    }
    virtual void serverBpmChanged(short currentBpm){
        qDebug() << "BPM CHANGED " << currentBpm << endl;
    }
    virtual void audioIntervalPartAvailable(const Ninjam::User &user, int channelIndex, QByteArray encodedAudioData, bool lastPartOfInterval){
        qDebug() << "AUDIO AVAILABLE for " << user.getName() << " channel:"<< channelIndex << "lastPart: " << lastPartOfInterval << " bytes:" << encodedAudioData.size();
    }
    virtual void disconnectedFromServer(bool /*normalDisconnection*/){
        qDebug() << "DISCONNECTED FROM SERVER!";
    }
    virtual void chatMessageReceived(const Ninjam::User &sender, QString message){
        qDebug() << "CHAT MESSAGE " << sender.getName() <<":" << message;
    }
    virtual void privateMessageReceived(const Ninjam::User &sender, QString message){
        chatMessageReceived(sender, message);
    }
    virtual void userEnterInTheJam(const Ninjam::User &newUser){
        qDebug() << newUser.getName() << " enter in the jam" << endl;
    }
    virtual void userLeaveTheJam(const Ninjam::User &user){
        qDebug() << user.getName() << " leave the jam" << endl;
    }
    virtual void error(QString msg){
        qCritical(msg.toStdString().c_str());
    }
};

int main(int argc, char* args[])
{
    QApplication application(argc, args);

//    QTimer timer(&application);
//         timer.connect(&timer, SIGNAL(timeout()), &application, SLOT(quit()));
//         timer.start(10000);

    Ninjam::Service* service = Ninjam::Service::getInstance();
    //service->addListener(new Listener());
    QStringList channels("channel test");
    service->startServerConnection("localhost", 2049, "test", channels);

    return application.exec();
}

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


