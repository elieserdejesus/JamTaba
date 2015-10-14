#include "logHandler.h"
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

static bool logFileCreated = false;

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

    //if(type != QtDebugMsg){//write the critical messages to log file
        QDir logDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
        if(!logDir.exists()){
            logDir.mkpath(".");
        }
        QFile outFile( logDir.absoluteFilePath("log.txt"));
        QIODevice::OpenMode ioFlags = QIODevice::WriteOnly;
        if(logFileCreated)
            ioFlags |= QIODevice::Append;
        else{
            ioFlags |= QIODevice::Truncate;
            logFileCreated = true;
        }
        if(outFile.open(ioFlags)){
           QTextStream ts(&outFile);
           ts << stringMsg << endl;
        }
    //}

    if(type == QtFatalMsg){
        abort();
    }
}
