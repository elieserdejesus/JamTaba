#include "Configurator.h"
#include <QFile>
#include <QDebug>
#include <QApplication>

#include "log/Logging.h"

QScopedPointer<Configurator> Configurator::instance(nullptr);

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

bool Configurator::setUp()
{
    initializeDirs(); // directories initialization is different in Standalone and VstPlugin. Check the files ConfiguratorStandalone.cpp and VstPlugin.cpp

    if (!folderTreeExists())
        createFoldersTree();

    exportLogIniFile(); //copy log config file from resources to user hard disk
    setupLogConfigFile();
    return true;
}

void Configurator::setupLogConfigFile()
{
    QString logConfigFilePath = baseDir.absoluteFilePath(logConfigFileName);
    if (!logConfigFilePath.isEmpty()) {
        qputenv("QT_LOGGING_CONF", QByteArray(logConfigFilePath.toUtf8()));
        qInstallMessageHandler(&Configurator::LogHandler);
    }
}

void Configurator::createFoldersTree()
{
    qWarning(jtConfigurator) << " Creating folders tree...";

    if (!baseDir.exists()) {
        baseDir.mkpath(".");
        qCDebug(jtConfigurator) << "Base dir created in " << baseDir.absolutePath();
    }

    if (!cacheDir.exists()) {
        cacheDir.mkpath(".");
        qCDebug(jtConfigurator) << "Cache dir created in " << cacheDir.absolutePath();
    }

    if (!presetsDir.exists()) {
        presetsDir.mkpath(".");
        qCDebug(jtConfigurator) << "Standalone presets dir created in " << presetsDir.absolutePath();
    }
}

bool Configurator::folderTreeExists() const
{
    if (!presetsDir.exists() || !cacheDir.exists()) {
        qWarning(jtConfigurator) << "FOLDER'S TREE don't exist !";
        return false;
    }
    return true;
}

// copy the logging.ini from resources to application writable path, so user can tweak the Jamtaba log
void Configurator::exportLogIniFile()
{
    QString logConfigFilePath = baseDir.absoluteFilePath(logConfigFileName);
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

QString Configurator::getPresetPath(const QString &jsonFile)
{
    if (!presetsDir.exists()) {
        qDebug(jtConfigurator) << "Can't load preset file , preset dir inexistent !";
        return "";
    }

    QString path = presetsDir.absoluteFilePath(jsonFile);
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
        if (presetsDir.remove(presetPath))
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
