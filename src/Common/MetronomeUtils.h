#ifndef METRONOMEUTILS_H
#define METRONOMEUTILS_H

#include <QFile>
#include <QList>

class QString;

namespace Audio {

class SamplesBuffer;

struct MetronomeFilesPair
{
    QString primaryBeatFile;
    QString secondaryBeatFile;
    QString alias;

    MetronomeFilesPair(const QString &primaryBeatFile, const QString &secondaryBeatFile, const QString &alias);
};

class MetronomeUtils
{

public:
    static void createDefaultSounds(Audio::SamplesBuffer &firstBeat, Audio::SamplesBuffer &secondaryBeat, quint32 localSampleRate);
    static void createCustomSounds(const QString &firstBeatAudioFile, const QString &secondaryBeatAudioFile,
                                   Audio::SamplesBuffer &firstBeat, Audio::SamplesBuffer &secondaryBeat, quint32 localSampleRate);

    static void removeSilenceInBufferStart(Audio::SamplesBuffer &buffer);

    static QList<MetronomeFilesPair> getBuiltInMetronomeFiles();
private:
    static void createBuffer(const QString &audioFilePath, Audio::SamplesBuffer &outBuffer, quint32 localSampleRate);
    static void createResampledBuffer(const Audio::SamplesBuffer &buffer, Audio::SamplesBuffer &outBuffer, int originalSampleRate,
                                         int finalSampleRate);

    static const QString DEFAULT_METRONOME_AUDIO_FILE;
};

}//namespace

#endif // METRONOMEUTILS_H
