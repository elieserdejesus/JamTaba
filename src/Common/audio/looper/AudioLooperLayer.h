#ifndef _AUDIO_LOOPER_LAYER_
#define _AUDIO_LOOPER_LAYER_

#include <vector>
#include <QtGlobal>

namespace Audio {

class SamplesBuffer;

class LooperLayer
{
public:

    LooperLayer();
    virtual ~LooperLayer();

    void zero();

    void overdub(const SamplesBuffer &samples, uint samplesToMix, uint startPosition);
    void append(const SamplesBuffer &samples, uint samplesToAppend);

    void prepareForNewCycle(uint samplesInNewCycle, bool isOverdubbing);

    float computeMaxPeak(uint from, uint samplesPerPeak) const;

    std::vector<float> getSamplesPeaks(uint samplesPerPeak);

    void mixTo(SamplesBuffer &outBuffer, uint samplesToMix, uint intervalPosition);

    void setLocked(bool locked);
    bool isLocked() const;
    bool isValid() const;

    uint getAvailableSamples() const;

private:
    std::vector<float> leftChannel;
    std::vector<float> rightChannel;

    std::vector<float> peaksCache;
    uint lastSamplesPerPeak;
    uint availableSamples;
    uint lastCacheComputationSample;
    uint lastCycleLenght;
    bool locked;

    void resize(quint32 samplesPerCycle);

};

inline uint LooperLayer::getAvailableSamples() const
{
    return availableSamples;
}

inline bool LooperLayer::isValid() const
{
    return availableSamples > 0;
}

inline bool LooperLayer::isLocked() const
{
    return locked;
}

inline void LooperLayer::setLocked(bool locked)
{
    this->locked = locked;
}

} // namespace

#endif
