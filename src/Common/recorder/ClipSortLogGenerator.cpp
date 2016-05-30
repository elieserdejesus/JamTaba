#include "ClipSortLogGenerator.h"
#include <QUuid>
#include "../log/Logging.h"

using namespace Recorder;


void ClipSortLogGenerator::write(const Jam &jam){
    QDir jamDir = QDir(jam.getAudioAbsolutePath());

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
        QString replacementFilePath = QUuid::createUuid().toString().remove(QRegExp("[-{}]"));
        stringBuffer.append("user")
            .append(" " + replacementFilePath)
            .append(" \"" + interval.getUserName().replace("\"", "_") + "\"") // it'll work...
            .append(" " + interval.getChannelIndex())
            .append(" \"channel name\"")
            .append("\n");

        // Now some hackery is required to maybe create a dir with the first letter of the
        // replacement filename and then copy/link the existing file to that name
        if (!jamDir.exists(replacementFilePath.left(1)) && !jamDir.mkdir(replacementFilePath.left(1)))
        {
            qCCritical(jtJamRecorder) << "Could not create clip directory in " << jamDir;
            break; // to avoid flooding
        }
        else
        {
            // If this does not work, we'll have to copy and hope Windows at least creates a lazy copy
            QFile(interval.getPath()).link(jamDir.canonicalPath() + "/" + replacementFilePath.left(1) + "/" + replacementFilePath);
        }

    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //save
    jamDir.cdUp();
    QFile projectFile(jamDir.absoluteFilePath("clipsort.log"));
    if(!projectFile.open(QFile::WriteOnly)){
        qCCritical(jtJamRecorder) << "Can't write clipsort.log in " << jamDir;
    }
    QTextStream stream(&projectFile);
    QByteArray byteArray(stringBuffer.toUtf8());
    stream << byteArray;
}
