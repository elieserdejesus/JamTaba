#include "Mp3FileReader.h"
#include "audio/Mp3Decoder.h"

#include <QFile>
#include <QDebug>

using audio::Mp3FileReader;
using audio::SamplesBuffer;

bool Mp3FileReader::read(const QString &filePath, SamplesBuffer &outBuffer, quint32 &sampleRate)
{
    // Open the mp3 file
    QFile audioFile(filePath);
    if (!audioFile.open(QFile::ReadOnly)) {
        qCritical() << "Failed to open mp3 file ..." << filePath;
        return false;
    }

    audio::Mp3DecoderMiniMp3 decoder;
    QByteArray encodedData = audioFile.readAll();

    const quint64 totalBytesToProcess = encodedData.size();
    uint bytesProcessed = 0;
    const static int MAX_BYTES_PER_DECODING = 2048;         // chunks maxsize is 2048 bytes

    SamplesBuffer bufferedSamples(2);

    while (bytesProcessed < totalBytesToProcess) { // split in chunks to avoid a very large decoded buffer
        int bytesToProcess = std::min((int)(totalBytesToProcess - bytesProcessed), MAX_BYTES_PER_DECODING);
        const auto &decodedBuffer = decoder.decode(encodedData.data() + bytesProcessed, bytesToProcess);
        // +++++++++++++++++  PROCESS DECODED SAMPLES ++++++++++++++++
        bufferedSamples.append(decodedBuffer);
        bytesProcessed += bytesToProcess;
    }

    sampleRate = decoder.getSampleRate();
    outBuffer.setFrameLenght(bufferedSamples.getFrameLenght());
    outBuffer.set(bufferedSamples);

    return true;
}
