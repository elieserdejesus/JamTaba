#include "gui/mainframe.h"
#include "MainController.h"
#include "JamtabaFactory.h"
#include "persistence/Settings.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QDir>


#include "../audio/vst/VstPlugin.h"
#include "../audio/vst/vsthost.h"

void customLogHandler(QtMsgType, const QMessageLogContext &, const QString &);

int main(int argc, char* args[] ){
//    Vst::VstHost* host = Vst::VstHost::getInstance();
//    Vst::VstPlugin plugin(host);
//    bool result = plugin.load(host, "C:\\Program Files (x86)\\VSTPlugins\\4Front Bass x64.dll");
//    qWarning() << "result: " << result;
//    return 0;

    QApplication::setApplicationName("Jamtaba 2");
    QApplication::setApplicationVersion(APP_VERSION);

    qputenv("QT_LOGGING_CONF", ":/qtlogging.ini");//log cconfigurations is in resources at moment

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
    QString stringMsg;
    QString fullFileName(context.file);
    const char* file;
    int lastPathSeparatorIndex = fullFileName.lastIndexOf(QDir::separator());
    if(lastPathSeparatorIndex){
        file = fullFileName.right(fullFileName.size() - lastPathSeparatorIndex - 1).toStdString().c_str();
    }
    else{
        file = fullFileName.toStdString().c_str();
    }

    QTextStream stream(&stringMsg);
    switch (type) {
    case QtDebugMsg:
        stream << "\nDEBUG:" << localMsg.constData() << file << context.line;
        break;
    case QtWarningMsg:
        stream << "\n\nWARNING: " << localMsg.constData() << context.function << file << context.line;
        break;
    case QtCriticalMsg:
        stream << "\n\nCRITICAL:" << localMsg.constData() << context.function << file << context.line << "\n\n";
        break;
    case QtFatalMsg:
        stream << "\n\nFATAL:" << localMsg.constData() << context.function << file << context.line << "\n\n";
        //abort();
        break;
    //case QtInfoMsg:
    //    stream << "\n\nINFO:" << localMsg.constData() << file << context.line << "\n\n";
    }
    QTextStream(stdout) << stringMsg << endl;
    //fprintf(stderr, stringMsg.toStdString().c_str());//write log message to console
    //fflush(stderr);

    if(type != QtDebugMsg){//write the critical messages to log file
        QFile outFile("log.txt");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << stringMsg << "\n\r";
    }

    if(type == QtFatalMsg){
        abort();
    }
}

