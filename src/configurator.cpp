#include "configurator.h"
#include <QFile>
#include <QDebug>
#include <QApplication>

#include "log/logging.h"

QScopedPointer<Configurator> Configurator::instance(nullptr);

const QString Configurator::VST_PLUGIN_FOLDER_NAME = "PluginVst";
const QString Configurator::STANDALONE_PRESET_FOLDER_NAME= "Presets";
const QString Configurator::PLUGIN_PRESET_FOLDER_NAME= "PluginVst/Presets";

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

    //Depends if standalone or plugin ....
    QString path;
    Configurator* configurator = Configurator::getInstance();
    APPTYPE appType= configurator->getAppType();
    if (appType==standalone)
        path=configurator->getHomeDir().absoluteFilePath("log.txt");
    else if (appType==plugin)
        path=configurator->getPluginDir().absoluteFilePath("log.txt");

    QFile outFile(path );
    QIODevice::OpenMode ioFlags = QIODevice::WriteOnly;
    if(configurator->logFileIsCreated()){
        ioFlags |= QIODevice::Append;
    }
    else{
        ioFlags |= QIODevice::Truncate;
        configurator->setFileCreatedFlag();
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
Configurator::Configurator()
    :IniFilename("logging.ini"), logFileCreated(false){

}

//-------------------------------------------------------------------------------
Configurator* Configurator::getInstance(){
    if(Configurator::instance.isNull()){
        instance.reset( new Configurator());
    }
    return instance.data();
}

//-------------------------------------------------------------------------------
QDir Configurator::getPluginDir() const {
    QDir dir=getHomeDir();
    if(!dir.cd(VST_PLUGIN_FOLDER_NAME)){
        qCCritical(jtConfigurator) << "Cant' cd to " + VST_PLUGIN_FOLDER_NAME + " dir";
    }
    return dir;
}
//-------------------------------------------------------------------------------
//Presets now ....
QDir Configurator::getPresetsDir() const
{
    QDir dir=getHomeDir();
    switch(AppType)
    {
    case standalone:
        if(!dir.cd(STANDALONE_PRESET_FOLDER_NAME))
        {
            qCCritical(jtConfigurator) << "Cant' cd to " + STANDALONE_PRESET_FOLDER_NAME + " dir";
        }break;
    case plugin:
        if(!dir.cd(PLUGIN_PRESET_FOLDER_NAME))
        {
            qCCritical(jtConfigurator) << "Cant' cd to " + PLUGIN_PRESET_FOLDER_NAME + " dir";
        }break;
    default:break;
    }


    return dir;
}


QStringList  Configurator::getPresetFilesNames(bool fullpath)
{
    QStringList filesPaths;
    QDir dir=getPresetsDir();
    qInfo(jtConfigurator) << "Looking in Presets dir : "<<dir.absolutePath();
     int count=0;//how many preset files
    foreach(QFileInfo item, dir.entryInfoList() )
       {
           //if(item.isDir())
           //    qDebug() << "Dir: " << item.absoluteFilePath();
           if(item.isFile())
           {
               if(fullpath)
                   filesPaths.append(item.absoluteFilePath());
               else
               filesPaths.append(item.fileName());
               qDebug(jtConfigurator) << "File Found: " << item.absoluteFilePath();

           }
       }
    count=filesPaths.size();
    qInfo(jtConfigurator) << "Presets found : "<<count;
    return filesPaths;
}

//--------------------------------------------------------------------------------------------
bool Configurator::setUp(APPTYPE Type)
{
    AppType=Type;//plugin or standalone

    if(!treeExists()){createTree();}
    exportIniFile();
    setupIni();
    return true;
}
void Configurator::setupIni()
{
    QString iniFilePath = getIniFilePath();
    if(!iniFilePath.isEmpty())
    {
        qputenv("QT_LOGGING_CONF", QByteArray(iniFilePath.toUtf8()));
        qInstallMessageHandler(&Configurator::LogHandler);
    }
}

//-------------------------------------------------------------------------------
QDir Configurator::buildDirPaths()
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
    QDir d=buildDirPaths();
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

    //now the presets for stand alone
    d.mkpath(STANDALONE_PRESET_FOLDER_NAME);
    if( d.exists(STANDALONE_PRESET_FOLDER_NAME))
        qWarning(jtConfigurator) << " PRESETS folder CREATED in :"<<STANDALONE_PRESET_FOLDER_NAME;
    else
        qWarning(jtConfigurator) << " PRESETS folder NOT CREATED in !"<<STANDALONE_PRESET_FOLDER_NAME;

    //now the presets for plugin
    d.mkpath(PLUGIN_PRESET_FOLDER_NAME);
    if( d.exists(PLUGIN_PRESET_FOLDER_NAME))
        qWarning(jtConfigurator) << " PRESETS folder CREATED in :"<<PLUGIN_PRESET_FOLDER_NAME;
    else
        qWarning(jtConfigurator) << " PRESETS folder NOT CREATED in !"<<PLUGIN_PRESET_FOLDER_NAME;

}

bool Configurator::pluginDirExists() const{
   /* if(!treeExists()){
        return false;
    }*/
    QDir homeDir = getHomeDir();
    return homeDir.cd(VST_PLUGIN_FOLDER_NAME);
}
bool Configurator::presetsDirExists() const
{
    QDir homeDir = getHomeDir();
   if(AppType==standalone)
    return homeDir.cd(STANDALONE_PRESET_FOLDER_NAME);
   else
    return homeDir.cd(PLUGIN_PRESET_FOLDER_NAME);
}

bool Configurator::treeExists() const
{
    QDir d(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if(!d.exists()|| !pluginDirExists()|| !presetsDirExists())
    {qWarning(jtConfigurator) << " FOLDER'S TREE don't exist ! :" ;
        return false;
    }
    return true ;

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

QString Configurator::getIniFilePath() const
{
    //HOMEPATH...
    //QDir logDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    QDir iniDir(getHomeDirPath());

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
//Retrieve the path for Settings class
QString Configurator::getPresetPath(QString JsonFile)
{
    QString path("");
    //FIRST CHECK THE DIR
    if (!presetsDirExists())
    {   qDebug(jtConfigurator) << "Can't load preset file , preset dir inexistent !" ;
        return path ;//Settings MUST CHECK THAT
    }

    QDir presetDir=getPresetsDir();
    //SEARCH IN THE DIR
    //USE CONFIGURATOR FOR FILES STUFF
    path=presetDir.absoluteFilePath(JsonFile);
    QStringList list=getPresetFilesNames(true);
    foreach(QString item, list )
       {
        if(item.contains(path))
        {qDebug(jtConfigurator) << "Path to required preset is :" << path;
            return path;
        }
       }

    qDebug(jtConfigurator) << "NO Path found !" << path;
    return path;
}
void Configurator::deletePreset(QString name)
{
    QString s=getPresetPath(name);
    if(s.size()==0)return;
    {
        qInfo(jtConfigurator) << "!!! Preset to delete Path is " << s;
        if(getPresetsDir().remove(s))
            qInfo(jtConfigurator) << "!!! Preset deleted ! ";
        else
            qInfo(jtConfigurator) << "!!! Could not delete Preset ! ";
    }
}

//-------------------------------------------------------------------------------

Configurator::~Configurator()
{
    //qCDebug(jtConfigurator) << "Configurator destructor!";
}
//-------------------------------------------------------------------------------

