#include "NinjamService.h"
#include <QApplication>

void customLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char* args[])
{
    qInstallMessageHandler(customLogHandler);
    QApplication application(argc, args);
    NinjamService* ninjamService = NinjamService::getInstance();
    ninjamService->startServerConnection("ninbot.com", 2049);

    return application.exec();
    //return 0;
}
//++++++++++++++++++++++++++++++++++

void customLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "DEBUG: %s [%s: %u]\n", localMsg.constData(), context.file , context.line);
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
