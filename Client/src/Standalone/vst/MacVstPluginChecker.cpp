#include "VstPluginChecker.h"
#include <QFileInfo>
#include <QFile>

bool Vst::PluginChecker::isValidPluginFile(const QString &path)
{
    QFileInfo file(path);
    return file.isBundle() && file.absoluteFilePath().endsWith(".vst");
}
