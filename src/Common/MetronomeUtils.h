#ifndef METRONOMEUTILS_H
#define METRONOMEUTILS_H

#include <QFile>
#include <QList>

class QString;

namespace audio {

class SamplesBuffer;

class metronomeUtils
{

public:
    static void createBuiltInSounds(const QString &alias, SamplesBuffer &firstBeat, SamplesBuffer &offBeatBuffer, SamplesBuffer &accentBeatBuffer, quint32 localSampleRate);

    static void createCustomSounds(const QString &firstBeatAudioFile, const QString &offBeatAudioFile, const QString &accentBeatAudioFile,
                                   SamplesBuffer &firstBeat, SamplesBuffer &offBeat, SamplesBuffer &accentBeatBuffer, quint32 localSampleRate);

    static void removeSilenceInBufferStart(SamplesBuffer &buffer);

    static QList<QString> getBuiltInMetronomeAliases();

    static QList<int> getAccentBeats(int beatsPerAccent, int bpi);

    static QList<int> getAccentBeatsFromString(QString value);

private:
    static void createBuffer(const QString &audioFilePath, SamplesBuffer &outBuffer, quint32 localSampleRate);
    static void createResampledBuffer(const SamplesBuffer &buffer, SamplesBuffer &outBuffer, int originalSampleRate, int finalSampleRate);

    static QString buildMetronomeFileNameFromAlias(const QString &alias, const QString &Beat);

    static void createBuiltInSound(const QString &alias, const QString &beat, SamplesBuffer &beatBuffer, quint32 localSampleRate);

    static const QString DEFAULT_BUILT_IN_METRONOME_ALIAS;
    static const QString DEFAULT_BUILT_IN_METRONOME_DIR;
};

}//namespace

#endif // METRONOMEUTILS_H
