#include "ReaperProjectGenerator.h"
#include <QUuid>

using namespace Recorder;

void ReaperProjectGenerator::write(Jam jam){
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
        for (JamAudioFile audioChunk : channelAudioFiles) {
            double position = audioChunk.getIntervalIndex() * jam.getIntervalsLenght();
            QString filePath = audioChunk.getPath();
            stringBuffer.append("    <ITEM").append("\n");
            stringBuffer.append("      POSITION " + QString::number(position)).append("\n");
            stringBuffer.append("      LENGTH " + QString::number(jam.getIntervalsLenght())).append("\n");
            stringBuffer.append("      FADEIN 1 0.01 0 1 0 0").append("\n");
            stringBuffer.append("      FADEOUT 1 0.01 0 1 0 0").append("\n");
            stringBuffer.append("      IID " + QString::number(part)).append("\n");
            stringBuffer.append("      IGUID "+ QUuid::createUuid().toString()).append("\n");
            stringBuffer.append("      NAME \"" + QFileInfo(audioChunk.getPath()).baseName() + "\"").append("\n");
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
    QDir jamDir = QDir(jam.getAudioAbsolutePath());
    jamDir.cdUp();
    QFile projectFile(jamDir.absoluteFilePath("Reaper project.rpp"));
    if(!projectFile.open(QFile::WriteOnly)){
        qCCritical(recorder) << "Can't write the reaper project file in " << jamDir;
    }
    QTextStream stream(&projectFile);
    QByteArray byteArray(stringBuffer.toUtf8());
    stream << byteArray;
    //projectFile.write(stringBuffer.toStdString().c_str(), stringBuffer.size());
}


QString ReaperProjectGenerator::buildTrackName(QString userName, quint8 channelIndex) {
    return userName + " (Channel " + QString::number(channelIndex+1) + ")";
}
