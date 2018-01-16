#include "ClipSortLogGenerator.h"
#include <QUuid>
#include "../log/Logging.h"

using namespace recorder;

void ClipSortLogGenerator::write(const Jam &jam)
{
    QString stringBuffer("");

    int intervalIndex = -1;
    double bpm = 0.0;
    int bpi = 0;
    QList<JamInterval> intervals = jam.getJamIntervals();

    for (JamInterval interval : intervals) {
        if (interval.getIntervalIndex() != intervalIndex) {
            intervalIndex = interval.getIntervalIndex();
            bpm = interval.getBpm();
            bpi = interval.getBpi();
            stringBuffer.append("interval " + QString::number(intervalIndex))
                .append(" " + QString::number(bpm))
                .append(" " + QString::number(bpi))
                .append("\n");
        }
        //user 451065aed5824d1c51254b7cdf417598 "Alfred@62.163.190.x" 0 "Abnormal NINJAM"
        QString intervalName = QFileInfo(interval.getPath()).baseName();
        stringBuffer.append("user")
            .append(" " + intervalName)
            .append(" \"" + interval.getUserName().replace("\"", "_") + "\"") // it'll work...
            .append(" " + QString::number(interval.getChannelIndex()))
            .append(" \"channel name\"")
            .append("\n");
    }

    // save
    QDir jamDir = QDir(this->clipsortPath);
    QFile projectFile(jamDir.absoluteFilePath("clipsort.log"));
    if (!projectFile.open(QFile::WriteOnly)) {
        qCritical() << "Can't write clipsort.log in " << jamDir;
    }
    QTextStream stream(&projectFile);
    QByteArray byteArray(stringBuffer.toUtf8());
    stream << byteArray;
}

void ClipSortLogGenerator::setJamDir(const QString &newJamName, const QString &recordBasePath)
{
    QDir parentDir(QDir(recordBasePath).absoluteFilePath(newJamName));
    parentDir.mkpath("Reaper/clipsort");
    this->clipsortPath = parentDir.absoluteFilePath("Reaper/clipsort");
}

QString ClipSortLogGenerator::getAudioAbsolutePath(const QString &audioFileName)
{
    QDir jamDir = QDir(this->clipsortPath);
    QString replacementFilePath = QUuid::createUuid().toString().remove(QRegExp("[-{}]"));
    if (!jamDir.exists(replacementFilePath.left(1)) && !jamDir.mkdir(replacementFilePath.left(1))) {
        qCritical() << "Could not create clip directory in " << this->clipsortPath;
        return QString::null;
    }
    return jamDir.absoluteFilePath(replacementFilePath.left(1) + "/" +
            replacementFilePath + "." +
            QFileInfo(audioFileName).suffix());
}

QString ClipSortLogGenerator::getVideoAbsolutePath(const QString &videoFileName)
{
    Q_UNUSED(videoFileName);

    return QString();
}
