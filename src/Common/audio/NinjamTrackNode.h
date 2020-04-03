#ifndef NINJAMTRACKNODE_H
#define NINJAMTRACKNODE_H

#include "core/AudioNode.h"
#include <QByteArray>
#include "SamplesBufferResampler.h"

namespace audio {
class SamplesBuffer;
class StreamBuffer;
}

class NinjamTrackNode : public audio::AudioNode
{

public:

    enum LowCutState
    {
        Off, Normal, Drastic
    };

    enum ChannelMode {
        Intervalic,
        VoiceChat
    };

    explicit NinjamTrackNode(int ID);
    virtual ~NinjamTrackNode();
    void addVorbisEncodedInterval(const QByteArray &fullIntervalBytes);
    void addVorbisEncodedChunk(const QByteArray &chunkBytes, bool isLastPart);
    void processReplacing(const audio::SamplesBuffer &in, audio::SamplesBuffer &out, int sampleRate,
                          std::vector<midi::MidiMessage> &midiBuffer) override;

    void setLowCutState(LowCutState newState);
    LowCutState setLowCutToNextState();
    LowCutState getLowCutState() const;

    bool startNewInterval();
    int getID() const;

    int getSampleRate() const;

    bool isPlaying();

    bool isStereo() const;

    bool isIntervalic() const { return mode == Intervalic; }

    bool isVoiceChat() const { return mode == VoiceChat; }

    void setChannelMode(ChannelMode mode);

    // Discard all downloaded (but not played yet) intervals
    void discardDownloadedIntervals();

    void stopDecoding();

    //void setProcessingLastPartOfInterval(bool status);

private:
    int ID;
    SamplesBufferResampler resampler;

    class LowCutFilter;
    QScopedPointer<LowCutFilter> lowCut;
    const static double LOW_CUT_NORMAL_FREQUENCY;
    const static double LOW_CUT_DRASTIC_FREQUENCY;

    bool needResamplingFor(int targetSampleRate) const;

    int getFramesToProcess(int targetSampleRate, int outFrameLenght);

    //bool processingLastPartOfInterval;

    class IntervalDecoder;

    QList<IntervalDecoder*> decoders;
    IntervalDecoder* currentDecoder;
    QMutex decodersMutex;

    ChannelMode mode = Intervalic;

};

//inline void NinjamTrackNode::setProcessingLastPartOfInterval(bool status)
//{
//    this->processingLastPartOfInterval = status;
//}

inline int NinjamTrackNode::getID() const
{
    return ID;
}

#endif // NINJAMTRACKNODE_H
