#ifndef SAMPLESBUFFER_H
#define SAMPLESBUFFER_H

#include "AudioPeak.h"
#include <vector>

#include <QtGlobal>

namespace audio {

class SamplesBuffer
{
    friend class AudioNodeProcessor;

private:
    unsigned int channels;
    unsigned int frameLenght;

    // rms calculations
    double rmsRunningSum;
    float squaredSums[2];
    int summedSamples;
    int rmsWindowSize; // how many samples until have enough data to compute rms?
    float lastRmsValues[2];

    std::vector<std::vector<float>> samples;

public:
    explicit SamplesBuffer(unsigned int channels);
    explicit SamplesBuffer(unsigned int channels, unsigned int frameLenght);
    SamplesBuffer(const SamplesBuffer &other);
    SamplesBuffer &operator=(const SamplesBuffer &other);
    ~SamplesBuffer();

    void setRmsWindowSize(int samples);
    static int computeRmsWindowSize(int sampleRate, int windowTimeInMs = 300); // using 300 ms as default

    static const SamplesBuffer ZERO_BUFFER; // a static buffer with zero samples

    bool isMono() const;

    float *getSamplesArray(unsigned int channel) const;

    void discardFirstSamples(unsigned int samplesToDiscard); // discard N samples and set frame lenght to new size
    void append(const SamplesBuffer &other);

    void applyGain(float gainFactor, float boostFactor);

    void fade(float beginGain = 0, float endGain = 1);
    void fadeIn(int fadeFrameLenght, float beginGain = 0);
    void fadeOut(int fadeFrameLenght, float endGain = 0);

    // overload applyGain used to compute main gain and pan gains in one pass
    // panValue between [-1, 0, 1] => LEFT, CENTER, RIGHT
    void applyGain(float gainFactor, float leftGain, float rightGain, float boostFactor);

    void zero();

    void setToMono();
    void setToStereo();

    void invertStereo();

    audio::AudioPeak computePeak();

    void add(const SamplesBuffer &buffer);

    void add(uint channel, uint sampleIndex, float sampleValue);
    void add(const SamplesBuffer &buffer, int internalWriteOffset);// the offset is used in internal buffer, not in parameter buffer
    void add(uint channel, float *samples, uint samplesToAdd);

    // copy samplesToCopy' samples starting from bufferOffset to internal buffer starting in 'internalOffset'
    void set(const SamplesBuffer &buffer, uint bufferOffset, uint samplesToCopy, uint internalOffset);
    void set(const SamplesBuffer &buffer);
    void set(const SamplesBuffer &buffer, int bufferChannelOffset, int channelsToCopy);
    void set(uint channel, uint sampleIndex, float sampleValue);

    float get(uint channel, uint sampleIndex) const;

    unsigned int getFrameLenght() const;
    void setFrameLenght(unsigned int newFrameLenght);

    int getChannels() const;

    bool isEmpty() const;
};


inline int SamplesBuffer::getChannels() const
{
    return channels;
}

inline bool SamplesBuffer::isEmpty() const
{
    return frameLenght == 0;
}

inline void SamplesBuffer::add(const SamplesBuffer &buffer)
{
    add(buffer, 0);
}

inline bool SamplesBuffer::isMono() const
{
    return channels == 1;
}

inline unsigned int SamplesBuffer::getFrameLenght() const
{
    return frameLenght;
}

} // namespace

#endif // SAMPLESBUFFER_H
