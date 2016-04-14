#include "Configurator.h"
#include <QFile>
#include <QDebug>
#include <QApplication>

#include "log/Logging.h"

QScopedPointer<Configurator> Configurator::instance(nullptr);

const QString Configurator::VST_PLUGIN_FOLDER_NAME = "PluginVst";
const QString Configurator::PRESETS_FOLDER_NAME = "Presets";
const QString Configurator::CACHE_FOLDER_NAME = "Cache";
const QString Configurator::LOG_CONFIG_FILE_NAME = "logging.ini";

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

    QTextStream stream(&stringMsg);
    switch (type) {
    case QtDebugMsg:
        stream << context.category << ".DEBUG:  " << localMsg.constData() << " "  << " in "
               << file << " " << context.line << endl;
        break;
    case QtWarningMsg:
        stream << context.category << ".WARNING:  " << localMsg.constData() <<  context.function
               <<  " " << file << context.line << endl << endl;
        break;
    case QtCriticalMsg:
        stream << context.category << ".CRITICAL:  " << localMsg.constData() <<  context.function
               << " " << file << context.line << endl << endl;
        break;
    case QtFatalMsg:
        stream << context.category  << ".FATAL:  " << localMsg.constData() << context.function
               << file << context.line << endl << endl;
        break;
    default:
        stream << context.category << ".INFO:  " << localMsg.constData() <<endl;
    }

    QTextStream(stdout) << stringMsg;

    Configurator *configurator = Configurator::getInstance();
    QDir logDir = configurator->getLogDir();
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

QDir Configurator::getLogDir() const
{
    if (appType == AppType::STANDALONE)
        return baseDir;

    return pluginDir;
}

Configurator::Configurator() :
    logConfigFileName(LOG_CONFIG_FILE_NAME),
    logFileCreated(false)
{
    baseDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    cacheDir = QDir(baseDir.absoluteFilePath(CACHE_FOLDER_NAME));
    pluginDir = QDir(baseDir.absoluteFilePath(VST_PLUGIN_FOLDER_NAME));
    pluginPresetsDir = QDir(pluginDir.absoluteFilePath(PRESETS_FOLDER_NAME));
    presetsDir = QDir(baseDir.absoluteFilePath(PRESETS_FOLDER_NAME));
}

// -------------------------------------------------------------------------------
Configurator *Configurator::getInstance()
{
    if (Configurator::instance.isNull())
        instance.reset(new Configurator());
    return instance.data();
}

QDir Configurator::getPresetsDir() const
{
    if (appType == AppType::PLUGIN)
        return pluginPresetsDir;

    return presetsDir;
}

QStringList Configurator::getPresetFilesNames(bool fullpath)
{
    QStringList filesPaths;
    QDir dir = getPresetsDir();
    qInfo(jtConfigurator) << "Looking in Presets dir : "<<dir.absolutePath();
    int count = 0;// how many preset files
    foreach (const QFileInfo &item, dir.entryInfoList()) {
        if (item.isFile()) {
            if (fullpath)
                filesPaths.append(item.absoluteFilePath());
            else
                filesPaths.append(item.fileName());
            qDebug(jtConfigurator) << "File Found: " << item.absoluteFilePath();
        }
    }
    count = filesPaths.size();
    qInfo(jtConfigurator) << "Presets found : "<<count;
    return filesPaths;
}

// --------------------------------------------------------------------------------------------
bool Configurator::setUp(AppType appType)
{
    this->appType = appType;// plugin or standalone

    if (!folderTreeExists())
        createFoldersTree();

    exportLogIniFile(); //copy log config file from resources to user hard disk
    setupLogConfigFile();
    return true;
}

void Configurator::setupLogConfigFile()
{
    QString logConfigFilePath = getLogConfigFileDir().absoluteFilePath(logConfigFileName);
    if (!logConfigFilePath.isEmpty()) {
        qputenv("QT_LOGGING_CONF", QByteArray(logConfigFilePath.toUtf8()));
        qInstallMessageHandler(&Configurator::LogHandler);
    }
}

void Configurator::createFoldersTree()
{
    qWarning(jtConfigurator) << " Creating folders tree...";

    if (!pluginDir.exists()) {
        pluginDir.mkpath(".");
        qCDebug(jtConfigurator) << "Plugin dir created in " << pluginDir;
    }

    if (!cacheDir.exists()) {
        cacheDir.mkpath(".");
        qCDebug(jtConfigurator) << "Cache dir created in " << cacheDir;
    }

    if (!presetsDir.exists()) {
        presetsDir.mkpath(".");
        qCDebug(jtConfigurator) << "Standalone presets dir created in " << presetsDir;
    }

    if (!pluginPresetsDir.exists()) {
        pluginPresetsDir.mkpath(".");
        qCDebug(jtConfigurator) << "Plugin presets dir created in " << pluginPresetsDir;
    }
}

bool Configurator::presetsDirExists() const
{
    if (appType == STANDALONE)
        return presetsDir.exists();

    return pluginPresetsDir.exists();
}

bool Configurator::folderTreeExists() const
{
    if (!pluginDir.exists() || !presetsDirExists() || !cacheDir.exists()) {
        qWarning(jtConfigurator) << "FOLDER'S TREE don't exist !";
        return false;
    }
    return true;
}

// copy the logging.ini from resources to application writable path, so user can tweak the Jamtaba log
void Configurator::exportLogIniFile()
{
    QString logConfigFilePath = getLogConfigFileDir().absoluteFilePath(logConfigFileName);
    if (!QFile(logConfigFilePath).exists()) {
        qDebug(jtConfigurator) << "Log Ini file don't exist in' :"<<logConfigFilePath;

        //copy the log config file from resources to 'filePath'
        bool result = QFile::copy(":/" + logConfigFileName, logConfigFilePath);
        if (result)
            qDebug(jtConfigurator) << "Log Config file copied in :"<<logConfigFilePath;
        else
            qDebug(jtConfigurator) << "FAILED to copy the log config file in :"<<logConfigFilePath;
    }
}

QDir Configurator::getLogConfigFileDir() const
{
    if (appType == AppType::PLUGIN)
        return pluginDir;
    else
        return baseDir;
}

// Retrieve the path for Settings class
QString Configurator::getPresetPath(const QString &JsonFile)
{
    QString path("");
    // FIRST CHECK THE DIR
    if (!presetsDirExists()) {
        qDebug(jtConfigurator) << "Can't load preset file , preset dir inexistent !";
        return path; // Settings MUST CHECK THAT
    }

    QDir presetDir = getPresetsDir();
    // SEARCH IN THE DIR
    // USE CONFIGURATOR FOR FILES STUFF
    path = presetDir.absoluteFilePath(JsonFile);
    QStringList list = getPresetFilesNames(true);
    foreach (const QString &item, list) {
        if (item.contains(path)) {
            qDebug(jtConfigurator) << "Path to required preset is :" << path;
            return path;
        }
    }

    qDebug(jtConfigurator) << "NO Path found !" << path;
    return path;
}

void Configurator::deletePreset(const QString &name)
{
    QString presetPath = getPresetPath(name);
    if (presetPath.isEmpty())
        return;
    {
        qInfo(jtConfigurator) << "!!! Preset to delete Path is " << presetPath;
        if (getPresetsDir().remove(presetPath))
            qInfo(jtConfigurator) << "!!! Preset deleted ! ";
        else
            qInfo(jtConfigurator) << "!!! Could not delete Preset ! ";
    }
}

// -------------------------------------------------------------------------------

Configurator::~Configurator()
{
}

// -------------------------------------------------------------------------------
