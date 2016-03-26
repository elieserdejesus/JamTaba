#ifndef METRONOMEUTILS_H
#define METRONOMEUTILS_H

class QString;

namespace Audio {
    class SamplesBuffer;
}

class MetronomeUtils
{

public:
    static void createDefaultSounds(Audio::SamplesBuffer &firstBeat, Audio::SamplesBuffer &secondaryBeat, quint32 localSampleRate);
    static void createCustomSounds(const QString &firstBeatAudioFile, const QString &secondaryBeatAudioFile,
                                   Audio::SamplesBuffer &firstBeat, Audio::SamplesBuffer &secondaryBeat, quint32 localSampleRate);

    static void removeSilenceInBufferStart(Audio::SamplesBuffer &buffer);
private:
    static void createBuffer(const QString &audioFilePath, Audio::SamplesBuffer &outBuffer, quint32 localSampleRate);
    static void createResampledBuffer(const Audio::SamplesBuffer &buffer, Audio::SamplesBuffer &outBuffer, int originalSampleRate,
                                         int finalSampleRate);

    static const QString DEFAULT_METRONOME_AUDIO_FILE;
};

#endif // METRONOMEUTILS_H
