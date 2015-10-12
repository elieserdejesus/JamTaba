#include "gui/MainWindowStandalone.h"
#include "StandAloneMainController.h"
#include "persistence/Settings.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QDir>
#include <QStandardPaths>


void customLogHandler(QtMsgType, const QMessageLogContext &, const QString &);

int main(int argc, char* args[] ){

    QApplication::setApplicationName("Jamtaba 2");
    QApplication::setApplicationVersion(APP_VERSION);

    qputenv("QT_LOGGING_CONF", ":/Standalone/qtlogging.ini");//log cconfigurations is in resources at moment

    qInstallMessageHandler(customLogHandler);

    Persistence::Settings settings;//read from file in constructor
    settings.load();

    Controller::StandaloneMainController mainController(settings, argc, args);//MainController extends QApplication
    mainController.configureStyleSheet("jamtaba.css");
    MainWindowStandalone  mainWindow(&mainController);
    mainController.setMainWindow(&mainWindow);
    mainWindow.show();

    int returnCode = mainController.exec();
    mainController.saveLastUserSettings(mainWindow.getInputsSettings());

    return returnCode;

 }

//++++++++++++++++++++++++++++++++++

void customLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString stringMsg;
    QString fullFileName(context.file);
    QString file;
    int lastPathSeparatorIndex = fullFileName.lastIndexOf(QDir::separator());
    if(lastPathSeparatorIndex){
        file = fullFileName.right(fullFileName.size() - lastPathSeparatorIndex - 1);//.toStdString().c_str();
    }
    else{
        file = fullFileName;//.toStdString().c_str();
    }

    QTextStream stream(&stringMsg);
    switch (type) {
    case QtDebugMsg:
        stream << "\nDEBUG:" << localMsg.constData() << " "  << " in " << file << " " << context.line << endl;
        break;
    case QtWarningMsg:
        stream << "\n\nWARNING: " << localMsg.constData() <<  context.function <<  " " << file << context.line << endl;
        break;
    case QtCriticalMsg:
        stream << "\n\nCRITICAL:" << localMsg.constData() <<  context.function << " " << file << context.line << endl <<endl;
        break;
    case QtFatalMsg:
        stream << "\n\nFATAL:" << localMsg.constData() << context.function << file << context.line << endl << endl;
        //abort();
        break;
    default:
       stream << "\n\nINFO:" << localMsg.constData() << file << context.line << endl << endl;
    }
    QTextStream(stdout) << stringMsg << endl;

    if(type != QtDebugMsg){//write the critical messages to log file
        QDir logDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
        if(!logDir.exists()){
            logDir.mkpath(".");
        }
        QFile outFile( logDir.absoluteFilePath("log.txt"));
        if(outFile.open(QIODevice::WriteOnly | QIODevice::Append)){
           QTextStream ts(&outFile);
           ts << stringMsg << endl;
        }
    }

    if(type == QtFatalMsg){
        abort();
    }
}


