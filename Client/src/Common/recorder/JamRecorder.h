#ifndef __JAM_RECORDER__
#define __JAM_RECORDER__

#include <QDir>
#include <QMap>

namespace Recorder {
// ++++++++++++++++++++++++++++++++++++++++++++++
class JamAudioFile
{
public:
    JamAudioFile(const QString &path, uint intervalIndex);
    JamAudioFile();// default construtor to use this class in QMap and QList without pointers
    inline uint getIntervalIndex() const
    {
        return intervalIndex;
    }

    inline QString getPath() const
    {
        return path;
    }

private:
    QString path;
    uint intervalIndex;
};
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class JamTrack
{
public:
    JamTrack(const QString &userName, quint8 channelIndex);
    JamTrack();// default construtor to use this class in QMap and QList without pointers
    void addAudioFile(const QString &path, int intervalIndex);
    inline QString getUserName() const
    {
        return userName;
    }

    inline quint8 getChannelIndex() const
    {
        return channelIndex;
    }

    inline QList<JamAudioFile> getAudioFiles() const
    {
        return audioFiles;
    }

private:
    QString userName;
    quint8 channelIndex;
    QList<JamAudioFile> audioFiles;
};
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Jam
{
public:
    Jam(int bpm, int bpi, int sampleRate, const QString &jamName, const QString &baseDir);

    double getIntervalsLenght() const;

    // called when a new file is writed in disk
    void addAudioFile(const QString &userName, quint8 channelIndex, const QString &filePath, int intervalIndex);
    QString getAudioAbsolutePath() const
    {
        return audioPath;
    }

    inline int getSampleRate() const
    {
        return sampleRate;
    }

    inline int getBpm() const
    {
        return bpm;
    }

    inline int getBpi() const
    {
        return bpi;
    }

    inline QString getBaseDir() const
    {
        return baseDir;
    }

    QList<JamTrack> getJamTracks() const;
private:
    int bpm;
    int bpi;
    int sampleRate;
    QString name;
    QString baseDir;
    QString audioPath;

    // the first map key is userName. The second map key is channelIndex
    QMap<QString, QMap<quint8, JamTrack> > jamTracks;
};
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class JamMetadataWriter
{
public:
    virtual void write(const Jam &metadata) = 0;
};
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class LocalNinjamInterval
{
public:
    LocalNinjamInterval(int intervalIndex) :
        intervalIndex(intervalIndex)
    {
    }

    LocalNinjamInterval()
    {
    }

    void appendEncodedAudio(const QByteArray &data)
    {
        encodedAudio.append(data);
    }

    inline QByteArray getEncodedData() const
    {
        return encodedAudio;
    }

    void clear()
    {
        encodedAudio.clear();
        this->intervalIndex++;
    }

    inline int getIntervalIndex() const
    {
        return intervalIndex;
    }

private:
    QByteArray encodedAudio;
    int intervalIndex;
};

class JamRecorder
{
public:
    JamRecorder(JamMetadataWriter *jamMetadataWritter);
    void appendLocalUserAudio(const QByteArray &encodedaudio, quint8 channelIndex,
                              bool isFirstPartOfInterval, bool isLastPastOfInterval);
    void addRemoteUserAudio(const QString &userName, const QByteArray &encodedAudio, quint8 channelIndex);
    void startRecording(const QString &localUser, const QDir &recordBasePath, int bpm, int bpi, int sampleRate);

    // these methods start a new recording
    void setRecordPath(const QDir &recordBasePath);
    void setBpm(int newBpm);
    void setBpi(int newBpi);
    void setSampleRate(int newSampleRate);

    void stopRecording();
    void newInterval();

private:
    QString currentJamName;
    Jam *jam;
    JamMetadataWriter *jamMetadataWritter;
    int globalIntervalIndex;
    QString localUserName;
    bool running;
    QDir newPath;// used to set the newPath in the next interval

    QMap<quint8, LocalNinjamInterval> localUserIntervals;// use channel index as key and store encoded bytes. When a full interval is stored the encoded bytes are store in a ogg file.

    QString getNewJamName();
    void writeEncodedFile(const QByteArray &encodedData, const QString &path);
    static QString buildAudioFileName(const QString &userName, quint8 channelIndex, int currentInterval);
    void writeProjectFile();

// ++++++++++++++++++++++++++++++++++++++++++++++++
};
}// namespace
#endif
