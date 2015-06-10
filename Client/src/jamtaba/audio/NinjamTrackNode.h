#ifndef NINJAMTRACKNODE_H
#define NINJAMTRACKNODE_H

#include "core/AudioNode.h"
#include <QByteArray>
#include "vorbis/VorbisDecoder.h"


namespace Audio{
    class SamplesBuffer;
    class StreamBuffer;
}

class NinjamTrackNode : public Audio::AudioNode
{
public:
    NinjamTrackNode();
    ~NinjamTrackNode();
    void addEncodedBytes(QByteArray encodedBytes, bool lastPartOfInterval);
    void processReplacing(Audio::SamplesBuffer&in, Audio::SamplesBuffer& out);
    void startNewInterval();
private:
    bool playing;//playing one interval or waiting for more vorbis data to decode
    int readIndex;//used to point to decoder to read
    int writeIndex;//used to point to decoder to write new vorbis data
    VorbisDecoder decoders[2];//use two decoders, one to decode the current interval and other to decode the next interval
    //float** decoderOutBuffer;
};

#endif // NINJAMTRACKNODE_H
