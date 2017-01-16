#include "Configurator.h"
#include <QtGlobal>

void Configurator::initializeDirs()
{
    QDir appLocalDataDir = getApplicationDataDir();
    
#ifdef Q_OS_WIN
    QString pluginDir(QStringLiteral("VstPlugin"));
#else
    QString pluginDir(QStringLiteral("AUPlugin"));
#endif
    baseDir = QDir(appLocalDataDir.absoluteFilePath(pluginDir));
    
    qDebug() << "baseDir: " << baseDir.absolutePath();

    //the cache folder is shared between Standalone and VstPlugin
    cacheDir = QDir(appLocalDataDir.absoluteFilePath(CACHE_FOLDER_NAME));
    qDebug() << "cacheDir: " << cacheDir.absolutePath();

    //the presets dir will be created inside VstPlugin dir. Standalone and VstPlugin are using different presets dirs.
    presetsDir = createPresetsDir(baseDir);
    qDebug() << "presetsDir: " << presetsDir.absolutePath();

}
