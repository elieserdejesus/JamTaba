#ifndef NINJAMTRACKNODE_H
#define NINJAMTRACKNODE_H

#include "core/AudioNode.h"
#include <QByteArray>
#include "vorbis/VorbisDecoder.h"
#include "SamplesBufferResampler.h"

namespace Audio {
class SamplesBuffer;
class StreamBuffer;
}

class NinjamTrackNode : public Audio::AudioNode
{
public:
    explicit NinjamTrackNode(int ID);
    virtual ~NinjamTrackNode();
    void addVorbisEncodedInterval(const QByteArray &encodedBytes);
    void processReplacing(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate,
                          const Midi::MidiMessageBuffer &midiBuffer);
    bool startNewInterval();
    inline int getID() const
    {
        return ID;
    }

    int getSampleRate() const;

    bool isPlaying();

    void discardIntervals(bool keepMostRecentInterval);
    inline void setProcessingLastPartOfInterval(bool status)
    {
        this->processingLastPartOfInterval = status;
    }

private:
    int ID;
    SamplesBufferResampler resampler;

    bool needResamplingFor(int targetSampleRate) const;

    int getFramesToProcess(int targetSampleRate, int outFrameLenght);

    bool processingLastPartOfInterval;

    class IntervalDecoder;

    QList<IntervalDecoder*> decoders;
    IntervalDecoder* currentDecoder;
    QMutex decodersMutex;

};

#endif // NINJAMTRACKNODE_H
