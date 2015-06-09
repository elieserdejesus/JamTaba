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
    void addEncodedBytes(QByteArray encodedBytes);
    void processReplacing(Audio::SamplesBuffer&in, Audio::SamplesBuffer& out);

private:
    QByteArray encodedBytes;
    VorbisDecoder decoder;
    Audio::StreamBuffer* streamBuffer;
    char outBuffer[1024 * 10];

    void copyStereoDecodedBytesToStream(int decodedBytes);
    void copyMonoDecodedBytesToStream(int decodedBytes);
};

#endif // NINJAMTRACKNODE_H
