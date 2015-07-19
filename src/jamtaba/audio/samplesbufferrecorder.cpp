#include "samplesbufferrecorder.h"

#include <QDataStream>
#include "core/SamplesBuffer.h"
#include <QByteArray>
#include <QFile>
#include <QDir>

SamplesBufferRecorder::SamplesBufferRecorder(QString fileName, quint32 sampleRate)
    :fileName(fileName), sampleRate(sampleRate){
    char header[44];
    array.appendDataToUpload(header, 44);//create empty space in first 44 bytes to write header in desctructor
}

SamplesBufferRecorder::~SamplesBufferRecorder(){
    writeSamplesToFile();
}

void SamplesBufferRecorder::addSamples(const Audio::SamplesBuffer &buffer){
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.skipRawData(array.size());
    for (int s = 0; s < buffer.getFrameLenght(); ++s) {
        quint16 sampleVale = (quint16)(buffer.get(0, s) * 32767);
        stream << sampleVale;
    }
}

void SamplesBufferRecorder::writeWavHeader(){
    quint32 frameLenght = (quint32)(array.size() - 44);
    quint32 chunckSize = (quint32)16;
    quint16 audioFormat = (quint16)1;//PCM
    quint16 channels = (quint16)1;
    quint32 bytesPerSecond = (quint32)(sampleRate * 2);
    quint16 bytesPerSample = (quint16)2;
    quint16 bitDepth = (quint16)16;

    QDataStream stream(&array, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream.writeRawData("RIFF", 4);                                 // 00 - RIFF
    stream << frameLenght;      // 04 - how big is the rest of this file?
    stream.writeRawData("WAVE", 4);                                 // 08 - WAVE
    stream.writeRawData("fmt ", 4);                                 // 12 - fmt
    stream << chunckSize;  // 16 - size of this chunk
    stream << audioFormat;     // 20 - what is the audio format? 1 for PCM = Pulse Code Modulation
    stream << channels;   // 22 - mono or stereo? 1 or 2?  (or 5 or ???)
    stream << sampleRate;     // 24 - samples per second (numbers per second)
    stream << bytesPerSecond;       // 28 - bytes per second
    stream << bytesPerSample; // 32 - # of bytes in one sample, for all channels
    stream << bitDepth;  // 34 - how many bits in a sample(number)?  usually 16 or 24
    stream.writeRawData("data", 4);                                 // 36 - data
    stream << frameLenght;       // 40 - how big is this data chunk
}

void SamplesBufferRecorder::writeSamplesToFile(){

    writeWavHeader();
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.write(array.data(), array.size());
    file.close();

}
