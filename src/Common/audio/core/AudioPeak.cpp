#include "AudioPeak.h"
#include <algorithm>
#include <QtGlobal>

using namespace audio;

AudioPeak::AudioPeak(float leftPeak, float rightPeak, float rmsLeft, float rmsRight)
{
    peaks[0] = leftPeak;
    peaks[1] = rightPeak;

    rms[0] = rmsLeft;
    rms[1] = rmsRight;
}

AudioPeak::AudioPeak()
{
    zero();
}

AudioPeak AudioPeak::operator -(const AudioPeak &otherPeak)
{
    float leftPeak = peaks[0] - otherPeak.peaks[0];
    float rightPeak = peaks[1] - otherPeak.peaks[1];
    float leftRms = rms[0] = otherPeak.rms[0];
    float rightRms = rms[1] = otherPeak.rms[1];

    return AudioPeak(leftPeak, rightPeak, leftRms, rightRms);
}

void AudioPeak::update(const AudioPeak &other)
{
    peaks[0] = other.peaks[0];
    peaks[1] = other.peaks[1];

    rms[0] = other.rms[0];
    rms[1] = other.rms[1];
}

void AudioPeak::zero()
{
    peaks[0] = 0.0f;
    peaks[1] = 0.0f;

    rms[0] = 0.0f;
    rms[1] = 0.0f;
}

float AudioPeak::getMaxPeak() const
{
    return std::max(qAbs(peaks[0]), qAbs(peaks[1]));
}
