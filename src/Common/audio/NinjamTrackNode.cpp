#include "NinjamTrackNode.h"
#include "audio/core/AudioDriver.h"
#include <QDataStream>
#include <QDebug>
#include <QList>
#include <QByteArray>
#include <QMutexLocker>
#include <QDateTime>
#include <QtConcurrent/QtConcurrent>


class NinjamTrackNode::IntervalDecoder
{
public:
    IntervalDecoder(const QByteArray &vorbisData);
    void decode(quint32 maxSamplesToDecode);
    quint32 getDecodedSamples(Audio::SamplesBuffer &outBuffer, int samplesToDecode);
    inline int getSampleRate() { return vorbisDecoder.getSampleRate(); }
private:
    VorbisDecoder vorbisDecoder;
    Audio::SamplesBuffer decodedBuffer;
    QMutex mutex;
};

NinjamTrackNode::IntervalDecoder::IntervalDecoder(const QByteArray &vorbisData)
    :decodedBuffer(2)
{
    vorbisDecoder.setInputData(vorbisData);
}

void NinjamTrackNode::IntervalDecoder::decode(quint32 maxSamplesToDecode)
{
    mutex.lock();

    decodedBuffer.append(vorbisDecoder.decode(maxSamplesToDecode));

    mutex.unlock();
}

quint32 NinjamTrackNode::IntervalDecoder::getDecodedSamples(Audio::SamplesBuffer &outBuffer, int samplesToDecode)
{
    mutex.lock();
    while (decodedBuffer.getFrameLenght() < samplesToDecode) { //need decode more samples to fill outBuffer?
        quint32 toDecode = samplesToDecode - decodedBuffer.getFrameLenght();
        const Audio::SamplesBuffer &decodedSamples = vorbisDecoder.decode(toDecode);
        decodedBuffer.append(decodedSamples);
        if (decodedSamples.isEmpty())
            break; //no more samples to decode
    }

    quint32 totalSamples = qMin(samplesToDecode, decodedBuffer.getFrameLenght());
    outBuffer.setFrameLenght(totalSamples);
    outBuffer.set(decodedBuffer);
    decodedBuffer.discardFirstSamples(totalSamples);
    mutex.unlock();
    return totalSamples;
}

//-------------------------------------------------------------

NinjamTrackNode::NinjamTrackNode(int ID) :
    ID(ID),
    processingLastPartOfInterval(false),
    currentDecoder(nullptr),
    decodersMutex(QMutex::NonRecursive)
{

}

int NinjamTrackNode::getSampleRate() const
{
    if (currentDecoder)
        return currentDecoder->getSampleRate();
    return 44100;
}

NinjamTrackNode::~NinjamTrackNode()
{
    decodersMutex.lock();
    foreach (IntervalDecoder *decoder, decoders)
        delete decoder;

    decoders.clear();
    decodersMutex.unlock();
}

void NinjamTrackNode::discardIntervals(bool keepMostRecentInterval)
{
    decodersMutex.lock();
    if (!keepMostRecentInterval) {
        while (!decoders.isEmpty())
            delete decoders.takeFirst();
    } else {
        while(decoders.size() > 1)//keep the last downloaded interval
            delete decoders.takeFirst();
    }
    qDebug() << "intervals discarded";
    decodersMutex.unlock();
}

bool NinjamTrackNode::isPlaying()
{
    QMutexLocker locker(&decodersMutex);
    return currentDecoder != nullptr;
}

bool NinjamTrackNode::startNewInterval()
{
    decodersMutex.lock();
    if (currentDecoder) {
        delete currentDecoder; //discard the precious interval decoder
        currentDecoder = nullptr;
    }
    if (!decoders.isEmpty())
        currentDecoder = decoders.takeFirst(); //using the next buffered decoder (next interval)

    decodersMutex.unlock();
    return isPlaying();
}

void NinjamTrackNode::addVorbisEncodedInterval(const QByteArray &vorbisData)
{
    decodersMutex.lock();
    IntervalDecoder *newIntervalDecoder = new IntervalDecoder(vorbisData);
    decoders.append(newIntervalDecoder);
    decodersMutex.unlock();

    //decoding the first samples in a separated thread to avoid slow down the audio thread in interval start (first beat)
    QtConcurrent::run(newIntervalDecoder, &NinjamTrackNode::IntervalDecoder::decode, 256);
}

// ++++++++++++++++++++++++++++++++++++++

int NinjamTrackNode::getFramesToProcess(int targetSampleRate, int outFrameLenght)
{
    return needResamplingFor(targetSampleRate) ? getInputResamplingLength(
        getSampleRate(), targetSampleRate, outFrameLenght) : outFrameLenght;
}

void NinjamTrackNode::processReplacing(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                                       int sampleRate, const Midi::MidiMessageBuffer &midiBuffer)
{
    if (!isPlaying())
        return;

    Q_ASSERT(currentDecoder);
    int framesToProcess = getFramesToProcess(sampleRate, out.getFrameLenght());
    internalInputBuffer.setFrameLenght(framesToProcess);
    currentDecoder->getDecodedSamples(internalInputBuffer, framesToProcess);

    if (!internalInputBuffer.isEmpty()) {
        if (needResamplingFor(sampleRate)) {
            const Audio::SamplesBuffer &resampledBuffer = resampler.resample(internalInputBuffer,
                                                                             out.getFrameLenght());
            internalInputBuffer.setFrameLenght(resampledBuffer.getFrameLenght());
            internalInputBuffer.set(resampledBuffer);
            if (internalInputBuffer.getFrameLenght() != out.getFrameLenght())
                qWarning() << internalInputBuffer.getFrameLenght() << " != "
                           << out.getFrameLenght();
        }
        Audio::AudioNode::processReplacing(in, out, sampleRate, midiBuffer);// process internal buffer pan, gain, etc
    }
}

bool NinjamTrackNode::needResamplingFor(int targetSampleRate) const
{
    if (currentDecoder)
        return currentDecoder->getSampleRate() != targetSampleRate;
    return false;
}
