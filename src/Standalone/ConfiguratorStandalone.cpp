#include "Configurator.h"
#include <QStandardPaths>

void Configurator::initializeDirs()
{
    baseDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    cacheDir = QDir(baseDir.absoluteFilePath(CACHE_FOLDER_NAME));
    presetsDir = QDir(baseDir.absoluteFilePath(PRESETS_FOLDER_NAME));
}
