#include "configurator.h"
//#include <QDateTime>
#include <QStandardPaths>
//#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QApplication>
//#include <QSysInfo>
//#include <QUuid>
//#include <QSettings>
#include <QDir>

#include "../log/logging.h"


Configurator::Configurator()
{
 logFilename = "logging.ini";
}

//copy the logging.ini from resources to application writable path, so user can tweak the Jamtaba log
void Configurator::exportLogFile()
{
    QDir logDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if(logDir.exists())
    {
        QString logConfigFilePath = logDir.absoluteFilePath(logFilename);
        if(!QFile(logConfigFilePath).exists())
        {//log config file in application directory? (same dir as json config files, cache.bin, etc.)
            bool result = QFile::copy(":/" + logFilename, logConfigFilePath ) ;
            if(result)
            {
                QFile loggingFile(logConfigFilePath);
                loggingFile.setPermissions(QFile::WriteOther);//The file is writable by anyone.
            }
        }
    }
    else
    {
        qWarning(jtCore) << "Log folder not exists!" << logDir.absolutePath();
    }
}

void Configurator::LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
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
        stream << context.category << ".DEBUG:  " << localMsg.constData() << " "  << " in " << file << " " << context.line << endl;
        break;
    case QtWarningMsg:
        stream << context.category << ".WARNING:  " << localMsg.constData() <<  context.function <<  " " << file << context.line << endl << endl;
        break;
    case QtCriticalMsg:
        stream << context.category << ".CRITICAL:  " << localMsg.constData() <<  context.function << " " << file << context.line << endl << endl;
        break;
    case QtFatalMsg:
        stream << context.category  << ".FATAL:  " << localMsg.constData() << context.function << file << context.line << endl << endl;
        break;
    default:
       stream << context.category << ".INFO:  " << localMsg.constData() <<endl;
    }
    QTextStream(stdout) << stringMsg;

    QDir logDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if(!logDir.exists()){
        //check if we are plugin or standalone
        QString s=QApplication::instance()->applicationName();
        qDebug() << "Jamtaba Instance:" << s;
        logDir.mkpath("s");
    }
    QFile outFile( logDir.absoluteFilePath("logg.txt"));
    QIODevice::OpenMode ioFlags = QIODevice::WriteOnly;
    if(logFileCreated)
        ioFlags |= QIODevice::Append;
    else{
        ioFlags |= QIODevice::Truncate;
        logFileCreated = true;
    }
    if(outFile.open(ioFlags)){
        QTextStream ts(&outFile);
        ts << stringMsg;
    }

    if(type == QtFatalMsg){
        abort();
    }
}
Configurator::~Configurator()
{

}
