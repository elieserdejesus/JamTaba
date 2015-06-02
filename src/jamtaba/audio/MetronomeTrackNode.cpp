#include "MetronomeTrackNode.h"
#include <QDebug>
#include "../audio/core/AudioDriver.h"
#include <QFile>
#include <cmath>
#include <QtEndian>

using namespace Audio;

MetronomeTrackNode::MetronomeTrackNode(QString metronomeWaveFile)

{
    clickSoundBuffer = readWavFile(metronomeWaveFile);
    clickBufferOffset = 0;
}

MetronomeTrackNode::~MetronomeTrackNode()
{
    delete clickSoundBuffer;
}

void MetronomeTrackNode::processReplacing(SamplesBuffer &in, SamplesBuffer &out){
    static int sampleIndex = 0;
    //copiar as amostras internas para o internal buffer
    internalBuffer->setFrameLenght(out.getFrameLenght());
    int samplesToCopy = std::min( clickSoundBuffer->getFrameLenght() - clickBufferOffset, (unsigned int)out.getFrameLenght());
    if(samplesToCopy > 0){
        internalBuffer->set(*clickSoundBuffer, clickBufferOffset, samplesToCopy, 0);
        clickBufferOffset += samplesToCopy;
        //qDebug() << "offset: " << clickBufferOffset << endl;
        AudioNode::processReplacing(in, out);
    }
    sampleIndex += out.getFrameLenght();
    if(sampleIndex >= 44100){
        sampleIndex = clickBufferOffset = 0;
    }
}

SamplesBuffer *MetronomeTrackNode::readWavFile(QString fileName){
    // Open wave file
    qDebug() << "Opening WAV file at: " << fileName;
    QFile wavFile(fileName);
    if (!wavFile.open(QFile::ReadOnly))
    {
        qWarning() << "Failed to open WAV file...";
        return nullptr; // Done
    }

    // Read in the whole thing
    QByteArray wavFileContent = wavFile.readAll();
    qDebug() << "The size of the WAV file is: " << wavFileContent.size();

    // Define the header components
    quint8 fileType[4];
    quint32 fileSize;
    quint8 waveName[4];
    quint8 fmtName[3];
    quint32 fmtLength;
    quint16 fmtType;
    quint16 numberOfChannels;
    quint32 sampleRate;
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

    // Now pull out the data
    //return analyzeHeaderDS.readRawData((char*)ramBuffer,(int)dataSize);
    //stream.setByteOrder(QDataStream::LittleEndian );
    qint16 sample = 0;
    for (int s = 0; s < samples; ++s) {
        for (int c = 0; c < numberOfChannels; ++c) {
            stream >> sample;
            buffer->set(c, s, sample / 32.767f);
            //qDebug() << sample << endl;
        }
    }

    return buffer;

}

