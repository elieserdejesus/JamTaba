#ifndef AUDIOPEAK_H
#define AUDIOPEAK_H

namespace Audio {

class AudioPeak{

public:
    AudioPeak(float leftPeak, float rightPeak);
    explicit AudioPeak();
    void setLeft(float newLeftValue);
    void setRight(float newRightValue);
    float getMax() const;
    inline float getLeft() const{return left;}
    inline float getRight() const{return right;}
    void update(const AudioPeak& other);
    void zero();
private:
    float left;
    float right;
};
}

#endif // AUDIOPEAK_H
