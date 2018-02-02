#include "OggFileReader.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include "audio/vorbis/VorbisDecoder.h"

using audio::OggFileReader;
using audio::SamplesBuffer;

bool OggFileReader::read(const QString &filePath, SamplesBuffer &outBuffer, quint32 &sampleRate)
{
    // Open the ogg file
    QFile oggFile(filePath);
    if (!oggFile.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open OGG file ..." << filePath;
        return false;
    }

    vorbis::Decoder decoder;
    decoder.setInputData(oggFile.readAll());
    decoder.initialize(); // read the ogg headers from file
    sampleRate = decoder.getSampleRate();
    if (decoder.isMono())
        outBuffer.setToMono();
    else
        outBuffer.setToStereo();

    // decode and append decoded data in 'outBuffer'
    int decodedFrames = 0;
    const int MAX_SAMPLES_PER_DECODE = 1024;
    do
    {
        const auto &decodedBuffer = decoder.decode(MAX_SAMPLES_PER_DECODE);
        decodedFrames = decodedBuffer.getFrameLenght();
        if (!decodedBuffer.isEmpty()) {
            outBuffer.append(decodedBuffer);
        }
    }
    while(decodedFrames > 0);

    return true;
}
