#ifndef NINJAMTRACKNODE_H
#define NINJAMTRACKNODE_H

#include "core/AudioNode.h"
#include <QByteArray>
#include "vorbis/VorbisDecoder.h"
#include "Resampler.h"

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
    void processReplacing(Audio::SamplesBuffer&in, Audio::SamplesBuffer& out);
    bool startNewInterval();
    inline int getID() const{return ID;}
    virtual bool needResamplingFor(int targetSampleRate) const;
    virtual int getSampleRate() const;
    inline bool isPlaying() const{return playing;}
private:
    bool playing;//playing one interval or waiting for more vorbis data to decode
    VorbisDecoder decoder;
    QList<QByteArray> intervals;
    //QMutex mutex;
    int ID;
    //Resampler resamplerLeft;
    //Resampler resamplerRight;
};

#endif // NINJAMTRACKNODE_H
