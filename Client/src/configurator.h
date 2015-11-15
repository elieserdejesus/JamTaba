#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>
#include <QString>
#include <QStandardPaths>
#include <QDir>


void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

//!Not very useful struct now , should be deprecated soon
//struct SHOMEDIR{bool exist;};
enum APPTYPE{standalone,plugin};

//! Configurator class for Jamtaba !
//! Easy to use , it is intended to create the folders tree in the user local folder.
//! It will create on folder for the plugin version , where the log file and the config
//! file ( Jamtaba.Json )
class Configurator
{
private :
    QString IniFilename;//!contains the name of the Ini file
    //SHOMEDIR HomeDir;
    APPTYPE AppType ;//! Contains the type of configuration used : standalone or plugin
    QString HomePath;//! The full path of the root directory in QString format
    QString Pluginpath;//! The full path of the plugin directory in QString format
    QDir createDirPaths();//! Used to create the paths of the folder's tree
    void createTree();

public:
  Configurator();//!Default constructor

    bool setUp(APPTYPE Type);
      void exportIniFile();
        void setupIni();
        QString getIniFilePath();
         //check if Jamtaba 2 folder exists in application data
         bool homeExists();
         //! Retrieve the root dir
        inline QDir getHomeDir(){ QDir dir(getHomeDirPath()) ;return dir;}
       inline QDir getPluginDir(){QDir dir=getHomeDir();dir.cd("PluginVst");return dir ;}
      inline QString getHomeDirPath(){return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);}
     inline QString getPluginDirPath(){return getPluginDir().path() ;}
   inline APPTYPE getAppType(){return AppType; }
  inline void setAppType(APPTYPE type){AppType=type; }


    ~Configurator();

};
extern Configurator *JTBConfig;
#endif // CONFIGURATOR_H
