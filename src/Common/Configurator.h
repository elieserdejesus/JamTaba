#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QString>
#include <QDir>
#include <QScopedPointer>

// Define the preprocessor macro to get the app version in Jamtaba.
const QString VERSION = "2.1.12";
#define APP_VERSION VERSION

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
    QString getPresetPath(const QString &JsonFile); // used by Settings
    QStringList getPresetFilesNames(bool fullpath); // all the files names in a folder
    void deletePreset(const QString &name);

    QStringList getPreviousLogContent() const;

protected:
    static QDir createPresetsDir(const QDir &baseDir);

private:
    static const QString PRESETS_FOLDER_NAME;
    static const QString CACHE_FOLDER_NAME;
    static const QString LOG_CONFIG_FILE_NAME;
    static const QString THEMES_FOLDER_NAME;
    static const QString THEMES_FOLDER_IN_RESOURCES;

    static const QString LOG_FILE;

    QString logConfigFileName; // the name of the json config file

    QDir cacheDir;
    QDir presetsDir;
    QDir baseDir;
    QDir themesDir;

    bool logFileCreated;

    QStringList lastLogFileContent;

    static QScopedPointer<Configurator> instance; // using a QScopedPointer to auto delete the singleton instance and avoid leak

    Configurator(); // private constructor for Singleton

    void createFoldersTree();
    void initializeDirs(); // this function is implemented in ConfiguratorStandalone.cpp and in ConfiguratorVST.cpp. Only the correct .cpp file is included in .pro files.
    void exportLogIniFile();
    void setupLogConfigFile();

    void exportThemes() const;

    void exportThemeFile(const QString &themeName, const QFileInfo &source, const QFileInfo &destination) const;

    static QDir getApplicationDataDir();

    static void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    static void terminateHandler();
    static void signalHandler(int signal);

    static QString getDebugColor(const QMessageLogContext &context);

    QStringList loadPreviousLogContent() const;
};

inline QStringList Configurator::getPreviousLogContent() const
{
    return lastLogFileContent;
}

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
