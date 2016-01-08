#ifndef SAMPLESBUFFERRECORDER_H
#define SAMPLESBUFFERRECORDER_H

#include "core/SamplesBuffer.h"
#include <QString>
#include <QByteArray>

class SamplesBufferRecorder
{
public:
    SamplesBufferRecorder(const QString &fileName, quint32 sampleRate);
    ~SamplesBufferRecorder();
    void addSamples(const Audio::SamplesBuffer &buffer);
private:
    void writeSamplesToFile();
    void writeWavHeader();
    QByteArray array;
    QString fileName;
    quint32 sampleRate;
};

#endif // SAMPLESBUFFERRECORDER_H
