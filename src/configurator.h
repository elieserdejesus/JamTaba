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
    static const QString STANDALONE_PRESET_FOLDER_NAME;
    static const QString PLUGIN_PRESET_FOLDER_NAME ;

    QString IniFilename;//!contains the name of the Ini file
    APPTYPE AppType ;//! Contains the type of configuration used : standalone or plugin
    QString HomePath;//! The full path of the root directory in QString format
    QString Pluginpath;//! The full path of the plugin directory in QString format
    QDir buildDirPaths() ;//! Used to create the paths of the folder's tree
    void createTree();

    static void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    //private constructor
    Configurator();//!Default constructor
    static QScopedPointer<Configurator> instance;//using a QScopedPointer to auto delete and avoid leak

    bool logFileCreated;

    void exportIniFile();
    void setupIni();
public:
    static Configurator* getInstance();
    ~Configurator();

    inline bool logFileIsCreated() const{ return logFileCreated; }
    inline void setFileCreatedFlag(){logFileCreated = true;}

    bool setUp(APPTYPE Type);
    QString getIniFilePath() const;
    bool treeExists() const;    //check if Jamtaba 2 folder exists in application data
    bool pluginDirExists() const;
    bool presetsDirExists() const;
    inline QDir getHomeDir() const{ return QDir(getHomeDirPath());}
    QDir getPluginDir() const ;
    inline QString getHomeDirPath() const {return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);}
    inline QString getPluginDirPath() const {return getPluginDir().path() ;}
    inline APPTYPE getAppType() const{ return AppType; }
    //inline void setAppType(APPTYPE type){AppType=type; }
    //Presets now ....
    QDir getPresetsDir() const ;
    QString getPresetPath(QString JsonFile);//used by Settings
    QStringList getPresetFilesNames(bool fullpath);//all the files names in a folder
    void deletePreset(QString name);


};
#endif // CONFIGURATOR_H
