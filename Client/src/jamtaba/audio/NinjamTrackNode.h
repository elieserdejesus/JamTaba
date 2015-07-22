#ifndef NINJAMTRACKNODE_H
#define NINJAMTRACKNODE_H

#include "core/AudioNode.h"
#include <QByteArray>
#include "vorbis/VorbisDecoder.h"
#include "SamplesBufferResampler.h"


namespace Audio{
    class SamplesBuffer;
    class StreamBuffer;
}

class NinjamTrackNode : public Audio::AudioNode
{
public:
    explicit NinjamTrackNode(int ID);
    virtual ~NinjamTrackNode();
    void addVorbisEncodedInterval(QByteArray encodedBytes);
    void processReplacing(const Audio::SamplesBuffer&in, Audio::SamplesBuffer& out, int sampleRate);
    bool startNewInterval();
    inline int getID() const{return ID;}

    int getSampleRate() const;
    inline bool isPlaying() const{return playing;}
    void discardIntervals();
private:
    bool playing;//playing one interval or waiting for more vorbis data to decode
    VorbisDecoder decoder;
    QList<QByteArray> intervals;
    //QMutex mutex;
    int ID;
    SamplesBufferResampler resampler;

    bool needResamplingFor(int targetSampleRate) const;

    int getFramesToProcess(int targetSampleRate, int outFrameLenght);
};

#endif // NINJAMTRACKNODE_H
