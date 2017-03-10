#include "WaveFileReader.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <cmath>
#include <QtEndian>
#include <QDataStream>
#include <memory>

using namespace Audio;

class SampleExtractor
{
public:
    SampleExtractor(QDataStream *stream)
        :stream(stream)
    {
    }
    
    virtual ~SampleExtractor()
    {
        
    }

    virtual float nextSample() = 0;
protected:
    QDataStream *stream;
};

class SampleExtractor16Bits : public SampleExtractor
{
    public:

        SampleExtractor16Bits(QDataStream *stream)
            :SampleExtractor(stream)
        {

        }

        float nextSample() override
        {
            qint16 sampleValue;
            *stream >> sampleValue;
            return sampleValue / 32767.0f;
        }
};

class SampleExtractor24Bits : public SampleExtractor
{
    public:
        SampleExtractor24Bits(QDataStream *stream)
            :SampleExtractor(stream)
        {

        }

        float nextSample() override
        {

            char buffer[3];
            stream->readRawData(buffer, 3);
            qint32 sampleValue = (qint32)((buffer[0] & 0xFF) | (buffer[1] << 8) | (buffer[2] << 16));
            return sampleValue / 8388607.0; // 8388607 = max value in 24 bits
        }
};

class SampleExtractor8Bits : public SampleExtractor
{
    public:
        SampleExtractor8Bits(QDataStream *stream)
            :SampleExtractor(stream)
        {

        }

        float nextSample() override
        {
            qint8 byte;
            *stream >> byte;
            return byte / 127.0;
        }
};

class NullSampleExtractor : public SampleExtractor
{
    public:
        NullSampleExtractor()
            :SampleExtractor(nullptr)
        {
        }

        float nextSample() override
        {
            return 0.0;
        }
};

class SampleExtractorFactory
{
public:
    static std::unique_ptr<SampleExtractor> createExtractor(QDataStream *stream, quint8 bitsPerSample)
    {
        switch (bitsPerSample/8) {
            case 1: return std::unique_ptr<SampleExtractor8Bits>(new SampleExtractor8Bits(stream));
            case 2: return std::unique_ptr<SampleExtractor16Bits>(new SampleExtractor16Bits(stream));
            //case 3: return std::make_unique<SampleExtractor24Bits>(stream);
            //case 4: return std::make_unique<SampleExtractor32Bits>(stream);
        }
        qCritical() << "Can't create a SampleExtractor to handle " << bitsPerSample << " bits per sample!";
        return std::unique_ptr<NullSampleExtractor>(new NullSampleExtractor());
    }
};

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
    std::unique_ptr<SampleExtractor> sampleExtractor = SampleExtractorFactory::createExtractor(&stream, bitsPerSample);
    for (int s = 0; s < samples; ++s) {
        for (int c = 0; c < channels; ++c) {
            float sample = sampleExtractor->nextSample();
            outBuffer.set(c, s, sample);
        }
    }
}
