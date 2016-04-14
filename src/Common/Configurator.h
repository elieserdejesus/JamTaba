#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>
#include <QString>
#include <QStandardPaths>
#include <QDir>
#include <QScopedPointer>

enum AppType {
    STANDALONE, PLUGIN
};

// ! Configurator class for Jamtaba !
// ! Easy to use , it is intended to create the folders tree in the user local folder.
// ! It will create on folder for the plugin version , where the log file and the config
// ! file ( Jamtaba.Json )
class Configurator
{

public:
    static Configurator *getInstance();
    ~Configurator();

    inline bool logFileIsCreated() const
    {
        return logFileCreated;
    }

    inline void setFileCreatedFlag()
    {
        logFileCreated = true;
    }

    bool setUp(AppType appType);

    bool folderTreeExists() const;    // check if Jamtaba 2 folder exists in application data

    inline QDir getPluginDir() const { return pluginDir; }

    inline QDir getCacheDir() const { return cacheDir; }

    QDir getPresetsDir() const;

    QDir getLogConfigFileDir() const;

    QDir getLogDir() const;

    inline AppType getAppType() const
    {
        return appType;
    }

    // Presets
    QString getPresetPath(const QString &JsonFile);// used by Settings
    QStringList getPresetFilesNames(bool fullpath);// all the files names in a folder
    void deletePreset(const QString &name);

private:
    static const QString VST_PLUGIN_FOLDER_NAME;
    static const QString PRESETS_FOLDER_NAME;
    static const QString CACHE_FOLDER_NAME;
    static const QString LOG_CONFIG_FILE_NAME;

    bool presetsDirExists() const;

    QString logConfigFileName;// the name of the json config file
    AppType appType; // type of configuration used : standalone or plugin

    QDir pluginDir; // vst plugin directory
    QDir cacheDir;
    QDir presetsDir; //standalone presets
    QDir pluginPresetsDir; //plugin presets
    QDir baseDir;

    void createFoldersTree();

    static void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    Configurator(); // private constructor for Singleton
    static QScopedPointer<Configurator> instance;// using a QScopedPointer to auto delete and avoid leak

    bool logFileCreated;

    void exportLogIniFile();
    void setupLogConfigFile();

};
#endif // CONFIGURATOR_H
