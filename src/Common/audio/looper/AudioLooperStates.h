#ifndef _AUDIO_LOOPER_STATE_
#define _AUDIO_LOOPER_STATE_

#include <QtGlobal>

namespace Audio {

class Looper;
class SamplesBuffer;

class LooperState
{

public:
    LooperState(Looper *looper);
    virtual void process(SamplesBuffer &samples, uint samplesToProcess);
    virtual void handleNewCycle(uint samplesInCycle) = 0;
    virtual inline bool isWaiting() const { return false; }
    virtual inline bool isStopped() const { return false; }
    virtual inline bool isRecording() const { return false; }
    virtual inline bool isPlaying() const { return false; }

protected:
    Looper *looper;
};

// -------------------------------------------------------

class StoppedState : public LooperState
{
public:
    StoppedState();
    void handleNewCycle(uint samplesInCycle) override;
    void process(SamplesBuffer &samples, uint samplesToProcess) override;
    inline bool isStopped() const override { return true ;}
};

// -------------------------------------------------------

class PlayingState : public LooperState
{
public:
    PlayingState(Looper *looper);
    void handleNewCycle(uint samplesInCycle) override;
    inline bool isPlaying() const override { return true ;}
};

// -------------------------------------------------------

class RecordingState : public LooperState
{
public:
    RecordingState(Looper *looper);
    void handleNewCycle(uint samplesInCycle) override;
    void process(SamplesBuffer &samples, uint samplesToProcess) override;
    inline bool isRecording() const override { return true ;}
};

// -------------------------------------------------------

class WaitingState : public LooperState
{
public:
    WaitingState(Looper *looper);
    void handleNewCycle(uint samplesInCycle) override;
    inline bool isWaiting() const override { return true ;}
};

} // namespace

#endif
