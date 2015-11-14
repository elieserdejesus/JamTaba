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

    //Depends if standalone or plugin ....
    QString path;
    if (JTBConfig->getAppType()==standalone)
    path=JTBConfig->getHomeDir().absoluteFilePath("log.txt");
    QFile outFile(path );
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
 IniFilename = "logging.ini";

}
//-------------------------------------------------------------------------------

bool Configurator::setUp(APPTYPE type)
{
 AppType=type;//plugin or standalone

 if(!homeExists())createTree();
  exportIniFile();
  setupIni();
  return true;
}
void Configurator::setupIni()
{
    QString iniFilePath =JTBConfig->getIniFilePath();
        if(!iniFilePath.isEmpty())
        {
            qputenv("QT_LOGGING_CONF", QByteArray(iniFilePath.toUtf8()));
            qInstallMessageHandler(LogHandler);
        }
}

//-------------------------------------------------------------------------------
void Configurator::createTree()
{
    //create the folder
    QDir d(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    HomeDir.Dir=d;
    HomeDir.HomePath=d.path();
    qWarning(jtConfigurator) << " Creating folders tree...";
    HomeDir.Dir.mkpath("PluginVst");
    if( HomeDir.Dir.exists(HomeDir.HomePath))
        qWarning(jtConfigurator) << " HomePath folder CREATED in :"<<HomeDir.HomePath;
    else
        qWarning(jtConfigurator) << " HomePath folder NOT CREATED in !"<<HomeDir.HomePath;;

    HomeDir.Pluginpath=HomeDir.Dir.absoluteFilePath("PluginVst");

    if( HomeDir.Dir.exists(HomeDir.Pluginpath))
        qWarning(jtConfigurator) << " PluginVst folder CREATED in :" <<HomeDir.Pluginpath;
    else
        qWarning(jtConfigurator) << " PluginVst folder NOT CREATED in :" <<HomeDir.Pluginpath;

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
        //we use one ini file ( in Home/) to all types for log
        QString FilePath = HomeDir.Dir.absoluteFilePath(IniFilename);
        //but we want the log to be in the right folder
        if(!QFile(FilePath).exists())
        {
            qDebug(jtConfigurator) << "Ini file don't exist in' :"<<FilePath ;

            bool result;
            QString path(":/");
            if(AppType==standalone)path+=IniFilename;
            else if(AppType==plugin)path+=HomeDir.Pluginpath+ IniFilename ;
            //log config file in application directory? (same dir as json config files, cache.bin, etc.)
           // bool result = QFile::copy(":/" + IniFilename,FilePath ) ;
            result = QFile::copy(path,FilePath ) ;
            if(result)
            {
                qDebug(jtConfigurator) << "Ini file copied in :"<<FilePath ;
                QFile loggingFile(FilePath);
                loggingFile.setPermissions(QFile::WriteOther);//The file is writable by anyone.
            }
        }


}
//-------------------------------------------------------------------------------

QString Configurator::getIniFilePath()
{
    //HOMEPATH...
   QDir logDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if(logDir.exists())
    {
        QString logConfigFilePath = logDir.absoluteFilePath(IniFilename);
        if(QFile(logConfigFilePath).exists()){//log config file in application directory? (same dir as json config files, cache.bin, etc.)
            return logConfigFilePath;
        }
        else{//search log config file in resources
            //qDebug(jtCore) << "Log file not founded in release folder (" <<logDir.absolutePath() << "), searching in resources...";
            logConfigFilePath = ":/" + IniFilename ;
            if(QFile(logConfigFilePath).exists()){
                qDebug(jtConfigurator) << "Ini file founded in resources...";
                return logConfigFilePath;
            }
            qDebug(jtConfigurator) << "Ini file not founded in source code tree: " << logDir.absolutePath();
        }
    }
    qDebug(jtConfigurator) << "ini folder not exists!" << logDir.absolutePath();
    return "";

}
//-------------------------------------------------------------------------------

Configurator::~Configurator()
{

}
//-------------------------------------------------------------------------------

