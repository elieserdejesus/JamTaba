#include "JamRecorder.h"
#include <QDateTime>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "../log/Logging.h"

using namespace Recorder;

//++++++++++++++++++++++++++++++++++++++++++++++
JamAudioFile::JamAudioFile(const QString &path, uint intervalIndex)
    :path(path), intervalIndex(intervalIndex){

}
JamAudioFile::JamAudioFile()//default construtor to use this class in QMap and QList without pointers
    :path(""), intervalIndex(0){

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
JamTrack::JamTrack(const QString &userName, quint8 channelIndex)
    :userName(userName), channelIndex(channelIndex){

}

JamTrack::JamTrack()//default construtor to use this class in QMap and QList without pointers
    :userName(""), channelIndex(0){

}

void JamTrack::addAudioFile(const QString &path, int intervalIndex){
    audioFiles.append( JamAudioFile(path, intervalIndex));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
JamInterval::JamInterval(const int intervalIndex, const int bpm, const int bpi, const QString &path, const QString &userName, const quint8 channelIndex)
    :intervalIndex(intervalIndex), bpm(bpm), bpi(bpi), path(path), userName(userName), channelIndex(channelIndex){
}

JamInterval::JamInterval()
    :intervalIndex(0), bpm(-1), bpi(-1), path(""), userName(""), channelIndex(0){
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Jam::Jam(int bpm, int bpi, int sampleRate)
    : bpm(bpm), bpi(bpi), sampleRate(sampleRate) {
}

//+++++++++++++++++++++++++++++++++++++++++++

QList<JamTrack> Jam::getJamTracks() const{
    QList<JamTrack> tracks;
    foreach (const QString &userName, jamTracks.keys()) {
        tracks.append(jamTracks[userName].values());
    }
    return tracks;
}

QList<JamInterval> Jam::getJamIntervals() const{
    QList<JamInterval> intervals;
    foreach (const int intervalIndex, jamIntervals.keys()) {
        intervals.append(jamIntervals[intervalIndex]);
    }
    return intervals;
}

//called when a new file is writed in disk
void Jam::addAudioFile(const QString &userName, quint8 channelIndex, const QString &filePath, int intervalIndex){

    if(!jamTracks.contains(userName)){
        jamTracks.insert(userName, QMap<quint8, JamTrack>());
    }
    if(!jamTracks[userName].contains(channelIndex)){
        jamTracks[userName].insert(channelIndex, JamTrack(userName, channelIndex));
    }
    jamTracks[userName][channelIndex].addAudioFile(filePath, intervalIndex);

    if(!jamIntervals.contains(intervalIndex)){
        jamIntervals.insert(intervalIndex, QList<JamInterval>());
    }
    jamIntervals[intervalIndex].insert(intervalIndex, JamInterval(intervalIndex, getBpm(), getBpi(), filePath, userName, channelIndex));

    qCDebug(jtJamRecorder) << "adding a file in jam interval:" <<intervalIndex << " path:" << filePath;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString JamRecorder::getNewJamName() {
    QDateTime now = QDateTime::currentDateTime();
    QString nowString = now.toString(Qt::TextDate);
    nowString = nowString.replace(QRegExp("[/:]"), "-").replace(QRegExp("[ ]"), "_");
    return "Jam-" + nowString;
}

void JamRecorder::writeEncodedFile(const QByteArray& encodedData, const QString &path){

    QFile audioFile(path);
    if(!audioFile.open(QFile::WriteOnly)){
        qCritical() << "can't open file " << path;
        return;
    }
    audioFile.write(encodedData.data(), encodedData.size());
    qCDebug(jtJamRecorder) << "file writed:" <<path;
}

QString JamRecorder::buildAudioFileName(const QString &userName, quint8 channelIndex, int currentInterval) {
    QString channelName = "Channel " + QString::number(channelIndex + 1);
    return userName + " (" + channelName + ") part " + QString::number(currentInterval) + ".ogg";
}

JamRecorder::JamRecorder(JamMetadataWriter* jamMetadataWritter)
    : jam(nullptr), jamMetadataWritter(jamMetadataWritter), globalIntervalIndex(0), running(false){
    //this->recordingActivated = true;//just to test
    qCDebug(jtJamRecorder) << "Creating JamRecorder!";
}

JamRecorder::~JamRecorder()
{
    delete jamMetadataWritter;
    qCDebug(jtJamRecorder) << "Deleting JamRecorder!";
}

void JamRecorder::appendLocalUserAudio(const QByteArray &encodedaudio, quint8 channelIndex, bool isFirstPartOfInterval, bool isLastPastOfInterval){
    if(!running){
        qCCritical(jtJamRecorder) << "Illegal state! Recorder is not running!";
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
        QString audioFilePath = jamMetadataWritter->getAudioAbsolutePath(audioFileName);
        QByteArray encodedData(localUserIntervals[channelIndex].getEncodedData());
        QtConcurrent::run(this, &JamRecorder::writeEncodedFile, encodedData, audioFilePath);
        //writeEncodedFile(localUserIntervals[channelIndex].getEncodedData(), audioFilePath);
        jam->addAudioFile(localUserName, channelIndex, audioFilePath, localUserIntervals[channelIndex].getIntervalIndex());
        localUserIntervals[channelIndex].clear();
    }
}

void JamRecorder::addRemoteUserAudio(const QString &userName, const QByteArray &encodedAudio, quint8 channelIndex){
    if(!running){
        qCCritical(jtJamRecorder) << "Illegal state! Recorder is not running!";
        return;
    }
    int intervalIndex = globalIntervalIndex;
    QString audioFileName = buildAudioFileName(userName, channelIndex, intervalIndex);
    QString audioFilePath = jamMetadataWritter->getAudioAbsolutePath(audioFileName);
    QtConcurrent::run(this, &JamRecorder::writeEncodedFile, encodedAudio, audioFilePath);
    jam->addAudioFile(userName, channelIndex, audioFilePath, intervalIndex);
}

void JamRecorder::startRecording(const QString &localUser, const QDir &recordBaseDir, int bpm, int bpi, int sampleRate){
    this->localUserName = localUser;
    this->recordBaseDir = recordBaseDir;
    this->jamMetadataWritter->setJamDir(getNewJamName(), recordBaseDir.absolutePath());

    if(this->jam){
        delete this->jam;
    }
    this->jam = new Jam(bpm, bpi, sampleRate);

    this->running = true;
    qDebug(jtJamRecorder) << this->jamMetadataWritter->getWriterId() << "startRecording!";
}

//these methods are called when the user change the preferences. All these methods start a new recording
void JamRecorder::setBpi(int newBpi){
    if(running){
        stopRecording();
        startRecording(localUserName, recordBaseDir, jam->getBpm(), newBpi, jam->getSampleRate() );
    }
}

void JamRecorder::setBpm(int newBpm){
    if(running){
        stopRecording();
        startRecording(localUserName, recordBaseDir, newBpm, jam->getBpi(), jam->getSampleRate() );
    }
}

void JamRecorder::setRecordPath(const QDir &newDir){
    recordBaseDir = newDir;
    if(running){
        stopRecording();
        startRecording(localUserName, recordBaseDir, jam->getBpm(), jam->getBpi(), jam->getSampleRate() );
    }
}

void JamRecorder::setSampleRate(int newSampleRate){
    if(running){
        stopRecording();
        startRecording(localUserName, recordBaseDir, jam->getBpm(), jam->getBpi(), newSampleRate );
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

