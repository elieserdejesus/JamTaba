#include "WaveFileWriter.h"

#include <QFile>
#include <QDebug>
#include <QDataStream>
#include <QThread>
#include <climits>

using audio::WaveFileWriter;
using audio::SamplesBuffer;

void WaveFileWriter::write(const QString &filePath, const SamplesBuffer &buffer, quint32 sampleRate, quint8 bitDepth)
{
    QFile wavFile(filePath);
    if (!wavFile.open(QFile::WriteOnly)) {
        qCritical() << "Failed to create WAV file ..." << filePath;
        return;
    }

    const uint samples = buffer.getFrameLenght();
    const uint dataChunkSize = buffer.getChannels() * buffer.getFrameLenght() * bitDepth/8;// bytes per sample
    const uint fileSize = dataChunkSize + 44; // WAVE HEADER is 44 bytes

    QDataStream out(&wavFile);
    out.setByteOrder(QDataStream::LittleEndian);

    // RIFF chunk
    out.writeRawData("RIFF", 4);
    out << quint32(fileSize - 8); // Placeholder for the RIFF chunk size (filled by close())
    out.writeRawData("WAVE", 4);

    const quint8 sampleSize = bitDepth;
    // Format description chunk
    out.writeRawData("fmt ", 4);
    out << quint32(16); // "fmt " chunk size (always 16 for PCM)
    out << quint16(bitDepth == 16 ? 1 : 3); // data format (1 => PCM, 3 => IEEE float) http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
    out << quint16(buffer.getChannels());
    out << quint32(sampleRate);
    out << quint32(sampleRate * buffer.getChannels() * sampleSize / 8 ); // bytes per second
    out << quint16(buffer.getChannels() * sampleSize / 8); // Block align
    out << quint16(sampleSize); // Significant Bits Per Sample

    // Data chunk
    out.writeRawData("data", 4);
    out << quint32(dataChunkSize); // Placeholder for the data chunk size (filled by close())

    //write interleaved samples
    if (bitDepth == 32)
        out.setFloatingPointPrecision(QDataStream::SinglePrecision);

    uint channels = buffer.getChannels();
    for (uint s = 0; s < samples; ++s) {
        for (uint c = 0; c < channels; ++c) {
            if (bitDepth == 16) {
                int sample = buffer.get(c, s) * SHRT_MAX;
                // hard clip
                if (sample > SHRT_MAX)
                    sample = SHRT_MAX;
                else if (sample < SHRT_MIN)
                    sample = SHRT_MIN;

                out << quint16(sample);
            }
            else { // 32 bits
                out << buffer.get(c, s);
            }
        }
    }

}
