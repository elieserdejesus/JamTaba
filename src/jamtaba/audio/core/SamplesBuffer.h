#ifndef SAMPLESBUFFER_H
#define SAMPLESBUFFER_H

namespace Audio {

class SamplesBuffer{

    friend class AudioNodeProcessor;

public:
    float** samples;//deixei public porque o ogg decoder não funcionou de outra maneira, tive que acessar diretamente o endereço desse array
private:
    unsigned int channels;
    unsigned int frameLenght;
    unsigned const int maxFrameLenght;
    int offset;

    mutable float peaks[2];

    inline bool isMono() const {return channels == 1;}

    inline bool channelIsValid(unsigned int channel) const{return channel < channels;}
    inline bool sampleIndexIsValid(unsigned int sampleIndex) const{return sampleIndex < frameLenght;}

    void computePeaks();

    //achei que isso iria gerar error nos lugares onde acontecem copias, mas não rolou
    //tentar implementar esses métodos e disparar warnings para ver


public:
    SamplesBuffer(unsigned int channels, const unsigned int MAX_BUFFERS_LENGHT);
    SamplesBuffer(const SamplesBuffer& other);
    SamplesBuffer& operator=(const SamplesBuffer& other);

    ~SamplesBuffer();

    static const SamplesBuffer ZERO_BUFFER;//a static buffer with zero samples


    void setOffset(int offset);
    void resetOffset();

    inline float** getSamplesArray() const{return samples;}

    //inline int getID() const{return ID;}
    void applyGain(float gainFactor);

    void fade(float beginGain=0, float endGain=1);


    //overload applyGain used to compute main gain and pan gains in one pass
    void applyGain(float gainFactor, float leftGain, float rightGain);//panValue between [-1, 0, 1] => LEFT, CENTER, RIGHT

    void zero();


    const float *getPeaks() const;//{return peaks[channel];}

    inline void add(const SamplesBuffer& buffer){add(buffer, 0);}
    void add(int channel, int sampleIndex, float sampleValue);
    void add(const SamplesBuffer& buffer, int offset);//the offset is used in internal buffer, not in parameter buffer

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
