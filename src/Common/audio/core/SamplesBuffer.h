#ifndef SAMPLESBUFFER_H
#define SAMPLESBUFFER_H

#include "AudioPeak.h"
#include <vector>

namespace Audio {
class SamplesBuffer
{
    friend class AudioNodeProcessor;
private:
    unsigned int channels;
    unsigned int frameLenght;

    //rms calculations
    double rmsRunningSum;
    float squaredSums[2];
    int summedSamples;
    int rmsWindowSize; //how many samples until have enough data to compute rms?
    float lastRmsValues[2];

    std::vector< std::vector<float> > samples;

    SamplesBuffer &operator=(const SamplesBuffer &other);
public:
    explicit SamplesBuffer(unsigned int channels);
    explicit SamplesBuffer(unsigned int channels, unsigned int frameLenght);
    SamplesBuffer(const SamplesBuffer &other);
    ~SamplesBuffer();

    void setRmsWindowSize(int samples);
    static int computeRmsWindowSize(int sampleRate, int windowTimeInMs = 300); //using 300 ms as default

    static const SamplesBuffer ZERO_BUFFER;// a static buffer with zero samples

    inline bool isMono() const
    {
        return channels == 1;
    }

    float *getSamplesArray(unsigned int channel) const;

    void discardFirstSamples(unsigned int samplesToDiscard);// discard N samples and set frame lenght to new size
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

    Audio::AudioPeak computePeak();

    inline void add(const SamplesBuffer &buffer)
    {
        add(buffer, 0);
    }

    void add(int channel, int sampleIndex, float sampleValue);
    void add(const SamplesBuffer &buffer, int internalWriteOffset);// the offset is used in internal buffer, not in parameter buffer
    void add(unsigned int channel, float *samples, int samplesToAdd);

    // copy samplesToCopy' samples starting from bufferOffset to internal buffer starting in 'internalOffset'
    void set(const SamplesBuffer &buffer, unsigned int bufferOffset, unsigned int samplesToCopy,
             unsigned int internalOffset);
    void set(const SamplesBuffer &buffer);
    void set(const SamplesBuffer &buffer, int bufferChannelOffset, int channelsToCopy);
    void set(int channel, int sampleIndex, float sampleValue);

    float get(int channel, int sampleIndex) const;

    unsigned int getFrameLenght() const;
    void setFrameLenght(unsigned int newFrameLenght);
    inline int getChannels() const
    {
        return channels;
    }

    inline bool isEmpty() const
    {
        return frameLenght <= 0;
    }
};

inline unsigned int SamplesBuffer::getFrameLenght() const
{
    return frameLenght;
}

}
#endif // SAMPLESBUFFER_H
