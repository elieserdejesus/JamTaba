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
class JamInterval
{
public:
    JamInterval(const int intervalIndex, const int bpm, const int bpi, const QString &path, const QString &userName, const quint8 channelIndex);
    JamInterval();

    inline int getIntervalIndex() const
    {
        return intervalIndex;
    }

    inline int getBpm() const
    {
        return bpm;
    }

    inline int getBpi() const
    {
        return bpi;
    }

    inline QString getPath() const
    {
        return path;
    }

    inline QString getUserName() const
    {
        return userName;
    }

    inline quint8 getChannelIndex() const
    {
        return channelIndex;
    }

private:
    int intervalIndex;
    int bpm;
    int bpi;
    QString path;
    QString userName;
    quint8 channelIndex;
};
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Jam
{
public:
    Jam(int bpm, int bpi, int sampleRate, const QString &jamName, const QString &baseDir);

    inline QString getRPPAudioAbsolutePath() const
    {
        return rppAudioPath;
    }

    inline QString getClipSortAbsolutePath() const
    {
        return clipsortPath;
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

    inline double getIntervalsLenght() const
    {
        return 60.0/bpm * (double)bpi;
    }

    // called when a new file is writed in disk
    void addAudioFile(const QString &userName, const quint8 channelIndex, const QString &filePath, const int intervalIndex);

    QList<JamTrack> getJamTracks() const;

    QList<JamInterval> getJamIntervals() const;
private:
    int bpm;
    int bpi;
    int sampleRate;
    QString name;
    QString baseDir;
    QString rppAudioPath;
    QString clipsortPath;

    // the first map key is userName. The second map key is channelIndex
    QMap<QString, QMap<quint8, JamTrack> > jamTracks;

    // the map key is intervalIndex.
    QMap<int, QList<JamInterval> > jamIntervals;
};
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class JamMetadataWriter
{
public:
    virtual void write(const Jam &metadata) = 0;
    virtual ~JamMetadataWriter(){}
    virtual QString getWriterId() const = 0;
    virtual QString getWriterName() const = 0; // Localised
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
    ~JamRecorder();
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

    inline QString getWriterId() const { return jamMetadataWritter->getWriterId(); }
    inline QString getWriterName() const { return jamMetadataWritter->getWriterName(); }

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
