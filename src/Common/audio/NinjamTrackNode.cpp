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
    state(LowCutState::Off),
    leftFilter(Filter::FilterType::HighPass, sampleRate, LOW_CUT_NORMAL_FREQUENCY, 1.0, 1.0),
    rightFilter(Filter::FilterType::HighPass, sampleRate, LOW_CUT_NORMAL_FREQUENCY, 1.0, 1.0)
{

}

void NinjamTrackNode::LowCutFilter::setState(NinjamTrackNode::LowCutState state)
{
    this->state = state;
    if (state != LowCutState::Off){
        double frequency = LOW_CUT_NORMAL_FREQUENCY;
        if (state == LowCutState::Drastic)
            frequency = LOW_CUT_DRASTIC_FREQUENCY;

        leftFilter.setFrequency(frequency);
        rightFilter.setFrequency(frequency);
    }
}

void NinjamTrackNode::LowCutFilter::process(audio::SamplesBuffer &buffer)
{
    if (state == LowCutState::Off)
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
    explicit IntervalDecoder(const QByteArray &vorbisData = QByteArray());
    void decode(quint32 maxSamplesToDecode);
    void addEncodedData(const QByteArray &vorbisData);
    quint32 getDecodedSamples(audio::SamplesBuffer &outBuffer, uint samplesToDecode);
    inline int getSampleRate() const { return vorbisDecoder.getSampleRate(); }
    inline bool isStereo() const { return vorbisDecoder.isStereo(); }
    void stopDecoding();
    bool isFullyDecoded() const { return vorbisDecoder.isFinished(); }
private:
    vorbis::Decoder vorbisDecoder;
    audio::SamplesBuffer decodedBuffer;
    QMutex mutex;
};

NinjamTrackNode::IntervalDecoder::IntervalDecoder(const QByteArray &vorbisData)
    :decodedBuffer(2)
{
    // this funcion is called from GUI thread



    QMutexLocker locker(&mutex);
    vorbisDecoder.setInputData(vorbisData);
}

void NinjamTrackNode::IntervalDecoder::addEncodedData(const QByteArray &vorbisData)
{
    // this funcion is called from GUI thread

    QMutexLocker locker(&mutex);
    vorbisDecoder.addInputData(vorbisData);
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
    //processingLastPartOfInterval(false),
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
    discardDownloadedIntervals();

    if (currentDecoder != nullptr)
        currentDecoder->stopDecoding();
}

NinjamTrackNode::LowCutState NinjamTrackNode::setLowCutToNextState()
{
    LowCutState newState = LowCutState::Off;

    switch (lowCut->getState() ) {

    case LowCutState::Off:
        newState = LowCutState::Normal;
        break;
    case LowCutState::Normal:
        newState = LowCutState::Drastic;
        break;
    case LowCutState::Drastic:
        newState = LowCutState::Off;
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
    qDebug() << "Deastrutor NinjamTrackNode";
    decodersMutex.lock();
    foreach (IntervalDecoder *decoder, decoders)
        delete decoder;

    decoders.clear();
    if (currentDecoder) {
        //delete currentDecoder;
        currentDecoder = nullptr;
    }
    decodersMutex.unlock();
}

void NinjamTrackNode::discardDownloadedIntervals()
{
    decodersMutex.lock();

    while (!decoders.isEmpty())
        delete decoders.takeFirst();

    currentDecoder = nullptr;

    qDebug() << "intervals discarded";
    decodersMutex.unlock();
}

bool NinjamTrackNode::isPlaying()
{
    QMutexLocker locker(&decodersMutex);
    return currentDecoder != nullptr || mode == VoiceChat; // voice chat is always playing
}

bool NinjamTrackNode::startNewInterval()
{
    qDebug() << "--------START INTERVAL------------";

    if (mode == Intervalic) {
        decodersMutex.lock();
        if (currentDecoder) {
            delete currentDecoder; //discard the previous interval decoder
            currentDecoder = nullptr;
        }
        if (!decoders.isEmpty())
            currentDecoder = decoders.takeFirst(); //using the next buffered decoder (next interval)

        decodersMutex.unlock();
    }

    return isPlaying();
}

// this function is used only for voice chat mode. The parameter is not a full Ogg Vorbis interval, it's just a chunk of data.
void NinjamTrackNode::addVorbisEncodedChunk(const QByteArray &chunkBytes, bool isLastPart)
{
    if(mode != VoiceChat)
        return;

    decodersMutex.lock();

    if (decoders.isEmpty()) {

        //if (currentDecoder)
            //delete currentDecoder;

        //qDebug() << "Decoders list is empty, creating new IntervalDecoder";
        //currentDecoder = new IntervalDecoder(); // processReplacing will consume this decoder without wait for startInterval

        decoders.push_back(new IntervalDecoder());
    }


    decoders.last()->addEncodedData(chunkBytes);

    if (isLastPart) {
        qDebug() << "Last part received, creating new IntervalDecoder";
        decoders.push_back(new IntervalDecoder());

    }

    decodersMutex.unlock();
}

 // this function is used only for Intervalic mode. The parameter is a full Ogg Vorbis Interval data
void NinjamTrackNode::addVorbisEncodedInterval(const QByteArray &fullIntervalBytes)
{
    if (mode != Intervalic)
        return;

    auto newIntervalDecoder = new IntervalDecoder(fullIntervalBytes);

    decodersMutex.lock();

    decoders.append(newIntervalDecoder);

    decodersMutex.unlock();

    //decoding the first samples in a separated thread to avoid slow down the audio thread in interval start (first beat)
    QtConcurrent::run(newIntervalDecoder, &NinjamTrackNode::IntervalDecoder::decode, 256);
}

// ++++++++++++++

void NinjamTrackNode::setChannelMode(ChannelMode newMode)
{
    if (newMode != this->mode) {
        this->mode = newMode;

        discardDownloadedIntervals();
    }
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

    { // mutex scope
        QMutexLocker locker(&decodersMutex);

        auto framesToProcess = getFramesToProcess(sampleRate, out.getFrameLenght());
        internalInputBuffer.setFrameLenght(framesToProcess);

        if (!currentDecoder) {
            if (mode == VoiceChat && !decoders.isEmpty()) {
                currentDecoder = decoders.first();
                qDebug() << "USING FIRST DECODER";
            }
            else {
                //qDebug() << "Current decoder is null, not playing!";
                return;
            }
        }

        auto samplesDecoded = 0;

        if (currentDecoder)
            currentDecoder->getDecodedSamples(internalInputBuffer, framesToProcess);

        if (mode == VoiceChat) { // in voice chat we will not wait until startInterval to use the next available downloaded decoder
            if (samplesDecoded <= framesToProcess && currentDecoder->isFullyDecoded()) {
                qDebug() << "current decoder consumed, using the next decoder";
                currentDecoder = nullptr;
                if (!decoders.isEmpty()) {
                    auto decoder = decoders.takeFirst();
                    delete decoder;
                }
            }
        }

    }

    if (!internalInputBuffer.isEmpty()) {
        if (needResamplingFor(sampleRate)) {
            const auto &resampledBuffer = resampler.resample(internalInputBuffer, out.getFrameLenght());
            internalInputBuffer.setFrameLenght(resampledBuffer.getFrameLenght());
            internalInputBuffer.set(resampledBuffer);
            if (internalInputBuffer.getFrameLenght() != out.getFrameLenght())
                qWarning() << internalInputBuffer.getFrameLenght() << " != " << out.getFrameLenght();
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
