#include "MetronomeTrackNode.h"
#include <QDebug>
#include "audio/core/AudioDriver.h"
#include "audio/core/SamplesBuffer.h"
#include "audio/SamplesBufferResampler.h"
#include <QFile>
#include <cmath>
#include <QtEndian>
#include "Resampler.h"
#include <QDataStream>
#include <QMutexLocker>

using namespace Audio;

SamplesBuffer *createResampledBuffer(const SamplesBuffer &buffer, int originalSampleRate,
                                     int finalSampleRate)
{
    int finalSize = (double)finalSampleRate/originalSampleRate * buffer.getFrameLenght();
    int channels = buffer.getChannels();
    SamplesBuffer *newBuffer = new SamplesBuffer(channels, finalSize);
    for (int c = 0; c < channels; ++c) {
        ResamplerTest resampler;
        resampler.process(buffer.getSamplesArray(c),
                          buffer.getFrameLenght(), newBuffer->getSamplesArray(c), finalSize);
    }
    return newBuffer;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MetronomeTrackNode::MetronomeTrackNode(const QString &metronomeWaveFile, int localSampleRate) :
    samplesPerBeat(0),
    intervalPosition(0),
    beatPosition(0),
    currentBeat(0),
    beatsPerAccent(0)
{
    clickSoundBuffer = readWavFile(metronomeWaveFile, this->waveFileSampleRate);// the last param value will be changed by readWavFile method
    if (waveFileSampleRate != (uint)localSampleRate) {
        SamplesBuffer *temp = clickSoundBuffer;
        clickSoundBuffer = createResampledBuffer(*clickSoundBuffer, waveFileSampleRate,
                                                 localSampleRate);
        delete temp;
    }

    firstIntervalBeatBuffer = createResampledBuffer(*clickSoundBuffer, localSampleRate,
                                                    localSampleRate * 0.5);
    firstMeasureBeatBuffer = createResampledBuffer(*clickSoundBuffer, localSampleRate,
                                                   localSampleRate * 0.75);

    resetInterval();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MetronomeTrackNode::~MetronomeTrackNode()
{
    delete clickSoundBuffer;
    delete firstIntervalBeatBuffer;
    delete firstMeasureBeatBuffer;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::setBeatsPerAccent(int beatsPerAccent)
{
    this->beatsPerAccent = beatsPerAccent;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::setSamplesPerBeat(long samplesPerBeat)
{
    if (samplesPerBeat <= 0)
        qCritical() << "samples per beat <= 0";
    this->samplesPerBeat = samplesPerBeat;
    resetInterval();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::resetInterval()
{
    beatPosition = intervalPosition = 0;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::setIntervalPosition(long intervalPosition)
{
    if (samplesPerBeat <= 0)
        return;
    this->intervalPosition = intervalPosition;
    this->beatPosition = intervalPosition % samplesPerBeat;
    this->currentBeat = (intervalPosition / samplesPerBeat);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SamplesBuffer *MetronomeTrackNode::getBuffer(int beat)
{
    if (beat == 0) {
        return firstIntervalBeatBuffer;
    } else {
        if (isPlayingAccents() && beat % beatsPerAccent == 0)
            return firstMeasureBeatBuffer;
    }
    return clickSoundBuffer;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out,
                                          int SampleRate, const Midi::MidiMessageBuffer &midiBuffer)
{
    if (samplesPerBeat <= 0)
        return;
    internalInputBuffer.setFrameLenght(out.getFrameLenght());
    internalInputBuffer.zero();

    SamplesBuffer *samplesBuffer = getBuffer(currentBeat);
    int samplesToCopy = std::min(
        (int)(samplesBuffer->getFrameLenght() - beatPosition), out.getFrameLenght());
    int nextBeatSample = beatPosition + out.getFrameLenght();
    int internalOffset = 0;
    int clickSoundBufferOffset = beatPosition;
    if (nextBeatSample > samplesPerBeat) {// next beat starting in this audio buffer?
        samplesBuffer = getBuffer(currentBeat + 1);
        internalOffset = samplesPerBeat - beatPosition;
        samplesToCopy = std::min(nextBeatSample - samplesPerBeat,
                                 (long)samplesBuffer->getFrameLenght());
        clickSoundBufferOffset = 0;
    }
    if (samplesToCopy > 0)
        internalInputBuffer.set(*samplesBuffer, clickSoundBufferOffset, samplesToCopy,
                                internalOffset);
    AudioNode::processReplacing(in, out, SampleRate, midiBuffer);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SamplesBuffer *MetronomeTrackNode::readWavFile(const QString &fileName, quint32 &sampleRate)
{
    // Open wave file
    QFile wavFile(fileName);
    if (!wavFile.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open WAV file...";
        return nullptr; // Done
    }

    // Read in the whole thing
    QByteArray wavFileContent = wavFile.readAll();

    // Define the header components
    quint8 fileType[4];
    quint32 fileSize;
    quint8 waveName[4];
    quint8 fmtName[3];
    quint32 fmtLength;
    quint16 fmtType;
    quint16 numberOfChannels;
    quint32 sampleRateXBitsPerSampleXChanngelsDivEight;
    quint16 bitsPerSampleXChannelsDivEightPointOne;
    quint16 bitsPerSample;
    quint8 dataHeader[4];
    quint32 dataSize;

    // Create a data stream to analyze the data
    QDataStream stream(&wavFileContent, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    // Now pop off the appropriate data into each header field defined above
    stream.readRawData((char *)fileType, 4); // "RIFF"
    stream >> fileSize; // File Size
    stream.readRawData((char *)waveName, 4); // "WAVE"
    stream.readRawData((char *)fmtName, 4); // "fmt"
    stream >> fmtLength; // Format length
    stream >> fmtType; // Format type
    stream >> numberOfChannels; // Number of channels
    stream >> sampleRate; // Sample rate
    stream >> sampleRateXBitsPerSampleXChanngelsDivEight; // (Sample Rate * BitsPerSample * Channels) / 8
    stream >> bitsPerSampleXChannelsDivEightPointOne; // (BitsPerSample * Channels) / 8.1
    stream >> bitsPerSample; // Bits per sample
    stream.readRawData((char *)dataHeader, 4); // "data" header
    stream >> dataSize; // Data Size

    int samples = dataSize / numberOfChannels / (bitsPerSample/ 8);
    SamplesBuffer *buffer = new SamplesBuffer(numberOfChannels, samples);

    // Now pull out the data
    qint16 sample = 0;
    for (int s = 0; s < samples; ++s) {
        for (int c = 0; c < numberOfChannels; ++c) {
            stream >> sample;
            buffer->set(c, s, sample / 32767.0f);
        }
    }
    return buffer;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
