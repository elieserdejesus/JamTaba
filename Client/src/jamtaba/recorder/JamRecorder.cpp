#include "JamRecorder.h"
#include <QDateTime>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

Q_LOGGING_CATEGORY(recorder, "recorder")

using namespace Recorder;

//++++++++++++++++++++++++++++++++++++++++++++++
JamAudioFile::JamAudioFile(QString path, uint intervalIndex)
    :path(path), intervalIndex(intervalIndex){

}
JamAudioFile::JamAudioFile()//default construtor to use this class in QMap and QList without pointers
    :path(""), intervalIndex(0){

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
JamTrack::JamTrack(QString userName, quint8 channelIndex)
    :userName(userName), channelIndex(channelIndex){

}

JamTrack::JamTrack()//default construtor to use this class in QMap and QList without pointers
    :userName(""), channelIndex(0){

}

void JamTrack::addAudioFile(QString path, int intervalIndex){
    audioFiles.append( JamAudioFile(path, intervalIndex));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Jam::Jam(int bpm, int bpi, int sampleRate, QString jamName, QString baseDir)
    : bpm(bpm), bpi(bpi), name(jamName), sampleRate(sampleRate), baseDir(baseDir) {

    QDir jamDir(QDir(baseDir).absoluteFilePath(jamName));
    jamDir.mkpath("audio");
    this->audioPath = jamDir.absoluteFilePath("audio");
}

//+++++++++++++++++++++++++++++++++++++++++++

QList<JamTrack> Jam::getJamTracks() const{
    QList<JamTrack> tracks;
    foreach (QString userName, jamTracks.keys()) {
        tracks.append(jamTracks[userName].values());
    }
    return tracks;
}

double Jam::getIntervalsLenght(){
    return 60.0/bpm * (double)bpi;
}

//called when a new file is writed in disk
void Jam::addAudioFile(QString userName, quint8 channelIndex, QString filePath, int intervalIndex){
    if(!jamTracks.contains(userName)){
        jamTracks.insert(userName, QMap<quint8, JamTrack>());
    }
    if(!jamTracks[userName].contains(channelIndex)){
        jamTracks[userName].insert(channelIndex, JamTrack(userName, channelIndex));
    }
    jamTracks[userName][channelIndex].addAudioFile(filePath, intervalIndex);
    qCDebug(recorder) << "adding a file in jam interval:" <<intervalIndex << " path:" << filePath;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString JamRecorder::getNewJamName() {
    QDateTime now = QDateTime::currentDateTime();
    QString dateString = now.toString();
    dateString = dateString.replace(QRegExp("[/: ]"), "-");
    return "Jam-" + dateString;
}

void JamRecorder::writeEncodedFile(const QByteArray& encodedData, QString path){

    QFile audioFile(path);
    if(!audioFile.open(QFile::WriteOnly)){
        qCritical() << "can't open file " << path;
        return;
    }
    audioFile.write(encodedData.data(), encodedData.size());
    qCDebug(recorder) << "file writed:" <<path;
}

QString JamRecorder::buildAudioFileName(QString userName, quint8 channelIndex, int currentInterval) {
    QString channelName = "Channel " + QString::number(channelIndex + 1);
    return userName + " (" + channelName + ") part " + QString::number(currentInterval) + ".ogg";
}

JamRecorder::JamRecorder(JamMetadataWriter* jamMetadataWritter)
    : jam(nullptr), jamMetadataWritter(jamMetadataWritter), globalIntervalIndex(0), running(false){

    //this->recordingActivated = true;//just to test
}


void JamRecorder::appendLocalUserAudio(QByteArray encodedaudio, quint8 channelIndex, bool isFirstPartOfInterval, bool isLastPastOfInterval){
    if(!running){
        qCCritical(recorder) << "Illegal state! Recorder is not running!";
        return;
    }
    //qCDebug(recorder) << "appending encoded audio to a local user channel index:" <<channelIndex;
    Q_UNUSED(isFirstPartOfInterval)
    if(!localUserIntervals.contains(channelIndex)){
        localUserIntervals.insert(channelIndex, LocalNinjamInterval(globalIntervalIndex));
    }
    localUserIntervals[channelIndex].appendEncodedAudio(encodedaudio);
    if(isLastPastOfInterval){
        QString audioFileName = buildAudioFileName(localUserName, channelIndex, localUserIntervals[channelIndex].getIntervalIndex());
        QString audioFilePath = QDir(jam->getAudioAbsolutePath()).absoluteFilePath(audioFileName);
        QByteArray encodedData(localUserIntervals[channelIndex].getEncodedData());
        QtConcurrent::run(this, &JamRecorder::writeEncodedFile, encodedData, audioFilePath);
        //writeEncodedFile(localUserIntervals[channelIndex].getEncodedData(), audioFilePath);
        jam->addAudioFile(localUserName, channelIndex, audioFilePath, localUserIntervals[channelIndex].getIntervalIndex());
        localUserIntervals[channelIndex].clear();
    }
}

void JamRecorder::addRemoteUserAudio(QString userName, QByteArray encodedAudio, quint8 channelIndex){
    if(!running){
        qCCritical(recorder) << "Illegal state! Recorder is not running!";
        return;
    }
    int intervalIndex = globalIntervalIndex;
    QString audioFileName = buildAudioFileName(userName, channelIndex, intervalIndex);
    QString audioFilePath = QDir(jam->getAudioAbsolutePath()).absoluteFilePath(audioFileName);
    QtConcurrent::run(this, &JamRecorder::writeEncodedFile, encodedAudio, audioFilePath);
    jam->addAudioFile(userName, channelIndex, audioFilePath, intervalIndex);
}

void JamRecorder::startRecording(QString localUser, QDir recordBasePath, int bpm, int bpi, int sampleRate){
    this->localUserName = localUser;

    this->currentJamName = getNewJamName();
    if(this->jam){
        delete this->jam;
    }
    this->jam = new Jam(bpm, bpi, sampleRate, currentJamName, recordBasePath.absolutePath());
    this->running = true;
}

//these methods are called when the user change the preferences. All these methods start a new recording
void JamRecorder::setBpi(int newBpi){
    if(running){
        stopRecording();
        startRecording(localUserName, QDir(jam->getBaseDir()), jam->getBpm(), newBpi, jam->getSampleRate() );
    }
}

void JamRecorder::setBpm(int newBpm){
    if(running){
        stopRecording();
        startRecording(localUserName, QDir(jam->getBaseDir()), newBpm, jam->getBpi(), jam->getSampleRate() );
    }
}

void JamRecorder::setRecordPath(QDir recordBasePath){
    if(running){
        stopRecording();
        startRecording(localUserName, recordBasePath, jam->getBpm(), jam->getBpi(), jam->getSampleRate() );
    }
}

void JamRecorder::setSampleRate(int newSampleRate){
    if(running){
        stopRecording();
        startRecording(localUserName, QDir(jam->getBaseDir()), jam->getBpm(), jam->getBpi(), newSampleRate );
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void JamRecorder::stopRecording() {
    if(running){
        writeProjectFile();
        this->running = false;
        this->globalIntervalIndex = 0;
        this->localUserIntervals.clear();
    }
}


void JamRecorder::writeProjectFile() {
    if (jamMetadataWritter && jam) {
        jamMetadataWritter->write(*jam);
    }
}


void JamRecorder::newInterval() {
    if (running) {
        globalIntervalIndex++;
        writeProjectFile();
    }
    //        if (newPath == null) {
    //            if (recording) {
    //                globalInterval++;
    //                writeProjectFile();
    //            }
    //        } else {
    //            end();
    //            globalInterval = 0;
    //            jam = jam.cloneToNewRecordPath(newPath);
    //            newPath = null;
    //        }
}

