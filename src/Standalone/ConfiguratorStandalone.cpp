#include "Configurator.h"
#include <QStandardPaths>

void Configurator::initializeDirs()
{
    baseDir = getApplicationDataDir();
    cacheDir = QDir(baseDir.absoluteFilePath(CACHE_FOLDER_NAME));
    presetsDir = createPresetsDir(baseDir);
}
