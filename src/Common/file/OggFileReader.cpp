#include "OggFileReader.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include "audio/vorbis/VorbisDecoder.h"

using namespace Audio;

void OggFileReader::read(const QString &filePath, Audio::SamplesBuffer &outBuffer, quint32 &sampleRate)
{
    // Open the ogg file
    QFile oggFile(filePath);
    if (!oggFile.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open OGG file ..." << filePath;
        return;
    }

    VorbisDecoder decoder;
    decoder.setInputData(oggFile.readAll());
    decoder.initialize(); //read the ogg headers from file
    sampleRate = decoder.getSampleRate();
    if (decoder.isMono())
        outBuffer.setToMono();
    else
        outBuffer.setToStereo();

    //decode and append decoded data in 'outBuffer'
    int decodedFrames = 0;
    const int MAX_SAMPLES_PER_DECODE = 1024;
    do
    {
        const Audio::SamplesBuffer &decodedBuffer = decoder.decode(MAX_SAMPLES_PER_DECODE);
        decodedFrames = decodedBuffer.getFrameLenght();
        if (!decodedBuffer.isEmpty()) {
            outBuffer.append(decodedBuffer);
        }
    }
    while(decodedFrames > 0);

}
