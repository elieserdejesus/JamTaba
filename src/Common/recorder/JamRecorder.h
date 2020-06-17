#ifndef __JAM_RECORDER__
#define __JAM_RECORDER__

#include <QDir>
#include <QMap>

#include <memory>

namespace recorder {


class JamAudioFile
{

public:
    JamAudioFile(const QString &path, uint intervalIndex);
    JamAudioFile(); // default construtor to use this class in QMap and QList without pointers

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
    JamTrack(); // default construtor to use this class in QMap and QList without pointers

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
    Jam(int bpm, int bpi, int sampleRate);

    inline int getBpm() const
    {
        return bpm;
    }

    inline int getBpi() const
    {
        return bpi;
    }

    inline int getSampleRate() const
    {
        return sampleRate;
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

    virtual void setJamDir(const QString &newJamName, const QString &recordBasePath) = 0;

    virtual QString getAudioAbsolutePath(const QString &audioFileName) = 0;

    virtual QString getVideoAbsolutePath(const QString &videoFileName) = 0;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class LocalNinjamInterval
{
public:
    explicit LocalNinjamInterval(int intervalIndex) :
        intervalIndex(intervalIndex)
    {
    }

    LocalNinjamInterval()
    {
    }

    void appendEncodedData(const QByteArray &data)
    {
        encodedData.append(data);
    }

    inline QByteArray getEncodedData() const
    {
        return encodedData;
    }

    void clear()
    {
        encodedData.clear();
        this->intervalIndex++;
    }

    inline int getIntervalIndex() const
    {
        return intervalIndex;
    }

    inline bool isEmpty() const
    {
        return encodedData.isEmpty();
    }

private:
    QByteArray encodedData;
    int intervalIndex;
};

class JamRecorder
{
public:
    explicit JamRecorder(JamMetadataWriter *jamMetadataWritter);
    ~JamRecorder();
    void appendLocalUserAudio(const QByteArray &encodedAudio, quint8 channelIndex,
                              bool isFirstPartOfInterval);

    void appendLocalUserVideo(const QByteArray &encodedVideo, bool isFirstPartOfInterval);

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

    void setDirNameDateFormat(Qt::DateFormat newDateFormat);

private:
    QString currentJamName;
    std::unique_ptr<Jam> jam;
    std::unique_ptr<JamMetadataWriter> jamMetadataWritter;
    int globalIntervalIndex;
    QString localUserName;
    bool running;
    QDir recordBaseDir;
    Qt::DateFormat dirNameDateFormat;

    /**
        Audio Intervals: Using channel index as key and store encoded bytes. When a full interval is stored the encoded bytes are store in a ogg file.
        Video Intervals: Using 255 as default channel index.
     */
    QMap<quint8, LocalNinjamInterval> localUserIntervals; // storing encoded data for audio and video intervals
    static const quint8 VIDEO_CHANNEL_KEY;

    QString getNewJamName();

    void writeEncodedFile(const QByteArray &encodedData, const QString &path);

    static QString buildAudioFileName(const QString &userName, quint8 channelIndex, int currentInterval);
    static QString buildVideoFileName(const QString &userName, int currentInterval, const QString &fileExtension);
    static QString buildPaddedFileNumber(int fileNumber);

    void writeProjectFile();

};

} // namespace

#endif
