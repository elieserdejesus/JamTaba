#include "gui/MainFrame.h"
#include "MainController.h"
#include "JamtabaFactory.h"
#include "loginserver/LoginService.h"
#include "ninjam/Service.h"
#include "JamRoomViewPanel.h"
#include "audio/core/AudioDriver.h"
#include <QThread>

void customLogHandler(QtMsgType, const QMessageLogContext &, const QString &);

//extern "C"{
//    #include "minimp3.h"
//}

int main(int argc, char* args[] )
{

//    mp3_decoder_t decoder = mp3_create();

//    mp3_info_t info;
//    int bytesDecoded = mp3_decode(&decoder, 0, 0, 0, &info);
//    mp3_done(&decoder);
//    qDebug() << "bytes decoded: " << bytesDecoded;

    qInstallMessageHandler(customLogHandler);

    JamtabaFactory* factory = new ReleaseFactory();
    Controller::MainController mainController(factory, argc, args);//MainController extends QApplication

    MainFrame w(&mainController);
    w.show();

    delete factory;
    return mainController.exec();
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

