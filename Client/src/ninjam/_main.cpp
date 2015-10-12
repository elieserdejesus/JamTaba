#include "Service.h"
#include <QApplication>
#include "protocol/ClientMessages.h"
#include "protocol/ServerMessageParser.h"
#include "protocol/ServerMessages.h"
#include "Server.h"
#include "Service.h"
#include "User.h"
#include <QTcpSocket>
#include <QCryptographicHash>
#include <QTimer>

//void customLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char* args[])
{

    QApplication application(argc, args);

    QTimer timer(&application);
         timer.connect(&timer, SIGNAL(timeout()), &application, SLOT(quit()));
         timer.start(60000);

    Ninjam::Service* service = Ninjam::Service::getInstance();

    QStringList channels("channel test");
    service->startServerConnection("localhost", 2049, "test", channels);

    return application.exec();

}

//void customLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
//{
//    QByteArray localMsg = msg.toLocal8Bit();
//    switch (type) {
//    case QtDebugMsg:
//        fprintf(stderr, "%s \n", localMsg.constData());
//        break;
//    case QtWarningMsg:
//        fprintf(stderr, "\n\nWARNING: %s (%s) [%s:%u]\n\n", localMsg.constData(), context.function, context.file, context.line);
//        break;
//    case QtCriticalMsg:
//        fprintf(stderr, "\n\nCRITICAL: %s (%s) [%s:%u]\n\n", localMsg.constData(), context.function, context.file, context.line);
//        break;
//    case QtFatalMsg:
//        fprintf(stderr, "\n\nFATAL: %s (%s) [%s:%u]\n\n", localMsg.constData(), context.function, context.file, context.line);
//        abort();
//    }
//    fflush(stderr);
//}


