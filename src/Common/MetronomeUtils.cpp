#include "MetronomeUtils.h"

#include "audio/core/SamplesBuffer.h"
#include "audio/file/FileReaderFactory.h"
#include "audio/file/FileReader.h"
#include "audio/Resampler.h"
#include <QString>
#include <QFileInfo>
#include <QFile>
#include <QDir>

using namespace Audio;

const QString MetronomeUtils::DEFAULT_BUILT_IN_METRONOME_ALIAS("Default");
const QString MetronomeUtils::DEFAULT_BUILT_IN_METRONOME_DIR(":/metronome");

QList<QString> MetronomeUtils::getBuiltInMetronomeAliases()
{
    QDir metronomeDir(DEFAULT_BUILT_IN_METRONOME_DIR);
    if (!metronomeDir.exists()) {
        qCritical() << "Metronome folder doesn't exist in resources file!";
    }

    QList<QString> aliases;
    QFileInfoList fileInfos = metronomeDir.entryInfoList();
    foreach (QFileInfo fileInfo, fileInfos) {
       QString fileNameComplete = fileInfo.fileName();
       int indexOf1st =  fileNameComplete.indexOf("_1st");
       if (indexOf1st >= 0) {
          QString alias = fileNameComplete.left(indexOf1st);

          //check if the 2nd file exists
          QString secondFileName = buildMetronomeFileNameFromAlias(alias, false);
          QFileInfo secondFile = QFileInfo(fileInfo.dir(), secondFileName);
          if (secondFile.exists()) {
            aliases.append(alias);
          }
       }
    }
    return aliases;
}

void MetronomeUtils::createDefaultSounds(SamplesBuffer &firstBeat, SamplesBuffer &secondaryBeat, quint32 localSampleRate)
{
    createBuiltInSounds(DEFAULT_BUILT_IN_METRONOME_ALIAS, firstBeat, secondaryBeat, localSampleRate);
}

void MetronomeUtils::createBuiltInSounds(const QString &alias, Audio::SamplesBuffer &firstBeatBuffer, Audio::SamplesBuffer &secondaryBeatBuffer, quint32 localSampleRate)
{
    QString firstBeatFile = buildMetronomeFileNameFromAlias(alias, true);
    QString secondBeatFile = buildMetronomeFileNameFromAlias(alias, false);
    QDir builtInMetronomeDir(DEFAULT_BUILT_IN_METRONOME_DIR);
    createBuffer(QFileInfo(builtInMetronomeDir, firstBeatFile).absoluteFilePath(), firstBeatBuffer, localSampleRate);
    createBuffer(QFileInfo(builtInMetronomeDir, secondBeatFile).absoluteFilePath(), secondaryBeatBuffer, localSampleRate);
}

QString MetronomeUtils::buildMetronomeFileNameFromAlias(const QString &alias, bool isFirstBeat)
{
    return alias + ((isFirstBeat) ? "_1st" : "_2nd") + ".ogg";
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
            createBuffer(DEFAULT_BUILT_IN_METRONOME_ALIAS, secondaryBeatBuffer, localSampleRate);
        }
    }
    else{//use the default metronome
        createDefaultSounds(firstBeatBuffer, secondaryBeatBuffer, localSampleRate);
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
    qDebug() << "Creating audio buffer to file " << audioFilePath;

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
