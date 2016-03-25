#include "WaveFileReader.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <cmath>
#include <QtEndian>
#include <QDataStream>

using namespace Audio;

void WaveFileReader::read(const QString &filePath, Audio::SamplesBuffer &outBuffer, quint32 &sampleRate)
{
    // Open the wave file
    QFile wavFile(filePath);
    if (!wavFile.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open WAV file ..." << filePath;
        return; // Done, out buffer is not changed
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
    quint16 channels;
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
    stream >> channels; // Number of channels
    stream >> sampleRate; // Sample rate
    stream >> sampleRateXBitsPerSampleXChanngelsDivEight; // (Sample Rate * BitsPerSample * Channels) / 8
    stream >> bitsPerSampleXChannelsDivEightPointOne; // (BitsPerSample * Channels) / 8.1
    stream >> bitsPerSample; // Bits per sample
    stream.readRawData((char *)dataHeader, 4); // "data" header
    stream >> dataSize; // Data Size

    int samples = dataSize / channels / (bitsPerSample/ 8);

    if (channels == 1)
        outBuffer.setToMono();
    else
        outBuffer.setToStereo();
    outBuffer.setFrameLenght(samples);

    // Now pull out the data
    qint16 sample = 0;
    for (int s = 0; s < samples; ++s) {
        for (int c = 0; c < channels; ++c) {
            stream >> sample;
            outBuffer.set(c, s, sample / 32767.0f);
        }
    }
}
