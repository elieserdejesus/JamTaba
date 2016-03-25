#ifndef METRONOMEUTILS_H
#define METRONOMEUTILS_H

namespace Audio {
    class SamplesBuffer;
}

class MetronomeUtils
{

public:
    static void createDefaultSounds(Audio::SamplesBuffer &firstBeat, Audio::SamplesBuffer &secondaryBeat, quint32 localSampleRate);

private:
    static void createResampledBuffer(const Audio::SamplesBuffer &buffer, Audio::SamplesBuffer &outBuffer, int originalSampleRate,
                                         int finalSampleRate);
};

#endif // METRONOMEUTILS_H
