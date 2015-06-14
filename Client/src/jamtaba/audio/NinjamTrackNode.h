#ifndef NINJAMTRACKNODE_H
#define NINJAMTRACKNODE_H

#include "core/AudioNode.h"
#include <QByteArray>
#include "vorbis/VorbisDecoder.h"
#include <QList>
#include <QMutex>

namespace Audio{
    class SamplesBuffer;
    class StreamBuffer;
}

class NinjamInterval;

class NinjamTrackNode : public Audio::AudioNode
{
public:
    NinjamTrackNode(int ID);
    ~NinjamTrackNode();
    void addEncodedBytes(QByteArray encodedBytes, bool lastPartOfInterval);
    void processReplacing(Audio::SamplesBuffer&in, Audio::SamplesBuffer& out);
    void startNewInterval();
    inline int getID() const{return ID;}
private:
    bool playing;//playing one interval or waiting for more vorbis data to decode
    VorbisDecoder decoder;
    QList<NinjamInterval> intervals;
    QMutex mutex;
    int ID;
};

#endif // NINJAMTRACKNODE_H
