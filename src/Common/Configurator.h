#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QString>
#include <QDir>
#include <QScopedPointer>

// ! Configurator class for Jamtaba !
// ! Easy to use , it is intended to create the folders tree in the user local folder.
// ! It will create on folder for the plugin version , where the log file and the config
// ! file ( Jamtaba.Json )
class Configurator
{

public:
    static Configurator *getInstance();

    ~Configurator();

    bool logFileIsCreated() const;
    void setFileCreatedFlag();

    bool setUp();

    bool folderTreeExists() const; // check if Jamtaba 2 folder exists in application data

    QDir getCacheDir() const;
    QDir getPresetsDir() const;
    QDir getBaseDir() const;
    QDir getThemesDir() const;

    // Presets
    QString getPresetPath(const QString &JsonFile);// used by Settings
    QStringList getPresetFilesNames(bool fullpath);// all the files names in a folder
    void deletePreset(const QString &name);

private:
    static const QString PRESETS_FOLDER_NAME;
    static const QString CACHE_FOLDER_NAME;
    static const QString LOG_CONFIG_FILE_NAME;
    static const QString THEMES_FOLDER_NAME;
    static const QString THEMES_FOLDER_IN_RESOURCES;

    QString logConfigFileName;// the name of the json config file

    QDir cacheDir;
    QDir presetsDir;
    QDir baseDir;
    QDir themesDir;

    bool logFileCreated;

    static QScopedPointer<Configurator> instance;// using a QScopedPointer to auto delete the singleton instance and avoid leak

    Configurator(); // private constructor for Singleton

    void createFoldersTree();
    void initializeDirs(); //this function is implemented in ConfiguratorStandalone.cpp and in ConfiguratorVST.cpp. Only the correct .cpp file is included in .pro files.
    void exportLogIniFile();
    void setupLogConfigFile();
    bool needExportThemes() const;
    void exportThemes() const;

    static QDir getApplicationDataDir();

    static void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};


inline bool Configurator::logFileIsCreated() const
{
    return logFileCreated;
}

inline void Configurator::setFileCreatedFlag()
{
    logFileCreated = true;
}

inline QDir Configurator::getCacheDir() const
{
    return cacheDir;
}

inline QDir Configurator::getPresetsDir() const
{
    return presetsDir;
}

inline QDir Configurator::getBaseDir() const
{
    return baseDir;
}

inline QDir Configurator::getThemesDir() const
{
    return themesDir;
}

#endif // CONFIGURATOR_H
