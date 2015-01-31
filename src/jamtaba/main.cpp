#include "gui/mainframe.h"
#include "MainController.h"
#include "JamtabaFactory.h"
#include "network/loginserver/DefaultLoginService.h"
#include "ninjam/NinjamService.h"
#include "nvwa/debug_new.h"

int main(int argc, char* args[])
{

    qInstallMessageHandler(customLogHandler);
    JamtabaFactory* factory = new ReleaseFactory();
    MainController mainController(factory, argc, args);//MainController extends QApplication

    MainFrame w(&mainController);
    w.show();

    delete factory;

    //NinjamService* ninjamService = NinjamService::getInstance();
    //ninjamService->startServerConnection("ninbot.com", 2049);

    return mainController.exec();
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
