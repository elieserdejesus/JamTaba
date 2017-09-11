#include "FileUtils.h"

#include <QRegularExpression>

QString file::sanitizeFileName(QString &fileName)
{
    static const QRegularExpression regex("[\\\\/:*\?\"<>|]");
    return fileName.replace(regex, "_");
}
