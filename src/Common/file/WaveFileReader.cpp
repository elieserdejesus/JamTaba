#include "WaveFileReader.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <cmath>
#include <QtEndian>
#include <QDataStream>
#include <memory>

using audio::SamplesBuffer;
using audio::WaveFileReader;

class SampleExtractor
{

public:
    explicit SampleExtractor(QDataStream *stream) :
        stream(stream)
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

        explicit SampleExtractor16Bits(QDataStream *stream)
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

class SampleExtractor32Bits : public SampleExtractor
{
    public:
        explicit SampleExtractor32Bits(QDataStream *stream)
            :SampleExtractor(stream)
        {

        }

        float nextSample() override
        {
            char buffer[4];
            stream->readRawData(buffer, 4);

            float sample = 0;
            memcpy(&sample, &buffer, sizeof(sample));
            return sample;
        }
};

class SampleExtractor24Bits : public SampleExtractor
{
    public:
        explicit SampleExtractor24Bits(QDataStream *stream)
            :SampleExtractor(stream)
        {

        }

        float nextSample() override
        {
            char buffer[3];
            stream->readRawData(buffer, 3);
            return ((buffer[0] & 0xFF) | ((buffer[1] & 0xFF) << 8) | (buffer[2] << 16)) / 8388606.0F;
        }
};

class SampleExtractor8Bits : public SampleExtractor
{
    public:
        explicit SampleExtractor8Bits(QDataStream *stream)
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
            case 3: return std::unique_ptr<SampleExtractor24Bits>(new SampleExtractor24Bits(stream));
            case 4: return std::unique_ptr<SampleExtractor32Bits>(new SampleExtractor32Bits(stream));
        }
        qCritical() << "Can't create a SampleExtractor to handle " << bitsPerSample << " bits per sample!";
        return std::unique_ptr<NullSampleExtractor>(new NullSampleExtractor());
    }
};

bool WaveFileReader::read(const QString &filePath, audio::SamplesBuffer &outBuffer, quint32 &sampleRate)
{
    // Open the wave file
    QFile wavFile(filePath);
    if (!wavFile.open(QFile::ReadOnly)) {
        qCritical() << "Failed to open WAV file ..." << filePath;
        return false; // Done, out buffer is not changed
    }

    // Read in the whole thing
    QByteArray wavFileContent = wavFile.readAll();

    // Define the header components
    char fileType[4];
    quint32 fileSize;
    char waveName[4];
    char fmtName[4];
    quint32 fmtLength;
    quint16 fmtType;
    quint16 channels;
    quint32 sampleRateXBitsPerSampleXChanngelsDivEight;
    quint16 bitsPerSampleXChannelsDivEightPointOne;
    quint16 bitsPerSample;
    char dataHeader[4];
    quint32 dataSize = 0;

    // Create a data stream to analyze the data
    QDataStream stream(&wavFileContent, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    // Now pop off the appropriate data into each header field defined above
    stream.readRawData((char *)fileType, 4); // "RIFF"
    if (QString::fromLocal8Bit(fileType, 4) != "RIFF") {
        qCritical() << "Error loading " << filePath << ", 'RIFF' chunk not founded!";
        return false;
    }

    stream >> fileSize; // File Size

    stream.readRawData((char *)waveName, 4); // "WAVE"
    if (QString::fromLocal8Bit(waveName, 4) != "WAVE") {
        qCritical() << "Error loading " << filePath << ", 'WAVE' chunk not founded!";
        return false;
    }

    stream.readRawData((char *)fmtName, 4); // "fmt"
    if (QString::fromLocal8Bit(fmtName, 4) != "fmt ") {
        qCritical() << "Error loading " << filePath << ", 'fmt' chunk not founded!";
        return false;
    }

    stream >> fmtLength;  // Format length
    stream >> fmtType;    // Format type
    stream >> channels;   // Number of channels
    stream >> sampleRate; // Sample rate
    stream >> sampleRateXBitsPerSampleXChanngelsDivEight; // (Sample Rate * BitsPerSample * Channels) / 8
    stream >> bitsPerSampleXChannelsDivEightPointOne; // (BitsPerSample * Channels) / 8.1
    stream >> bitsPerSample; // Bits per sample
    while (QString::fromLocal8Bit(dataHeader, 4) != "data" && !stream.atEnd()) {
        stream.readRawData((char *)dataHeader, 4); // "data" header
        stream >> dataSize; // Data Size
        if (QString::fromLocal8Bit(dataHeader, 4) != "data")
            stream.skipRawData(dataSize);
    }

    uint samples = dataSize / channels / (bitsPerSample/ 8);

    if (channels == 1)
        outBuffer.setToMono();
    else
        outBuffer.setToStereo();

    if (outBuffer.getFrameLenght() > 0) // load only outBuffer.frameLenght samples?
        samples = qMin(samples, outBuffer.getFrameLenght());

    outBuffer.setFrameLenght(samples);

    // Now pull out the data
    std::unique_ptr<SampleExtractor> sampleExtractor = SampleExtractorFactory::createExtractor(&stream, bitsPerSample);
    for (uint s = 0; s < samples; ++s) {
        for (int c = 0; c < channels; ++c) {
            float sample = sampleExtractor->nextSample();
            outBuffer.set(c, s, sample);
        }
    }

    return true;
}
