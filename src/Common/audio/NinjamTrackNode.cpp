#include "NinjamTrackNode.h"

#include <QDataStream>
#include <QDebug>
#include <QList>
#include <QByteArray>
#include <QMutexLocker>
#include <QDateTime>
#include <QtConcurrent/QtConcurrent>

#include "audio/core/Filters.h"
#include "audio/core/AudioDriver.h"
#include "audio/vorbis/VorbisDecoder.h"

const double NinjamTrackNode::LOW_CUT_DRASTIC_FREQUENCY = 220.0; // in Hertz
const double NinjamTrackNode::LOW_CUT_NORMAL_FREQUENCY = 120.0; // in Hertz

using audio::Filter;

class NinjamTrackNode::LowCutFilter
{
public:
    explicit LowCutFilter(double sampleRate);
    void process(audio::SamplesBuffer &buffer);
    //inline bool isActivated() const { return activated; }
    inline NinjamTrackNode::LowCutState getState(){ return this->state; }
    void setState(NinjamTrackNode::LowCutState state);
private:
    NinjamTrackNode::LowCutState state;
    Filter leftFilter;
    Filter rightFilter;
};

NinjamTrackNode::LowCutFilter::LowCutFilter(double sampleRate) :
    state(LowCutState::OFF),
    leftFilter(Filter::FilterType::HighPass, sampleRate, LOW_CUT_NORMAL_FREQUENCY, 1.0, 1.0),
    rightFilter(Filter::FilterType::HighPass, sampleRate, LOW_CUT_NORMAL_FREQUENCY, 1.0, 1.0)
{

}

void NinjamTrackNode::LowCutFilter::setState(NinjamTrackNode::LowCutState state)
{
    this->state = state;
    if (state != LowCutState::OFF){
        double frequency = LOW_CUT_NORMAL_FREQUENCY;
        if (state == LowCutState::DRASTIC)
            frequency = LOW_CUT_DRASTIC_FREQUENCY;

        leftFilter.setFrequency(frequency);
        rightFilter.setFrequency(frequency);
    }
}

void NinjamTrackNode::LowCutFilter::process(audio::SamplesBuffer &buffer)
{
    if (state == LowCutState::OFF)
        return;

    quint32 samples = buffer.getFrameLenght();
    leftFilter.process(buffer.getSamplesArray(0), samples);
    if (!buffer.isMono())
        rightFilter.process(buffer.getSamplesArray(1), samples);
}

//--------------------------------------------------------------------------

class NinjamTrackNode::IntervalDecoder
{
public:
    explicit IntervalDecoder(const QByteArray &vorbisData);
    void decode(quint32 maxSamplesToDecode);
    quint32 getDecodedSamples(audio::SamplesBuffer &outBuffer, uint samplesToDecode);
    inline int getSampleRate() const { return vorbisDecoder.getSampleRate(); }
    inline bool isStereo() const { return vorbisDecoder.isStereo(); }
    void stopDecoding();
private:
    vorbis::Decoder vorbisDecoder;
    audio::SamplesBuffer decodedBuffer;
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

void NinjamTrackNode::IntervalDecoder::stopDecoding()
{
    mutex.lock(); // this funcion is called from GUI thread

    vorbisDecoder.setInputData(QByteArray()); // empty data

    mutex.unlock();
}

quint32 NinjamTrackNode::IntervalDecoder::getDecodedSamples(audio::SamplesBuffer &outBuffer, uint samplesToDecode)
{
    mutex.lock();
    while (decodedBuffer.getFrameLenght() < samplesToDecode) { //need decode more samples to fill outBuffer?
        quint32 toDecode = samplesToDecode - decodedBuffer.getFrameLenght();
        const auto &decodedSamples = vorbisDecoder.decode(toDecode);
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
    lowCut(new NinjamTrackNode::LowCutFilter(44100)),
    processingLastPartOfInterval(false),
    currentDecoder(nullptr),
    decodersMutex(QMutex::NonRecursive)
{

}

bool NinjamTrackNode::isStereo() const
{
    if (currentDecoder)
        return currentDecoder->isStereo();

    return true;
}

void NinjamTrackNode::stopDecoding()
{
    discardDownloadedIntervals(false);

    if (currentDecoder != nullptr)
        currentDecoder->stopDecoding();
}

NinjamTrackNode::LowCutState NinjamTrackNode::setLowCutToNextState()
{
    LowCutState newState = LowCutState::OFF;

    switch (lowCut->getState() ) {

    case LowCutState::OFF:
        newState = LowCutState::NORMAl;
        break;
    case LowCutState::NORMAl:
        newState = LowCutState::DRASTIC;
        break;
    case LowCutState::DRASTIC:
        newState = LowCutState::OFF;
        break;
    }

    lowCut->setState(newState);

    return newState;
}

NinjamTrackNode::LowCutState NinjamTrackNode::getLowCutState() const
{
    return lowCut->getState();
}

void NinjamTrackNode::setLowCutState(LowCutState newState)
{
    lowCut->setState(newState);
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
    if (currentDecoder) {
        delete currentDecoder;
        currentDecoder = nullptr;
    }
    decodersMutex.unlock();
}

void NinjamTrackNode::discardDownloadedIntervals(bool keepMostRecentInterval)
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
        delete currentDecoder; //discard the previous interval decoder
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

void NinjamTrackNode::processReplacing(const audio::SamplesBuffer &in, audio::SamplesBuffer &out,
                                       int sampleRate, std::vector<midi::MidiMessage> &midiBuffer)
{
    if (!isPlaying())
        return;

    Q_ASSERT(currentDecoder);
    int framesToProcess = getFramesToProcess(sampleRate, out.getFrameLenght());
    internalInputBuffer.setFrameLenght(framesToProcess);
    currentDecoder->getDecodedSamples(internalInputBuffer, framesToProcess);

    if (!internalInputBuffer.isEmpty()) {
        if (needResamplingFor(sampleRate)) {
            const auto &resampledBuffer = resampler.resample(internalInputBuffer, out.getFrameLenght());
            internalInputBuffer.setFrameLenght(resampledBuffer.getFrameLenght());
            internalInputBuffer.set(resampledBuffer);
            if (internalInputBuffer.getFrameLenght() != out.getFrameLenght())
                qWarning() << internalInputBuffer.getFrameLenght() << " != "
                           << out.getFrameLenght();
        }

        lowCut->process(internalInputBuffer);

        audio::AudioNode::processReplacing(in, out, sampleRate, midiBuffer); // process internal buffer pan, gain, etc
    }
}

bool NinjamTrackNode::needResamplingFor(int targetSampleRate) const
{
    if (currentDecoder)
        return currentDecoder->getSampleRate() != targetSampleRate;
    return false;
}
