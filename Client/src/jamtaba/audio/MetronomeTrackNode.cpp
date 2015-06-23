#include "MetronomeTrackNode.h"
#include <QDebug>
#include "../audio/core/AudioDriver.h"
#include "../audio/core/SamplesBuffer.h"
#include <QFile>
#include <cmath>
#include <QtEndian>
#include "Resampler.h"
#include <QMutexLocker>

using namespace Audio;

SamplesBuffer* createResampledBuffer(const SamplesBuffer& buffer, int originalSampleRate, int finalSampleRate){
    int finalSize = (double)finalSampleRate/originalSampleRate * buffer.getFrameLenght();
    int channels = buffer.getChannels();
    SamplesBuffer* newBuffer = new SamplesBuffer(channels, finalSize);
    float** in = buffer.getSamplesArray();
    float** out = newBuffer->getSamplesArray();
    for (int c = 0; c < channels; ++c) {
        Resampler resampler(originalSampleRate, finalSampleRate);
        resampler.process(in[c], buffer.getFrameLenght(), out[c], finalSize);
    }
    //qDebug() << "Criou buffer com resampling: bufferAntigo: " <<buffer.getFrameLenght() << " novo:" << newBuffer->getFrameLenght() << endl;
    return newBuffer;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MetronomeTrackNode::MetronomeTrackNode(QString metronomeWaveFile, int localSampleRate)
    : samplesPerBeat(0), intervalPosition(0), beatPosition(0), currentBeat(0), beatsPerAccent(0){
    quint32 waveFileSampleRate;//this value will be changed by readWavFile method
    clickSoundBuffer = readWavFile(metronomeWaveFile, waveFileSampleRate);
    if(waveFileSampleRate != (uint)localSampleRate){
        SamplesBuffer* temp = clickSoundBuffer;
        clickSoundBuffer = createResampledBuffer(*clickSoundBuffer, waveFileSampleRate, localSampleRate);
        delete temp;
    }

    firstIntervalBeatBuffer = createResampledBuffer(*clickSoundBuffer, localSampleRate, localSampleRate * 0.5);
    firstMeasureBeatBuffer = createResampledBuffer(*clickSoundBuffer, localSampleRate, localSampleRate * 0.75);

    reset();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MetronomeTrackNode::~MetronomeTrackNode(){
    delete clickSoundBuffer;
    delete firstIntervalBeatBuffer;
    delete firstMeasureBeatBuffer;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::setBeatsPerAccent(int beatsPerAccent){
    QMutexLocker locker(&mutex);
    this->beatsPerAccent = beatsPerAccent;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::setSamplesPerBeat(long samplesPerBeat){
    if(samplesPerBeat <= 0){
        qCritical() << "samples per beat <= 0";
    }
    this->samplesPerBeat = samplesPerBeat;
    //clickSoundBuffer->setFrameLenght(samplesPerBeat);
    reset();
    //qDebug() << "samples per beat: " << this->samplesPerBeat << endl;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::reset(){
    beatPosition = intervalPosition = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::setIntervalPosition(long intervalPosition){
    if(samplesPerBeat <= 0){
        return;
    }
    this->intervalPosition = intervalPosition;
    this->beatPosition = intervalPosition % samplesPerBeat;
    this->currentBeat = (intervalPosition / samplesPerBeat);// + 1;
    //qDebug() << "beat: " << currentBeat;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SamplesBuffer* MetronomeTrackNode::getBuffer(int beat){
    if(beat == 0){
        return firstIntervalBeatBuffer;
    }else{
        if(isPlayingAccents() && beat % beatsPerAccent == 0){
            return firstMeasureBeatBuffer;
        }
    }
    return clickSoundBuffer;
}

void MetronomeTrackNode::processReplacing(SamplesBuffer &in, SamplesBuffer &out){
    if(samplesPerBeat <= 0){
        return;
    }
    internalBuffer->setFrameLenght(out.getFrameLenght());
    internalBuffer->zero();

    SamplesBuffer* samplesBuffer = getBuffer(currentBeat);
    int samplesToCopy = std::min( (int)(samplesBuffer->getFrameLenght() - beatPosition), out.getFrameLenght());
    int nextBeatSample = beatPosition + out.getFrameLenght();
    int outOffset = 0;
    int clickSoundBufferOffset = beatPosition;
    if(nextBeatSample > samplesPerBeat){
        samplesBuffer = getBuffer(currentBeat + 1);//iniciar o próximo beat nesse callback
        outOffset = samplesPerBeat - beatPosition;
        samplesToCopy = std::min( nextBeatSample - samplesPerBeat, (long)samplesBuffer->getFrameLenght());
        clickSoundBufferOffset = 0;

    }
    if(samplesToCopy > 0){
        internalBuffer->set(*samplesBuffer, clickSoundBufferOffset, samplesToCopy, outOffset);
        AudioNode::processReplacing(in, out);
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SamplesBuffer *MetronomeTrackNode::readWavFile(QString fileName, quint32 & sampleRate){
    // Open wave file
    //qDebug() << "Opening WAV file at: " << fileName;
    QFile wavFile(fileName);
    if (!wavFile.open(QFile::ReadOnly))
    {
        qWarning() << "Failed to open WAV file...";
        return nullptr; // Done
    }

    // Read in the whole thing
    QByteArray wavFileContent = wavFile.readAll();
    //qDebug() << "The size of the WAV file is: " << wavFileContent.size();

    // Define the header components
    quint8 fileType[4];
    quint32 fileSize;
    quint8 waveName[4];
    quint8 fmtName[3];
    quint32 fmtLength;
    quint16 fmtType;
    quint16 numberOfChannels;
    //quint32 sampleRate;
    quint32 sampleRateXBitsPerSampleXChanngelsDivEight;
    quint16 bitsPerSampleXChannelsDivEightPointOne;
    quint16 bitsPerSample;
    quint8 dataHeader[4];
    quint32 dataSize;

    // Create a data stream to analyze the data
    QDataStream stream(&wavFileContent,QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    // Now pop off the appropriate data into each header field defined above
    stream.readRawData((char*)fileType,4); // "RIFF"
    stream >> fileSize; // File Size
    stream.readRawData((char*)waveName,4); // "WAVE"
    stream.readRawData((char*)fmtName,4); // "fmt"
    stream >> fmtLength; // Format length
    stream >> fmtType; // Format type
    stream >> numberOfChannels; // Number of channels
    stream >> sampleRate; // Sample rate
    stream >> sampleRateXBitsPerSampleXChanngelsDivEight; // (Sample Rate * BitsPerSample * Channels) / 8
    stream >> bitsPerSampleXChannelsDivEightPointOne; // (BitsPerSample * Channels) / 8.1
    stream >> bitsPerSample; // Bits per sample
    stream.readRawData((char*)dataHeader,4); // "data" header
    stream >> dataSize; // Data Size

    //fileSize = qToBigEndian(fileSize);
    //dataSize = qToBigEndian(dataSize);
    //numberOfChannels = qToBigEndian(numberOfChannels);
    //bitsPerSample = qToBigEndian(bitsPerSample );


    int samples = dataSize / numberOfChannels / (bitsPerSample/ 8);
    SamplesBuffer* buffer = new SamplesBuffer(numberOfChannels, samples);
/*
    qDebug() << "File Type: " << QString::fromUtf8((char*)fileType, 4);
    qDebug() << "File Size: " << fileSize;
    qDebug() << "WAV Marker: " << QString::fromUtf8((char*)waveName, 4);
    qDebug() << "Format Name: " << QString::fromUtf8((char*)fmtName, 4);
    qDebug() << "Format Length: " << fmtLength;
    qDebug() << "Format Type: " << fmtType;
    qDebug() << "Number of Channels: " << numberOfChannels;
    qDebug() << "Sample Rate: " << sampleRate;
    qDebug() << "Sample Rate * Bits/Sample * Channels / 8: " << sampleRateXBitsPerSampleXChanngelsDivEight;
    qDebug() << "Bits per Sample * Channels / 8.1: " << bitsPerSampleXChannelsDivEightPointOne;
    qDebug() << "Bits per Sample: " << bitsPerSample;
    qDebug() << "Data Header: " << QString::fromUtf8((char*)dataHeader, 4);
    qDebug() << "Data Size: " << dataSize;
    */

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
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
