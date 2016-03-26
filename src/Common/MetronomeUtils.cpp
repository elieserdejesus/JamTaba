#include "MetronomeUtils.h"

#include "audio/core/SamplesBuffer.h"
#include "audio/file/FileReaderFactory.h"
#include "audio/file/FileReader.h"
#include "audio/Resampler.h"
#include <QString>
#include <QFileInfo>

const QString MetronomeUtils::DEFAULT_METRONOME_AUDIO_FILE(":/click.wav");

void MetronomeUtils::createDefaultSounds(Audio::SamplesBuffer &firstBeatBuffer, Audio::SamplesBuffer &secondaryBeatBuffer, quint32 localSampleRate)
{
    createBuffer(DEFAULT_METRONOME_AUDIO_FILE, secondaryBeatBuffer, localSampleRate);

    //create a resampled version of original buffer to make a high pitch sound for the first beat
    createResampledBuffer(secondaryBeatBuffer, firstBeatBuffer, localSampleRate, localSampleRate * 0.5);
}


void MetronomeUtils::createCustomSounds(const QString &firstBeatAudioFile, const QString &secondaryBeatAudioFile,
                                        Audio::SamplesBuffer &firstBeatBuffer, Audio::SamplesBuffer &secondaryBeatBuffer, quint32 localSampleRate)
{
    if (QFileInfo(firstBeatAudioFile).exists()){
        createBuffer(firstBeatAudioFile, firstBeatBuffer, localSampleRate);
        if (QFileInfo(secondaryBeatAudioFile).exists()) {
            createBuffer(secondaryBeatAudioFile, secondaryBeatBuffer, localSampleRate);
        }
        else{//using the default click sound if the secondary beat audio file is not setted
            createBuffer(DEFAULT_METRONOME_AUDIO_FILE, secondaryBeatBuffer, localSampleRate);
        }
    }
    else{//use the default metronome
        MetronomeUtils::createDefaultSounds(firstBeatBuffer, secondaryBeatBuffer, localSampleRate);
    }
}

void MetronomeUtils::removeSilenceInBufferStart(Audio::SamplesBuffer &buffer)
{
    int audioStartingIndex = 0;
    int frames = buffer.getFrameLenght();
    float sampleMix = 0;
    int channels = buffer.getChannels();
    while(audioStartingIndex < frames){
        sampleMix = 0;
        for (int c = 0; c < channels; ++c) {
            sampleMix += buffer.get(c, audioStartingIndex);
        }
        if (sampleMix != 0) //the first sample was found
            break;
        audioStartingIndex++;
    }

    if (audioStartingIndex > 0){
        buffer.discardFirstSamples(audioStartingIndex);
        qDebug() << "Discarding " << audioStartingIndex << " samples";
    }
}

void MetronomeUtils::createBuffer(const QString &audioFilePath, Audio::SamplesBuffer &outBuffer, quint32 localSampleRate)
{
    std::unique_ptr<Audio::FileReader> reader = Audio::FileReaderFactory::createFileReader(audioFilePath);
    quint32 audioFileSampleRate; //will be changed inside reader->read
    Audio::SamplesBuffer originalBuffer(1);//assuming mono for while
    reader->read(audioFilePath, originalBuffer, audioFileSampleRate);//buffer will be filled with audio file samples

    //need resample?
    if (audioFileSampleRate != (uint)localSampleRate) {
        createResampledBuffer(originalBuffer, outBuffer, audioFileSampleRate, localSampleRate);
    }
    else{
        if (originalBuffer.isMono())
            outBuffer.setToMono();
        outBuffer.setFrameLenght(originalBuffer.getFrameLenght());
        outBuffer.set(originalBuffer);
    }
}

void MetronomeUtils::createResampledBuffer(const Audio::SamplesBuffer &buffer, Audio::SamplesBuffer &outBuffer, int originalSampleRate,
                                     int finalSampleRate)
{
    int finalSize = (double)finalSampleRate/originalSampleRate * buffer.getFrameLenght();
    int channels = buffer.getChannels();
    if (channels > 1)
        outBuffer.setToStereo();
    else
        outBuffer.setToMono();
    outBuffer.setFrameLenght(finalSize);

    for (int c = 0; c < channels; ++c) {
        SimpleResampler resampler;
        resampler.process(buffer.getSamplesArray(c),
                          buffer.getFrameLenght(), outBuffer.getSamplesArray(c), finalSize);
    }
}
