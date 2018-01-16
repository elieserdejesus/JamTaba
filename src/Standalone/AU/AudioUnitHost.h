#ifndef AUDIOUNITHOST_H
#define AUDIOUNITHOST_H

#include "audio/Host.h"
#include <AudioUnit/AudioUnit.h>

#include <QScopedPointer>

namespace au {

class AudioUnitHost : public Host
{

public:
    static AudioUnitHost *getInstance();

    int getSampleRate() const override;
    int getBufferSize() const override;

    std::vector<Midi::MidiMessage> pullReceivedMidiMessages() override;

    void setSampleRate(int sampleRate) override;
    void setBlockSize(int blockSize) override;
    void setTempo(int bpm) override;
    void setPlayingFlag(bool playing) override;
    void setPositionInSamples(int intervalPosition) override;

    int getBeat() const;
    int getTempo() const;
    int getTimeSignatureNumerator() const;
    int getTimeSignatureDenominator() const;
    bool isPlaying() const;
    int getPosition() const;

private:

    AudioUnitHost();

    static QScopedPointer<AudioUnitHost> instance;

    int sampleRate;
    int bufferSize;
    Float64 tempo;
    Float64 beat;
    int timeSignatureNumerator;
    int timeSignatureDenominator;
    bool playing;
    int position;
};

inline int AudioUnitHost::getPosition() const
{
    return position;
}

inline bool AudioUnitHost::isPlaying() const
{
    return playing;
}

inline int AudioUnitHost::getTimeSignatureNumerator() const
{
    return timeSignatureNumerator;
}

inline int AudioUnitHost::getTimeSignatureDenominator() const
{
    return timeSignatureDenominator;
}

inline int AudioUnitHost::getBeat() const
{
    return beat;
}

inline int AudioUnitHost::getTempo() const
{
    return tempo;
}

inline int AudioUnitHost::getSampleRate() const
{
    return sampleRate;
}

inline int AudioUnitHost::getBufferSize() const
{
    return bufferSize;
}

} // namespace

#endif // AUDIOUNITHOST_H
