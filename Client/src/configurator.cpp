#include "configurator.h"
//#include <QDateTime>

//#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QApplication>
//#include <QSysInfo>
//#include <QUuid>
//#include <QSettings>


#include "../log/logging.h"

 static bool logFileCreated = false;
 extern Configurator *JTBConfig;

void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
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

    /*QDir logDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if(!logDir.exists()){
        //check if we are plugin or standalone
        QString s=QApplication::instance()->applicationName();
        qDebug() << "Jamtaba Instance:" << s;
        logDir.mkpath(".");
    }*/
    //JTBConfig->homeExists();

    QFile outFile( JTBConfig->getHomeDir().absoluteFilePath("log.txt"));
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

//--------------------------------Configurator-----------------------------------
//
//-------------------------------------------------------------------------------

Configurator::Configurator()
{
 logFilename = "logging.ini";

}
//-------------------------------------------------------------------------------

bool Configurator::setUp(APPTYPE type)
{
 AppType=type;//plugin or standalone
 //find the directory and store the name
 if(!homeExists())createTree();
  exportIniFile();
  return true;
}
//-------------------------------------------------------------------------------
void Configurator::createTree()
{
    //create the folder
    QDir d(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    HomeDir.Dir=d;
    HomeDir.HomePath=d.currentPath();
    qDebug(jtCore) << "Configurator: Creating PluginVst folder...";
    HomeDir.Dir.mkpath("PluginVst");

    HomeDir.Pluginpath=HomeDir.Dir.absoluteFilePath("PluginVst");
    if( HomeDir.Dir.exists(HomeDir.HomePath))
        qDebug(jtCore) << "Configurator: HomePath folder CREATED !";
    else
        qDebug(jtCore) << "Configurator: HomePath folder NOT CREATED !";
    if( HomeDir.Dir.exists(HomeDir.Pluginpath))
        qDebug(jtCore) << "Configurator: PluginVst folder CREATED !";
    else
        qDebug(jtCore) << "Configurator: PluginVst folder NOT CREATED !";

}

bool Configurator::homeExists()
{
    QDir d(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    return d.exists();
}

//-------------------------------------------------------------------------------

//copy the logging.ini from resources to application writable path, so user can tweak the Jamtaba log
void Configurator::exportIniFile()
{

        QString FilePath = HomeDir.Dir.absoluteFilePath(logFilename);
        if(!QFile(FilePath).exists())
        {//log config file in application directory? (same dir as json config files, cache.bin, etc.)
            bool result = QFile::copy(":/" + logFilename,FilePath ) ;
            if(result)
            {
                QFile loggingFile(FilePath);
                loggingFile.setPermissions(QFile::WriteOther);//The file is writable by anyone.
            }
        }


}
//-------------------------------------------------------------------------------

QString Configurator::getIniFilePath()
{
   QDir logDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if(logDir.exists())
    {
        QString logConfigFilePath = logDir.absoluteFilePath(logFilename);
        if(QFile(logConfigFilePath).exists()){//log config file in application directory? (same dir as json config files, cache.bin, etc.)
            return logConfigFilePath;
        }
        else{//search log config file in resources
            //qDebug(jtCore) << "Log file not founded in release folder (" <<logDir.absolutePath() << "), searching in resources...";
            logConfigFilePath = ":/" + logFilename ;
            if(QFile(logConfigFilePath).exists()){
                qDebug(jtCore) << "Log file founded in resources...";
                return logConfigFilePath;
            }
            qWarning(jtCore) << "Log file not founded in source code tree: " << logDir.absolutePath();
        }
    }
    qWarning(jtCore) << "Log folder not exists!" << logDir.absolutePath();
    return "";

}
//-------------------------------------------------------------------------------

Configurator::~Configurator()
{

}
//-------------------------------------------------------------------------------

