#include "ReaperProjectGenerator.h"
#include <QUuid>
#include "../log/Logging.h"

using namespace Recorder;


void ReaperProjectGenerator::write(const Jam &jam){
    QString stringBuffer("");
    stringBuffer.append("<REAPER_PROJECT 0.1 \"4.731\" 1416709867").append("\n");
    stringBuffer.append("  RECORD_PATH \"audio\" \"\"").append("\n");
    stringBuffer.append("  SAMPLERATE " + QString::number(jam.getSampleRate()) + "  0 0").append("\n");
    stringBuffer.append("  TEMPO " + QString::number(jam.getBpm()) + " 4 4").append("\n");

    //generate tracks

    QList<JamTrack> tracks = jam.getJamTracks();
    for (JamTrack track : tracks) {
        QString trackName = buildTrackName(track.getUserName(), track.getChannelIndex());
        QString trackGUID = QUuid::createUuid().toString();
        stringBuffer.append("  <TRACK "+ trackGUID).append("\n");
        stringBuffer.append("    NAME \"" + trackName + "\"").append("\n");
        stringBuffer.append("    TRACKID " + trackGUID).append("\n");
        QList<JamAudioFile> channelAudioFiles = track.getAudioFiles();
        int part = 1;
        for (JamAudioFile audioFile : channelAudioFiles) {
            double position = (audioFile.getIntervalIndex()-1) * jam.getIntervalsLenght();
            QString filePath = audioFile.getPath();
            stringBuffer.append("    <ITEM").append("\n");
            stringBuffer.append("      POSITION " + QString::number(position)).append("\n");
            stringBuffer.append("      LENGTH " + QString::number(jam.getIntervalsLenght())).append("\n");
            stringBuffer.append("      FADEIN 1 0.01 0 1 0 0").append("\n");
            stringBuffer.append("      FADEOUT 1 0.01 0 1 0 0").append("\n");
            stringBuffer.append("      IID " + QString::number(part)).append("\n");
            stringBuffer.append("      IGUID "+ QUuid::createUuid().toString()).append("\n");
            stringBuffer.append("      NAME \"" + QFileInfo(audioFile.getPath()).baseName() + "\"").append("\n");
            stringBuffer.append("      GUID "+ trackGUID).append("\n");
            stringBuffer.append("      <SOURCE VORBIS").append("\n");
            stringBuffer.append("        FILE \"" + filePath + "\"").append("\n");
            stringBuffer.append("      >").append("\n");//close SOURCE VORBIS
            stringBuffer.append("    >").append("\n");//close item
            part++;
        }
        stringBuffer.append("  >").append("\n");//close track
    }

    stringBuffer.append(">");//close the root tag
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //save
    QDir jamDir = QDir(this->rppPath);
    QFile projectFile(jamDir.absoluteFilePath("Reaper project.rpp"));
    if(!projectFile.open(QFile::WriteOnly)){
        qCritical() << "Can't write the reaper project file in " << jamDir;
    }
    QTextStream stream(&projectFile);
    QByteArray byteArray(stringBuffer.toUtf8());
    stream << byteArray;
    //projectFile.write(stringBuffer.toStdString().c_str(), stringBuffer.size());
}

void ReaperProjectGenerator::setJamDir(QString newJamName, QString recordBasePath)
{
    QDir parentDir(QDir(recordBasePath).absoluteFilePath(newJamName));
    parentDir.mkpath("Reaper");
    this->rppPath = parentDir.absoluteFilePath("Reaper");
}

QString ReaperProjectGenerator::getAudioAbsolutePath(QString audioFileName){
    QDir jamDir = QDir(this->rppPath);
    if (!jamDir.exists("audio") && !jamDir.mkdir("audio"))
    {
        qCritical() << "Could not create audio directory in " << this->rppPath;
        return QString::null;
    }
    return jamDir.absoluteFilePath("audio/" + audioFileName);
}


QString ReaperProjectGenerator::buildTrackName(const QString &userName, quint8 channelIndex) {
    return userName + " (Channel " + QString::number(channelIndex+1) + ")";
}
