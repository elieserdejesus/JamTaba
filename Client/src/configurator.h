#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>
#include <QString>
#include <QStandardPaths>
#include <QDir>
#include <QScopedPointer>

enum APPTYPE{standalone,plugin};

//! Configurator class for Jamtaba !
//! Easy to use , it is intended to create the folders tree in the user local folder.
//! It will create on folder for the plugin version , where the log file and the config
//! file ( Jamtaba.Json )
class Configurator
{
private :

    static const QString VST_PLUGIN_FOLDER_NAME;

    QString IniFilename;//!contains the name of the Ini file
    APPTYPE AppType ;//! Contains the type of configuration used : standalone or plugin
    QString HomePath;//! The full path of the root directory in QString format
    QString Pluginpath;//! The full path of the plugin directory in QString format
    QDir buildDirPaths();//! Used to create the paths of the folder's tree
    void createTree();

    static void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    //private constructor
    Configurator();//!Default constructor
    static QScopedPointer<Configurator> instance;//using a QScopedPointer to auto delete and avoid leak

    bool logFileCreated;
public:
    static Configurator* getInstance();
    ~Configurator();

    inline bool logFileIsCreated() const{ return logFileCreated; }
    inline void setFileCreatedFlag(){logFileCreated = true;}

    bool setUp(APPTYPE Type);
    void exportIniFile();
    void setupIni();
    QString getIniFilePath();
    //check if Jamtaba 2 folder exists in application data
    bool treeExists();
    bool pluginDirExists();
    //! Retrieve the root dir
    QDir getHomeDir() { QDir dir(getHomeDirPath()) ;return dir;}
    QDir getPluginDir()  ;
    inline QString getHomeDirPath(){return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);}
    inline QString getPluginDirPath(){return getPluginDir().path() ;}
    inline APPTYPE getAppType(){return AppType; }
    inline void setAppType(APPTYPE type){AppType=type; }

};
#endif // CONFIGURATOR_H
