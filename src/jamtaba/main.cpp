#include "gui/MainFrame.h"
#include "MainController.h"
#include "JamtabaFactory.h"
#include "persistence/Settings.h"
#include <QTextCodec>

void customLogHandler(QtMsgType, const QMessageLogContext &, const QString &);

int main(int argc, char* args[] ){
    QApplication::setApplicationName("Jamtaba 2");

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    qInstallMessageHandler(customLogHandler);

    JamtabaFactory* factory = new ReleaseFactory();
    Persistence::Settings settings;//read from file in constructor
    settings.load();

    Controller::MainController mainController(factory, settings, argc, args);//MainController extends QApplication
    MainFrame mainFrame(&mainController);
    mainFrame.show();

    delete factory;
    int returnCode = mainController.exec();
    mainController.saveLastUserSettings(mainFrame.getInputsSettings());
    return returnCode;
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

