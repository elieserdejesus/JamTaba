#include "JamRecorder.h"
#include <QDateTime>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "../log/Logging.h"

using namespace recorder;

const quint8 JamRecorder::VIDEO_CHANNEL_KEY = 255;

JamAudioFile::JamAudioFile(const QString &path, uint intervalIndex) :
    path(path),
    intervalIndex(intervalIndex)
{
    //
}

JamAudioFile::JamAudioFile() : // default construtor to use this class in QMap and QList without pointers
    path(""),
    intervalIndex(0)
{
    //
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

JamTrack::JamTrack(const QString &userName, quint8 channelIndex) :
    userName(userName),
    channelIndex(channelIndex)
{
    //
}

JamTrack::JamTrack() : // default construtor to use this class in QMap and QList without pointers
    userName(""),
    channelIndex(0)
{

}

void JamTrack::addAudioFile(const QString &path, int intervalIndex)
{
    audioFiles.append( JamAudioFile(path, intervalIndex));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

JamInterval::JamInterval(const int intervalIndex, const int bpm, const int bpi, const QString &path, const QString &userName, const quint8 channelIndex) :
    intervalIndex(intervalIndex),
    bpm(bpm),
    bpi(bpi),
    path(path),
    userName(userName),
    channelIndex(channelIndex)
{
    //
}

JamInterval::JamInterval() :
    intervalIndex(0),
    bpm(-1),
    bpi(-1),
    path(""),
    userName(""),
    channelIndex(0)
{
    //
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Jam::Jam(int bpm, int bpi, int sampleRate) :
    bpm(bpm),
    bpi(bpi),
    sampleRate(sampleRate)
{
    //
}

//+++++++++++++++++++++++++++++++++++++++++++

QList<JamTrack> Jam::getJamTracks() const
{
    QList<JamTrack> tracks;
    for (const QString &userName : jamTracks.keys()) {
        tracks.append(jamTracks[userName].values());
    }
    return tracks;
}

QList<JamInterval> Jam::getJamIntervals() const
{
    QList<JamInterval> intervals;
    for (const int intervalIndex : jamIntervals.keys()) {
        intervals.append(jamIntervals[intervalIndex]);
    }
    return intervals;
}

// called when a new file is writed in disk
void Jam::addAudioFile(const QString &userName, quint8 channelIndex, const QString &filePath, int intervalIndex)
{

    if (!jamTracks.contains(userName)) {
        jamTracks.insert(userName, QMap<quint8, JamTrack>());
    }

    if (!jamTracks[userName].contains(channelIndex)) {
        jamTracks[userName].insert(channelIndex, JamTrack(userName, channelIndex));
    }

    jamTracks[userName][channelIndex].addAudioFile(filePath, intervalIndex);

    if (!jamIntervals.contains(intervalIndex)) {
        jamIntervals.insert(intervalIndex, QList<JamInterval>());
    }

    jamIntervals[intervalIndex].insert(intervalIndex, JamInterval(intervalIndex, getBpm(), getBpi(), filePath, userName, channelIndex));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QString JamRecorder::getNewJamName()
{
    QDateTime now = QDateTime::currentDateTime();
    QString nowString = now.toString(dirNameDateFormat);
    nowString = nowString.replace(QRegExp("[/:]"), "-").replace(QRegExp("[ ]"), "_");
    return "Jam-" + nowString;
}

void JamRecorder::writeEncodedFile(const QByteArray& encodedData, const QString &path)
{
    QFile audioFile(path);
    if (!audioFile.open(QFile::WriteOnly)) {
        qCritical() << "can't open file " << path;
        return;
    }
    audioFile.write(encodedData.data(), encodedData.size());
}

QString JamRecorder::buildVideoFileName(const QString &userName, int currentInterval, const QString &fileExtension)
{
    return userName + "_video_" + buildPaddedFileNumber(currentInterval) + "." + fileExtension;
}

QString JamRecorder::buildAudioFileName(const QString &userName, quint8 channelIndex, int currentInterval)
{
    QString channelName = "Channel " + QString::number(channelIndex + 1);
    return userName + " (" + channelName + ") part " + buildPaddedFileNumber(currentInterval) + ".ogg";
}

QString JamRecorder::buildPaddedFileNumber(int fileNumber)
{
    const int padDigits = 3;
    const QChar padChar('0');

    return QString("%1").arg(fileNumber, padDigits, 10, padChar);
}

JamRecorder::JamRecorder(JamMetadataWriter* jamMetadataWritter) :
    jam(nullptr),
    jamMetadataWritter(jamMetadataWritter),
    globalIntervalIndex(0),
    running(false)
{
    //this->recordingActivated = true;//just to test
    qCDebug(jtJamRecorder) << "Creating JamRecorder!";
}

JamRecorder::~JamRecorder()
{
    qCDebug(jtJamRecorder) << "Deleting JamRecorder!";
}

void JamRecorder::appendLocalUserAudio(const QByteArray &encodedAudio, quint8 channelIndex, bool isFirstPartOfInterval)
{
    if (!running) {
        qCritical() << "Illegal state! Recorder is not running!";
        return;
    }

    if (!localUserIntervals.contains(channelIndex)) {
        localUserIntervals.insert(channelIndex, LocalNinjamInterval(globalIntervalIndex));
    }

    auto &interval = localUserIntervals[channelIndex];

    bool needSave = isFirstPartOfInterval && !interval.isEmpty();
    if (needSave) {
        QString audioFileName = buildAudioFileName(localUserName, channelIndex, interval.getIntervalIndex());
        QString audioFilePath = jamMetadataWritter->getAudioAbsolutePath(audioFileName);
        QByteArray encodedData(interval.getEncodedData());
        QtConcurrent::run(this, &JamRecorder::writeEncodedFile, encodedData, audioFilePath);
        jam->addAudioFile(localUserName, channelIndex, audioFilePath, interval.getIntervalIndex());
        interval.clear();
    }

    interval.appendEncodedData(encodedAudio);
}

void JamRecorder::appendLocalUserVideo(const QByteArray &encodedVideo, bool isFirstPartOfInterval)
{
    if (!running) {
        qCritical() << "Illegal state! Recorder is not running!";
        return;
    }

    if (!localUserIntervals.contains(VIDEO_CHANNEL_KEY)) {
        localUserIntervals.insert(VIDEO_CHANNEL_KEY, LocalNinjamInterval(globalIntervalIndex));
    }

    auto &videoInterval = localUserIntervals[VIDEO_CHANNEL_KEY];

    bool needSave = isFirstPartOfInterval && !videoInterval.isEmpty();
    if (needSave) {

        QByteArray encodedData(videoInterval.getEncodedData());

        QString videoFileName = buildVideoFileName(localUserName, videoInterval.getIntervalIndex(), "mp4");
        QString videoFilePath = jamMetadataWritter->getVideoAbsolutePath(videoFileName);

        if (!videoFilePath.isEmpty()) // some recorders (like ClipSort) can't save videos
            QtConcurrent::run(this, &JamRecorder::writeEncodedFile, encodedData, videoFilePath);

        videoInterval.clear();
    }

    videoInterval.appendEncodedData(encodedVideo);
}

void JamRecorder::addRemoteUserAudio(const QString &userName, const QByteArray &encodedAudio, quint8 channelIndex)
{
    if (!running) {
        qCritical() << "Illegal state! Recorder is not running!";
        return;
    }

    int intervalIndex = globalIntervalIndex;
    QString audioFileName = buildAudioFileName(userName, channelIndex, intervalIndex);
    QString audioFilePath = jamMetadataWritter->getAudioAbsolutePath(audioFileName);
    QtConcurrent::run(this, &JamRecorder::writeEncodedFile, encodedAudio, audioFilePath);
    jam->addAudioFile(userName, channelIndex, audioFilePath, intervalIndex);
}

void JamRecorder::startRecording(const QString &localUser, const QDir &recordBaseDir, int bpm, int bpi, int sampleRate)
{
    if (running)
        stopRecording();

    this->localUserName = localUser;
    this->recordBaseDir = recordBaseDir;
    this->jamMetadataWritter->setJamDir(getNewJamName(), recordBaseDir.absolutePath());

    jam.reset(new Jam(bpm, bpi, sampleRate));

    running = true;
    qDebug(jtJamRecorder) << jamMetadataWritter->getWriterId() << "startRecording!";
}

// these methods are called when the user change the preferences. All these methods start a new recording
void JamRecorder::setBpi(int newBpi)
{
    if (running) {
        stopRecording();
        startRecording(localUserName, recordBaseDir, jam->getBpm(), newBpi, jam->getSampleRate() );
    }
}

void JamRecorder::setBpm(int newBpm)
{
    if (running) {
        stopRecording();
        startRecording(localUserName, recordBaseDir, newBpm, jam->getBpi(), jam->getSampleRate() );
    }
}

void JamRecorder::setRecordPath(const QDir &newDir)
{
    recordBaseDir = newDir;
    if (running) {
        stopRecording();
        startRecording(localUserName, recordBaseDir, jam->getBpm(), jam->getBpi(), jam->getSampleRate() );
    }
}

void JamRecorder::setDirNameDateFormat(Qt::DateFormat newDateFormat)
{
    dirNameDateFormat = newDateFormat;
    if (running) {
        stopRecording();
        startRecording(localUserName, recordBaseDir, jam->getBpm(), jam->getBpi(), jam->getSampleRate() );
    }
}

void JamRecorder::setSampleRate(int newSampleRate)
{
    if (running) {
        stopRecording();
        startRecording(localUserName, recordBaseDir, jam->getBpm(), jam->getBpi(), newSampleRate );
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void JamRecorder::stopRecording()
{
    if (running) {
        writeProjectFile();
        this->running = false;
        this->globalIntervalIndex = 0;
        this->localUserIntervals.clear();
    }
}


void JamRecorder::writeProjectFile()
{
    if (jamMetadataWritter && jam) {
        jamMetadataWritter->write(*jam);
    }
}


void JamRecorder::newInterval()
{
    if (running) {
        globalIntervalIndex++;
        writeProjectFile();
    }

}

