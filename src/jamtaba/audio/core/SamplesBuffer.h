#ifndef SAMPLESBUFFER_H
#define SAMPLESBUFFER_H

#include "AudioPeak.h"

namespace Audio {



class SamplesBuffer{

    friend class AudioNodeProcessor;

//public:
    //deixei public porque o ogg decoder não funcionou de outra maneira, tive que acessar diretamente o endereço desse array
private:
    unsigned int channels;
    unsigned int frameLenght;
    unsigned const int maxFrameLenght;
    int offset;

    float** samples;//poderia deixar fixo com 2 canais e 4096 samples

    inline bool isMono() const {return channels == 1;}

    inline bool channelIsValid(unsigned int channel) const{return channel < channels;}
    inline bool sampleIndexIsValid(unsigned int sampleIndex) const{return sampleIndex < frameLenght;}

    //void computePeaks();

    SamplesBuffer(const SamplesBuffer& other);
    SamplesBuffer& operator=(const SamplesBuffer& other);

public:
    SamplesBuffer(unsigned int channels, const unsigned int MAX_BUFFERS_LENGHT);
    ~SamplesBuffer();

    static const SamplesBuffer ZERO_BUFFER;//a static buffer with zero samples

    //void copyLeftChannelToRight();//usefull to transform a mono input in a pseudo-stereo (both channels are the same)

    void setOffset(int offset);
    void resetOffset();

    inline float** getSamplesArray() const{return samples;}

    //inline int getID() const{return ID;}
    void applyGain(float gainFactor);

    void fade(float beginGain=0, float endGain=1);


    //overload applyGain used to compute main gain and pan gains in one pass
    void applyGain(float gainFactor, float leftGain, float rightGain);//panValue between [-1, 0, 1] => LEFT, CENTER, RIGHT

    void zero();


    Audio::AudioPeak computePeak() const;//{return peaks[channel];}

    inline void add(const SamplesBuffer& buffer){add(buffer, 0);}
    void add(int channel, int sampleIndex, float sampleValue);
    void add(const SamplesBuffer& buffer, int offset);//the offset is used in internal buffer, not in parameter buffer
    void add(int channel, float* samples, int samplesToAdd);

    /***
     * copy samplesToCopy' samples starting from bufferOffset to internal buffer starting in 'internalOffset'
     */
    void set(const SamplesBuffer& buffer, unsigned int bufferOffset, unsigned int samplesToCopy, unsigned int internalOffset);
    void set(const SamplesBuffer& buffer);
    void set(int channel, int sampleIndex, float sampleValue);

    float get( int channel,  int sampleIndex) const;


    //inline int getSampleRate(){ return sampleRate; }
    int getFrameLenght() const;//{ return frameLenght; }
    void setFrameLenght(unsigned int newFrameLenght);
    inline int getChannels() const { return channels; }
};

}
#endif // SAMPLESBUFFER_H
