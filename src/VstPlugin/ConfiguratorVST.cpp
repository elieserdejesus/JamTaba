#include "Configurator.h"

void Configurator::initializeDirs()
{
    QDir appLocalDataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);

    baseDir = QDir(appLocalDataDir.absoluteFilePath("VstPlugin"));

    //the cache folder is shared between Standalone and VstPlugin
    cacheDir = QDir(appLocalDataDir.absoluteFilePath(CACHE_FOLDER_NAME));

    //the presets dir will be created inside VstPlugin dir. Standalone and VstPlugin are using different presets dirs.
    presetsDir = QDir(baseDir.absoluteFilePath(PRESETS_FOLDER_NAME));
}
