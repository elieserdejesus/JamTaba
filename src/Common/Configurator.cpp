#include "Configurator.h"
#include <QFile>
#include <QDebug>
#include <QApplication>
#include <QStandardPaths>
#include <QTime>

#include "log/Logging.h"

QScopedPointer<Configurator> Configurator::instance(nullptr);

const QString Configurator::PRESETS_FOLDER_NAME = "Presets";
const QString Configurator::CACHE_FOLDER_NAME = "Cache";
const QString Configurator::LOG_CONFIG_FILE_NAME = "logging.ini";
const QString Configurator::THEMES_FOLDER_NAME = "Themes";
const QString Configurator::THEMES_FOLDER_IN_RESOURCES = ":/css/themes";

// from https://sites.google.com/a/embeddedlab.org/community/technical-articles/qt/qt-posts/howtodocoloredloggingusingqtdebug
#define COLOR_DEBUG         "\033[35;1m"
#define COLOR_DEBUG_MIDI    "\033[32;1m"
#define COLOR_DEBUG_AUDIO   "\033[35;1m"
#define COLOR_DEBUG_GUI     "\033[36;1m"
#define COLOR_WARN          "\033[33;1m"
#define COLOR_CRITICAL      "\033[31;1m"
#define COLOR_FATAL         "\033[31;1m"
#define COLOR_RESET         "\033[0m"

QString Configurator::getDebugColor(const QMessageLogContext &context)
{
    QString category(context.category);
    if (category == QString(jtMidi().categoryName()))
        return COLOR_DEBUG_MIDI;
    else if (category == QString(jtAudio().categoryName()))
        return COLOR_DEBUG_AUDIO;
    else if (category == QString(jtGUI().categoryName()))
        return COLOR_DEBUG_GUI;

    return COLOR_DEBUG;
}

void Configurator::LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString stringMsg;
    QString fullFileName(context.file);
    QString file;
    int lastPathSeparatorIndex = fullFileName.lastIndexOf(QDir::separator());
    if (lastPathSeparatorIndex)
        file = fullFileName.right(fullFileName.size() - lastPathSeparatorIndex - 1);
    else
        file = fullFileName;

    QString messageType;
    QString messageColor;

    QTextStream stream(&stringMsg);
    switch (type) {
    case QtDebugMsg:    messageType = "DEBUG   ";   messageColor = getDebugColor(context);   break;
    case QtWarningMsg:  messageType = "WARNING ";   messageColor = COLOR_WARN;   break;
    case QtCriticalMsg: messageType = "CRITICAL";   messageColor = COLOR_CRITICAL;   break;
    case QtFatalMsg:    messageType = "FATAL   ";   messageColor = COLOR_FATAL;   break;
    default:
        messageType = "INFO    "; messageColor = COLOR_RESET;
    }

    QString timeStamp(QTime::currentTime().toString("hh:mm:ss:zzz"));

    stream << messageColor
           << QString(QString(context.category) + "." + messageType).leftJustified(17, ' ')
           << " [" << timeStamp << "] "
           << localMsg.constData()
           << COLOR_RESET << " [" << file << ", line " << context.line << "]" << endl;

    QTextStream(stdout) << stringMsg;

    Configurator *configurator = Configurator::getInstance();
    QDir logDir = configurator->getBaseDir();
    QString path = logDir.absoluteFilePath("log.txt");

    QFile outFile(path);
    QIODevice::OpenMode ioFlags = QIODevice::WriteOnly;
    if (configurator->logFileIsCreated()) {
        ioFlags |= QIODevice::Append;
    } else {
        ioFlags |= QIODevice::Truncate;
        configurator->setFileCreatedFlag();
    }
    if (outFile.open(ioFlags)) {
        QTextStream ts(&outFile);
        ts << stringMsg;
    }

    if (type == QtFatalMsg)
        abort();
}

Configurator::Configurator() :
    logConfigFileName(LOG_CONFIG_FILE_NAME),
    logFileCreated(false)
{

}

// -------------------------------------------------------------------------------
Configurator *Configurator::getInstance()
{
    if (Configurator::instance.isNull())
        instance.reset(new Configurator());
    return instance.data();
}

QStringList Configurator::getPresetFilesNames(bool fullpath)
{
    QDir::SortFlags sort = QDir::Name;
    QDir::Filters filters = QDir::Files | QDir::Readable;
    QStringList nameFilters("*.json");

    QFileInfoList fileInfos = presetsDir.entryInfoList(nameFilters, filters, sort);

    QStringList filesPaths;
    foreach (const QFileInfo &item, fileInfos) {
        if (item.isFile()) {
            if (fullpath)
                filesPaths.append(item.absoluteFilePath());
            else
                filesPaths.append(item.fileName());
        }
    }
    return filesPaths;
}

bool Configurator::needExportThemes() const
{
    return themesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot).isEmpty();
}

QDir Configurator::getApplicationDataDir()
{
    QDir dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!dir.exists())
        dir.mkpath(".");
    return dir;
}

bool Configurator::setUp()
{
    initializeDirs(); // directories initialization is different in Standalone and VstPlugin. Check the files ConfiguratorStandalone.cpp and VstPlugin.cpp

    // themes dir is the same for Standalone and Vst plugin
    themesDir = QDir(getApplicationDataDir().absoluteFilePath(THEMES_FOLDER_NAME));

    if (!folderTreeExists())
        createFoldersTree();

    exportLogIniFile(); //copy log config file from resources to user hard disk

    if (needExportThemes()) // copy default themes from resources to user hard disk
        exportThemes();

    setupLogConfigFile();

    qInfo() << "JamTaba Base dir:" << baseDir.absolutePath();

    return true;
}

QDir Configurator::createPresetsDir(const QDir &baseDir)
{
    QString presetsFolder(PRESETS_FOLDER_NAME);
    if (QSysInfo::buildCpuArchitecture().endsWith("64")) { /** 32 and 64 bits presets are not compatible - issue #587 https://github.com/elieserdejesus/JamTaba/issues/587 **/
        presetsFolder += "_x64"; /** using a different folder for 64 bits presets **/
    }
    return QDir(baseDir.absoluteFilePath(presetsFolder));
}

void Configurator::exportThemes() const
{
    // copy default themes from resources to user hard disk
    QDir resourceDir(THEMES_FOLDER_IN_RESOURCES);
    QDir themesDir = getThemesDir();
    qDebug() << "Exporting themes from " << resourceDir.absolutePath() << " to " << themesDir.absolutePath();
    QStringList themesInResources = resourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &themeDir : themesInResources) {
        QDir themeFolderInResources(resourceDir.absoluteFilePath(themeDir));
        themesDir.mkdir(themeDir);
        QDir destinationDir(themesDir.absoluteFilePath(themeDir));
        qDebug() << "Exporting theme " << themeDir << " from " << themeFolderInResources.absolutePath() << " to " << destinationDir.absolutePath();
        QString fullPath = themeFolderInResources.absolutePath() + "/css/themes/" + themeDir;
        QDir pathInResources(fullPath);
        QStringList themeFiles = pathInResources.entryList(QDir::Files); // css files
        for (const QString &themeCSSFile : themeFiles) {
            QString sourcePath = pathInResources.absoluteFilePath(themeCSSFile);
            QString destinationPath = destinationDir.absoluteFilePath(themeCSSFile);
            if (QFile::copy(sourcePath, destinationPath))
                QFile(destinationPath).setPermissions(QFile::ReadOwner | QFile::WriteOwner);
            else
                qDebug(jtConfigurator) << "Can't copy " << sourcePath << " to " << destinationPath;

        }
    }
}

void Configurator::setupLogConfigFile()
{
    QString logConfigFilePath = baseDir.absoluteFilePath(logConfigFileName);
    if (!logConfigFilePath.isEmpty()) {
        qputenv("QT_LOGGING_CONF", QByteArray(logConfigFilePath.toUtf8()));
        qInstallMessageHandler(&Configurator::LogHandler);
    }
    else {
        qWarning() << "Log file path is empty!";
    }
}

void Configurator::createFoldersTree()
{
    qWarning(jtConfigurator) << " Creating folders tree...";

    if (!baseDir.exists()) {
        if (baseDir.mkpath("."))
            qCDebug(jtConfigurator) << "Base dir created in " << baseDir.absolutePath();
        else
            qCDebug(jtConfigurator) << "Can´t create Base dir in " << baseDir.absolutePath();
    }

    if (!cacheDir.exists()) {
        if(cacheDir.mkpath("."))
            qCDebug(jtConfigurator) << "Cache dir created in " << cacheDir.absolutePath();
        else
            qCDebug(jtConfigurator) << "Can´t create Cache dir in " << cacheDir.absolutePath();
    }

    if (!presetsDir.exists()) {
        if(presetsDir.mkpath("."))
            qCDebug(jtConfigurator) << "Standalone presets dir created in " << presetsDir.absolutePath();
        else
            qCDebug(jtConfigurator) << "Can´t create PRESETS dir in " << presetsDir.absolutePath();
    }

    if (!themesDir.exists()) {
        if(themesDir.mkpath("."))
            qCDebug(jtConfigurator) << "Themes dir created in " << themesDir.absolutePath();
        else
            qCDebug(jtConfigurator) << "Can´t create Themes dir in " << themesDir.absolutePath();
    }
}

bool Configurator::folderTreeExists() const
{
    if (!presetsDir.exists() || !cacheDir.exists() || !themesDir.exists()) {

        if (!presetsDir.exists())
            qWarning(jtConfigurator) << "Presets dir don't exist !";

        if (!cacheDir.exists())
            qWarning(jtConfigurator) << "Cache dir don't exist !";

        if (!themesDir.exists())
            qWarning(jtConfigurator) << "Themes dir don't exist !";

        return false;
    }
    return true;
}

// copy the logging.ini from resources to application writable path, so user can tweak the Jamtaba log
void Configurator::exportLogIniFile()
{
    QString logConfigFilePath(baseDir.absoluteFilePath(logConfigFileName));

    QFile file(logConfigFilePath);
    if (!file.exists()) {
        qDebug(jtConfigurator) << "Log Ini file don't exist in' :" << logConfigFilePath;

        //copy the log config file from resources to 'filePath'
        QFile logFile(":/" + logConfigFileName);
        bool result = logFile.copy(logConfigFilePath);
        if (result)
            qDebug(jtConfigurator) << "Log Config file copied in :"<<logConfigFilePath;
        else
            qDebug(jtConfigurator) << "FAILED to copy the log config file in :" << logConfigFilePath << "Error:" << logFile.errorString();
    }
}

QString Configurator::getPresetPath(const QString &presetFileName)
{
    if (!presetsDir.exists()) {
        qDebug(jtConfigurator) << "Can't load preset file , preset dir inexistent !";
        return "";
    }

    return presetsDir.absoluteFilePath(presetFileName);
}

void Configurator::deletePreset(const QString &name)
{
    QString presetPath = getPresetPath(name);
    if (presetPath.isEmpty())
        return;

    qInfo(jtConfigurator) << "!!! Preset to delete Path is " << presetPath;
    if (presetsDir.remove(name))
        qDebug() << "!!! Preset deleted ! ";
    else
        qDebug() << "!!! Could not delete Preset " << name;
}

// -------------------------------------------------------------------------------

Configurator::~Configurator()
{
}

// -------------------------------------------------------------------------------
