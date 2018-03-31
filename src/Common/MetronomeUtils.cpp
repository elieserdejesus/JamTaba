#include "MetronomeUtils.h"

#include "audio/core/SamplesBuffer.h"
#include "file/FileReaderFactory.h"
#include "file/FileReader.h"
#include "audio/Resampler.h"
#include <QString>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>

using audio::SamplesBuffer;
using audio::metronomeUtils;

const QString metronomeUtils::DEFAULT_BUILT_IN_METRONOME_ALIAS("Default");
const QString metronomeUtils::DEFAULT_BUILT_IN_METRONOME_DIR(":/metronome");

QList<QString> metronomeUtils::getBuiltInMetronomeAliases()
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

          // check that the Off and Accent beat files exists
          QString offFileName = buildMetronomeFileNameFromAlias(alias, "off");
          QString accentFileName = buildMetronomeFileNameFromAlias(alias, "accent");
          QFileInfo offFile = QFileInfo(fileInfo.dir(), offFileName);
          QFileInfo accentFile = QFileInfo(fileInfo.dir(), accentFileName);
          if (offFile.exists() && accentFile.exists()) {
              aliases.append(alias);
          }
       }
    }
    return aliases;
}

QList<int> metronomeUtils::getAccentBeats(int beatsPerAccent, int bpi)
{
    QList<int> accentBeats = QList<int>();
    for(int i = 1; beatsPerAccent > 0 && i < bpi; i++) {
        if (i % beatsPerAccent == 0) {
            accentBeats.append(i);
        }
    }
    return accentBeats;
}

QList<int> metronomeUtils::getAccentBeatsFromString(QString value)
{
    QList<int> accentBeats;
    QList<QString> accentBeatsStrings = value.trimmed().split(QRegExp("  *"));

    foreach(QString accentBeatString, accentBeatsStrings) {
        bool ok;
        int accentBeat = accentBeatString.toInt(&ok);
        if (ok) {
            accentBeats.append(accentBeat - 1);
        }
    }

    return accentBeats;
}

void metronomeUtils::createBuiltInSounds(const QString &alias, SamplesBuffer &firstBeatBuffer, SamplesBuffer &offBeatBuffer, SamplesBuffer &accentBeatBuffer, quint32 localSampleRate)
{
    createBuiltInSound(alias, "1st", firstBeatBuffer, localSampleRate);
    createBuiltInSound(alias, "off", offBeatBuffer, localSampleRate);
    createBuiltInSound(alias, "accent", accentBeatBuffer, localSampleRate);
}

void metronomeUtils::createBuiltInSound(const QString &alias, const QString &beat, SamplesBuffer &beatBuffer, quint32 localSampleRate) {
    QString beatFile = buildMetronomeFileNameFromAlias(alias, beat);
    createBuffer(QFileInfo(DEFAULT_BUILT_IN_METRONOME_DIR, beatFile).absoluteFilePath(), beatBuffer, localSampleRate);
}

QString metronomeUtils::buildMetronomeFileNameFromAlias(const QString &alias, const QString &beat)
{
    return (!alias.isEmpty() ? alias : DEFAULT_BUILT_IN_METRONOME_ALIAS) + "_" + beat + ".ogg";
}

void metronomeUtils::createCustomSounds(const QString &firstBeatAudioFile, const QString &offBeatAudioFile, const QString &accentBeatAudioFile,
                                        SamplesBuffer &firstBeatBuffer, SamplesBuffer &offBeatBuffer, SamplesBuffer &accentBeatBuffer, quint32 localSampleRate)
{
    if (QFileInfo(firstBeatAudioFile).exists()){
        createBuffer(firstBeatAudioFile, firstBeatBuffer, localSampleRate);
    } else {//use the default click sound if the first beat audio file is not found
        createBuiltInSound("", "1st", firstBeatBuffer, localSampleRate);
    }
    if (QFileInfo(offBeatAudioFile).exists()) {
        createBuffer(offBeatAudioFile, offBeatBuffer, localSampleRate);
    } else {//using the default click sound if the off beat audio file is not found
        createBuiltInSound("", "off", offBeatBuffer, localSampleRate);
    }
    if (QFileInfo(accentBeatAudioFile).exists()) {
        createBuffer(accentBeatAudioFile, accentBeatBuffer, localSampleRate);
    } else {//using the default click sound if the accent beat audio file is not found
        createBuiltInSound("", "accent", accentBeatBuffer, localSampleRate);
    }
}

void metronomeUtils::removeSilenceInBufferStart(SamplesBuffer &buffer)
{
    int audioStartingIndex = 0;
    int frames = buffer.getFrameLenght();
    float threshold = 0.2f;
    int channels = buffer.getChannels();
    while(audioStartingIndex < frames){
        float sampleMix = 0.0f;
        for (int c = 0; c < channels; ++c) {
            sampleMix += buffer.get(c, audioStartingIndex);
        }
        if (qAbs(sampleMix) >= threshold) //the first sample was found
            break;
        audioStartingIndex++;
    }

    if (audioStartingIndex > 0){
        buffer.discardFirstSamples(audioStartingIndex);
    }
}

void metronomeUtils::createBuffer(const QString &audioFilePath, SamplesBuffer &outBuffer, quint32 localSampleRate)
{
    std::unique_ptr<FileReader> reader = FileReaderFactory::createFileReader(audioFilePath);
    quint32 audioFileSampleRate; //will be changed inside reader->read
    SamplesBuffer originalBuffer(1);//assuming mono for while
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

void metronomeUtils::createResampledBuffer(const SamplesBuffer &buffer, SamplesBuffer &outBuffer, int originalSampleRate,
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
