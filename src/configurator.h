#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>
#include <QString>
#include <QStandardPaths>
#include <QDir>

void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

struct SHOMEDIR{bool exist;QDir Dir;QString HomePath;QString Pluginpath;};
enum APPTYPE{standalone,plugin};

class Configurator
{
private :
    QString IniFilename;//contains the name of the Ini file
    SHOMEDIR HomeDir;
    APPTYPE AppType ;
    void createTree();

public:
    Configurator();
    bool setUp(APPTYPE type);

        void exportIniFile();
        void setupIni();
        QString getIniFilePath();

        //check if Jamtaba 2 folder exists in application data
         bool homeExists();
         inline QDir getHomeDir(){return HomeDir.Dir ;}
         inline QString getHomeDirPath(){return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);}
         inline QString getPluginDirPath(){return HomeDir.Pluginpath ;}
         inline APPTYPE getAppType(){return AppType; }
         inline void setAppType(APPTYPE type){AppType=type; }


    ~Configurator();

};
extern Configurator *JTBConfig;
#endif // CONFIGURATOR_H
