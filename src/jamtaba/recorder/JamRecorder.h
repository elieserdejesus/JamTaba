#ifndef __JAM_RECORDER__
#define __JAM_RECORDER__

#include <QDir>
#include <QMap>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(recorder)

namespace Recorder {

//++++++++++++++++++++++++++++++++++++++++++++++
//class JamPaths {
//public:
//    JamPaths(QDir baseDirectory, QDir jamdirectory, QDir audiodirectory);
//    JamPaths cloneUsingNewBaseDirectory(QDir newBaseDirectory);
//    inline QDir getBaseDir() const{return baseDirectory;}
//    inline QDir getJamDir() const {return jamdirectory;}
//    inline QDir getAudioDir() const{return audiodirectory;}
//private:
//    QDir baseDirectory;
//    QDir jamdirectory;
//    QDir audiodirectory;
//};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class JamAudioFile{
public:
    JamAudioFile(QString path, uint intervalIndex);
    JamAudioFile();//default construtor to use this class in QMap and QList without pointers
private:
    QString path;
    uint intervalIndex;
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class JamTrack{
public:
    JamTrack(QString userName, quint8 channelIndex);
    JamTrack();//default construtor to use this class in QMap and QList without pointers
    void addAudioFile(QString path, int intervalIndex);
private:
    QString userName;
    quint8 channelIndex;
    QList<JamAudioFile> audioFiles;
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Jam {
public:
    Jam(int bpm, int bpi, int sampleRate, QString jamName, QString baseDir);
    Jam cloneToNewRecordPath(QString newRecordBasePath);
    double getIntervalsLenght();

    //called when a new file is writed in disk
    void addAudioFile(QString userName, quint8 channelIndex, QString filePath, int intervalIndex);
    //QDir getResolvedJamAudioDirectory() const;
    //QString getJamAudioDirectoryName() const;
    //QDir getRecordingBaseDirectory() const;
    QString getAudioAbsolutePath() const{return audioPath;}
private:
    int bpm;
    int bpi;
    int sampleRate;
    QString name;
    QString baseDir;
    QString audioPath;

    //the first map key is userName. The second map key is channelIndex
    QMap<QString, QMap<quint8, JamTrack>> jamTracks;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class JamMetadataWriter {
public:
    virtual void write(Jam metadata) = 0;
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class NinjamInterval{
public:
    NinjamInterval(int intervalIndex)
        :intervalIndex(intervalIndex){

    }
    NinjamInterval(){}
    void appendEncodedAudio(QByteArray data){encodedAudio.append(data);}
    inline QByteArray getEncodedData() const{return encodedAudio;}
    void clear(){
        encodedAudio.clear();
        this->intervalIndex++;
    }

    inline int getIntervalIndex() const{return intervalIndex;}
private:
    QByteArray encodedAudio;
    int intervalIndex;
};

class JamRecorder {
public:
    JamRecorder(JamMetadataWriter* jamMetadataWritter);
    void appendLocalUserAudio(QByteArray encodedaudio, quint8 channelIndex, bool isFirstPartOfInterval, bool isLastPastOfInterval);
    void startRecording(QString localUser, QDir recordBasePath, int bpm, int bpi, int sampleRate);
    void stopRecording() ;
    void newInterval();
    void setRecordingPath(QDir newPath);
    //inline QDir getRecordingPath() const{return jam->getRecordingBaseDirectory();}
    void setRecordingStatus(bool status);
    inline bool isRecordingActivated() const{return recordingActivated;}
private:
    QString currentJamName;
    Jam* jam;
    JamMetadataWriter* jamMetadataWritter;
    int globalIntervalIndex;
    QString localUserName;
    bool running;
    bool recordingActivated;
    QDir newPath;//used to set the newPath in the next interval

    QMap<quint8, NinjamInterval> localUserIntervals;//use channel index as key and store encoded bytes. When a full interval is stored the encoded bytes are store in a ogg file.

    QString getNewJamName() ;
    void writeEncodedFile(const QByteArray& encodedData, QString path);
    static QString buildAudioFileName(QString userName, quint8 channelIndex, int currentInterval);
    void writeProjectFile() ;

//++++++++++++++++++++++++++++++++++++++++++++++++

};

}//namespace
#endif
