#include "MetronomeUtils.h"

#include "audio/core/SamplesBuffer.h"
#include "audio/file/FileReaderFactory.h"
#include "audio/file/FileReader.h"
#include "audio/Resampler.h"
#include <QString>

void MetronomeUtils::createDefaultSounds(Audio::SamplesBuffer &firstBeatBuffer, Audio::SamplesBuffer &secondaryBeatBuffer, quint32 localSampleRate)
{
    QString audioFile = ":/click.wav";
    std::unique_ptr<Audio::FileReader> reader = Audio::FileReaderFactory::createFileReader(audioFile);
    quint32 audioFileSampleRate; //will be changed inside reader->read
    Audio::SamplesBuffer originalBuffer(2);
    reader->read(audioFile, originalBuffer, audioFileSampleRate);//buffer will be filled with audio file samples

    //need resample?
    if (audioFileSampleRate != (uint)localSampleRate) {
        createResampledBuffer(originalBuffer, secondaryBeatBuffer, audioFileSampleRate, localSampleRate);
    }
    else{
        if (originalBuffer.isMono())
            secondaryBeatBuffer.setToMono();
        secondaryBeatBuffer.setFrameLenght(originalBuffer.getFrameLenght());
        secondaryBeatBuffer.set(originalBuffer);
    }

    //create a resampled version of original buffer to make a high pitch sound for the first beat
    createResampledBuffer(secondaryBeatBuffer, firstBeatBuffer, localSampleRate, localSampleRate * 0.5);
}


void MetronomeUtils::createResampledBuffer(const Audio::SamplesBuffer &buffer, Audio::SamplesBuffer &outBuffer, int originalSampleRate,
                                     int finalSampleRate)
{
    int finalSize = (double)finalSampleRate/originalSampleRate * buffer.getFrameLenght();
    int channels = buffer.getChannels();
    if (channels > 1)
        outBuffer.setToStereo();
    outBuffer.setFrameLenght(finalSize);

    for (int c = 0; c < channels; ++c) {
        SimpleResampler resampler;
        resampler.process(buffer.getSamplesArray(c),
                          buffer.getFrameLenght(), outBuffer.getSamplesArray(c), finalSize);
    }
}
