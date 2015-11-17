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


const QString Configurator::VST_PLUGIN_FOLDER_NAME = "PluginVst";

bool logFileCreated = false;
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

    //Depends if standalone or plugin ....
    QString path;
    APPTYPE appType= JTBConfig->getAppType();
    if (appType==standalone)
        path=JTBConfig->getHomeDir().absoluteFilePath("log.txt");
    else if (appType==plugin)
        path=JTBConfig->getPluginDir().absoluteFilePath("log.txt");
    //qDebug(jtConfigurator)<<"LOG PATH :"<<path;
    // QDebug(jtConfigurator)

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

    if(appType == QtFatalMsg){
        abort();
    }
}

//--------------------------------Configurator-----------------------------------
//
//-------------------------------------------------------------------------------

Configurator::Configurator(){IniFilename = "logging.ini";}

//-------------------------------------------------------------------------------

QDir Configurator::getPluginDir() {
    QDir dir=getHomeDir();
    if(!dir.cd(VST_PLUGIN_FOLDER_NAME)){
        qCCritical(jtConfigurator) << "Cant' cd to " + VST_PLUGIN_FOLDER_NAME + " dir";
    }
    return dir;
}

bool Configurator::setUp(APPTYPE Type)
{
    AppType=Type;//plugin or standalone

    if(!treeExists() || !pluginDirExists()){
        createTree();
    }
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
QDir Configurator::createDirPaths()
{
    //create the Home folder
    QDir d(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    HomePath=d.path();
    Pluginpath=d.absoluteFilePath(VST_PLUGIN_FOLDER_NAME);
    return d;
}

void Configurator::createTree()
{
    qWarning(jtConfigurator) << " Creating folders tree...";
    QDir d=createDirPaths();
    d.mkpath("PluginVst");
    if( d.exists(HomePath))
        qWarning(jtConfigurator) << " Home folder CREATED in :"<<HomePath;
    else
        qWarning(jtConfigurator) << " Home folder NOT CREATED in !"<<HomePath;

    //HomeDir.Pluginpath=d.absoluteFilePath("PluginVst");

    if( d.exists(Pluginpath))
        qWarning(jtConfigurator) << " PluginVst folder CREATED in :" <<Pluginpath;
    else
        qWarning(jtConfigurator) << " PluginVst folder NOT CREATED in :" <<Pluginpath;

}

bool Configurator::pluginDirExists(){
    if(!treeExists()){
        return false;
    }
    QDir homeDir = getHomeDir();
    return homeDir.cd(VST_PLUGIN_FOLDER_NAME);
}

bool Configurator::treeExists()
{
    QDir d(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if(!d.exists())
    {qWarning(jtConfigurator) << " HOME folder don't exist ! :" ;
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------

//copy the logging.ini from resources to application writable path, so user can tweak the Jamtaba log
void Configurator::exportIniFile()
{
    //we use one ini file ( in Home/) to all types for log
    QString FilePath = getHomeDir().absoluteFilePath(IniFilename);
    //but we want the log to be in the right folder
    if(!QFile(FilePath).exists())
    {
        qDebug(jtConfigurator) << "Ini file don't exist in' :"<<FilePath ;

        bool result;

        //log config file in application directory? (same dir as json config files, cache.bin, etc.)
        // bool result = QFile::copy(":/" + IniFilename,FilePath ) ;
        result = QFile::copy(":/" + IniFilename, FilePath ) ;
        if(result)
        {
            qDebug(jtConfigurator) << "Ini file copied in :"<<FilePath ;
            QFile loggingFile(FilePath);
            loggingFile.setPermissions(QFile::WriteOther);//The file is writable by anyone.
        }
        else  qDebug(jtConfigurator) << "FAILED to copy Ini file in :"<<FilePath ;
    }
}
//-------------------------------------------------------------------------------

QString Configurator::getIniFilePath()
{
    //HOMEPATH...
    //QDir logDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    QDir iniDir(JTBConfig->getHomeDirPath());

    if(iniDir.exists())
    {
        QString iniFilePath = iniDir.absoluteFilePath(IniFilename);
        if(QFile(iniFilePath).exists()){//log config file in application directory? (same dir as json config files, cache.bin, etc.)
            return iniFilePath;
        }
        else{//search log config file in resources
            //qDebug(jtCore) << "Log file not founded in release folder (" <<logDir.absolutePath() << "), searching in resources...";
            iniFilePath = ":/" + IniFilename ;
            if(QFile(iniFilePath).exists()){
                qDebug(jtConfigurator) << "Ini file founded in resources...";
                return iniFilePath;
            }
            qDebug(jtConfigurator) << "Ini file not founded in source code tree: " << iniDir.absolutePath();
        }
    }
    qDebug(jtConfigurator) << "ini folder not exists!" << iniDir.absolutePath();
    return "";

}
//-------------------------------------------------------------------------------

Configurator::~Configurator()
{

}
//-------------------------------------------------------------------------------

