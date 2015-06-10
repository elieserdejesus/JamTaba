#ifndef STREAMBUFFER_H
#define STREAMBUFFER_H

namespace Audio {

class SamplesBuffer;

//Esta classe acabou não sendo usada

class StreamBuffer{
public:
    StreamBuffer(int channels, int capacity);
    ~StreamBuffer();
    void add(float leftSample, float rightSample);
    void add(float monoSample);
    void fillBuffer(Audio::SamplesBuffer& buffer);
    inline int getAvailable() const{return available;}
private:
    float** samples;
    int channels;
    int capacity;
    int writeIndex;
    int readIndex;
    int available;
};
}//namespace
#endif // STREAMBUFFER_H
