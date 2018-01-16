#ifndef _AUDIO_LOOPER_STATE_
#define _AUDIO_LOOPER_STATE_

#include <QtGlobal>
#include <QMap>

#include "Looper.h"

namespace audio {

class SamplesBuffer;

class LooperState
{

public:
    explicit LooperState(Looper *looper);
    virtual ~LooperState() {}

    virtual void mixTo(SamplesBuffer &samples, uint samplesToProcess) = 0;
    virtual void addBuffer(const SamplesBuffer &samples, uint samplesToProcess) = 0;

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
    void mixTo(SamplesBuffer &samples, uint samplesToProcess) override;
    void addBuffer(const SamplesBuffer &samples, uint samplesToProcess) override;
    inline bool isStopped() const override { return true ;}
};

// -------------------------------------------------------

class PlayingState : public LooperState
{
public:
    explicit PlayingState(Looper *looper);
    void handleNewCycle(uint samplesInCycle) override;
    void mixTo(SamplesBuffer &samples, uint samplesToProcess) override;
    void addBuffer(const SamplesBuffer &samples, uint samplesToProcess) override;
    inline bool isPlaying() const override { return true ;}
};

// -------------------------------------------------------

class WaitingToRecordState : public LooperState
{
public:
    explicit WaitingToRecordState(Looper *looper);
    void handleNewCycle(uint samplesInCycle) override;
    inline bool isWaiting() const override { return true ;}
    void addBuffer(const SamplesBuffer &samples, uint samplesToProcess) override;
    void mixTo(SamplesBuffer &samples, uint samplesToProcess) override;

private:
    SamplesBuffer lastInputBuffer;
};

// -------------------------------------------------------

class RecordingState : public LooperState
{
public:
    RecordingState(Looper *looper, quint8 recordingLayer);
    void handleNewCycle(uint samplesInCycle) override;
    void addBuffer(const SamplesBuffer &samples, uint samplesToProcess) override;
    void mixTo(SamplesBuffer &samples, uint samplesToProcess) override;
    inline bool isWaiting() const override { return false ;}
    inline bool isRecording() const override { return true ;}

private:
    const quint8 firstRecordingLayer; // used to watch when looper back to first rect layer and auto stop recording
};

// -------------------------------------------------------

} // namespace

#endif
