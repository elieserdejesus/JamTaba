#ifndef METRONOMEUTILS_H
#define METRONOMEUTILS_H

#include <QFile>
#include <QList>

class QString;

namespace Audio {

class SamplesBuffer;

class MetronomeUtils
{

public:
    static void createBuiltInSounds(const QString &alias, Audio::SamplesBuffer &firstBeat, Audio::SamplesBuffer &secondaryBeat, quint32 localSampleRate);

    static void createCustomSounds(const QString &firstBeatAudioFile, const QString &secondaryBeatAudioFile,
                                   Audio::SamplesBuffer &firstBeat, Audio::SamplesBuffer &secondaryBeat, quint32 localSampleRate);

    static void removeSilenceInBufferStart(Audio::SamplesBuffer &buffer);

    static QList<QString> getBuiltInMetronomeAliases();
private:
    static void createBuffer(const QString &audioFilePath, Audio::SamplesBuffer &outBuffer, quint32 localSampleRate);
    static void createResampledBuffer(const Audio::SamplesBuffer &buffer, Audio::SamplesBuffer &outBuffer, int originalSampleRate,
                                         int finalSampleRate);

    static QString buildMetronomeFileNameFromAlias(const QString &alias, bool isFirstBeat);

    static const QString DEFAULT_BUILT_IN_METRONOME_ALIAS;
    static const QString DEFAULT_BUILT_IN_METRONOME_DIR;
};

}//namespace

#endif // METRONOMEUTILS_H
