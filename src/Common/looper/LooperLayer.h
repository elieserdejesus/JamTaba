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

    void setGain(float gain);
    void setPan(float pan);

    float getPan() const;
    float getGain() const;

    void setSamples(const SamplesBuffer &samples);

    void zero();

    void reset();

    void overdub(const SamplesBuffer &samples, uint samplesToMix, uint startPosition);
    void append(const SamplesBuffer &samples, uint samplesToAppend, uint startPosition);

    void prepareForNewCycle(uint samplesInNewCycle, bool isOverdubbing);

    float computeMaxPeak(uint from, uint samplesPerPeak) const;

    std::vector<float> getSamplesPeaks(uint samplesPerPeak);

    SamplesBuffer getAllSamples() const;

    void mixTo(SamplesBuffer &outBuffer, uint samplesToMix, uint intervalPosition);

    void setLocked(bool locked);
    bool isLocked() const;
    bool isValid() const;

    enum MuteState
    {
        Unmuted,
        Muted,
        WaitingToMute, // waiting to mute in next interval
        WaitingToUnmute // waiting to UNmute in next interval
    };

    bool isMuted() const;
    void setMuteState(MuteState newState);
    MuteState getMuteState() const;

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

    float gain;

    float pan;
    float leftGain;
    float rightGain;

    MuteState muteState;

    void resize(quint32 samplesPerCycle);

};

inline LooperLayer::MuteState LooperLayer::getMuteState() const
{
    return muteState;
}

inline void LooperLayer::setMuteState(MuteState newState)
{
    muteState = newState;
}

inline bool LooperLayer::isMuted() const
{
    return muteState == MuteState::Muted;
}

inline float LooperLayer::getPan() const
{
    return pan;
}

inline float LooperLayer::getGain() const
{
    return gain;
}

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
