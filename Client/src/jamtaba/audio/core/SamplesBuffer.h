#ifndef SAMPLESBUFFER_H
#define SAMPLESBUFFER_H

#include "AudioPeak.h"
#include <vector>

namespace Audio {



class SamplesBuffer{
    friend class AudioNodeProcessor;
private:
    unsigned int channels;
    unsigned int frameLenght;

    std::vector< std::vector<float>> samples;

    inline bool isMono() const {return channels == 1;}


    inline bool channelIsValid(unsigned int channel) const{return channel < channels;}
    inline bool sampleIndexIsValid(unsigned int sampleIndex) const{return sampleIndex < frameLenght;}


    SamplesBuffer& operator=(const SamplesBuffer& other);
public:
    SamplesBuffer(unsigned int channels);
    SamplesBuffer(unsigned int channels, unsigned int frameLenght);
    SamplesBuffer(const SamplesBuffer& other);
    ~SamplesBuffer();

    static const SamplesBuffer ZERO_BUFFER;//a static buffer with zero samples

    float* getSamplesArray(unsigned int channel) const;

    //inline int getID() const{return ID;}
    void applyGain(float gainFactor);

    void fade(float beginGain=0, float endGain=1);
    void fadeIn(int fadeFrameLenght,  float beginGain = 0);
    void fadeOut(int fadeFrameLenght, float endGain = 0);

    //overload applyGain used to compute main gain and pan gains in one pass
    void applyGain(float gainFactor, float leftGain, float rightGain);//panValue between [-1, 0, 1] => LEFT, CENTER, RIGHT

    void zero();

    void setToMono();
    void setToStereo();

    Audio::AudioPeak computePeak() const;//{return peaks[channel];}

    inline void add(const SamplesBuffer& buffer){add(buffer, 0);}
    void add(int channel, int sampleIndex, float sampleValue);
    void add(const SamplesBuffer& buffer, int internalWriteOffset);//the offset is used in internal buffer, not in parameter buffer
    void add(unsigned int channel, float* samples, int samplesToAdd);

    /***
     * copy samplesToCopy' samples starting from bufferOffset to internal buffer starting in 'internalOffset'
     */
    void set(const SamplesBuffer& buffer, unsigned int bufferOffset, unsigned int samplesToCopy, unsigned int internalOffset);
    void set(const SamplesBuffer& buffer);
    void set(const SamplesBuffer& buffer, int bufferChannelOffset, int channelsToCopy);
    void set(int channel, int sampleIndex, float sampleValue);

    float get( int channel,  int sampleIndex) const;


    //inline int getSampleRate(){ return sampleRate; }
    int getFrameLenght() const;//{ return frameLenght; }
    void setFrameLenght(unsigned int newFrameLenght);
    inline int getChannels() const { return channels; }
};

}
#endif // SAMPLESBUFFER_H
