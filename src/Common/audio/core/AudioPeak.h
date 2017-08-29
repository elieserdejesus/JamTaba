#ifndef AUDIOPEAK_H
#define AUDIOPEAK_H

namespace Audio
{

class AudioPeak
{

public:
    AudioPeak(float leftPeak, float rightPeak, float rmsLeft, float rmsRight);
    explicit AudioPeak();
    float getMaxPeak() const;
    float getLeftPeak() const;
    float getRightPeak() const;

    float getLeftRMS() const;
    float getRightRMS() const;

    void update(const AudioPeak &other);
    void zero();

    AudioPeak operator-(const AudioPeak &otherPeak);

private:
    float peaks[2]; // max peaks
    float rms[2]; // rms values
};

inline float AudioPeak::getLeftPeak() const
{
    return peaks[0];
}

inline float AudioPeak::getRightPeak() const
{
    return peaks[1];
}

inline float AudioPeak::getLeftRMS() const{
    return rms[0];
}

inline float AudioPeak::getRightRMS() const
{
    return rms[1];
}

} // namespace

#endif // AUDIOPEAK_H
