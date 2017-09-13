#include "MetronomeUtils.h"

#include "audio/core/SamplesBuffer.h"
#include "file/FileReaderFactory.h"
#include "file/FileReader.h"
#include "audio/Resampler.h"
#include "log/Logging.h"
#include <QString>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>

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
          qCDebug(jtMetronome) << "getBuiltInMetronomeAliases checking " << alias;

          // check that the Off and Accent beat files exists
          QString offFileName = buildMetronomeFileNameFromAlias(alias, "off");
          QString accentFileName = buildMetronomeFileNameFromAlias(alias, "accent");
          QFileInfo offFile = QFileInfo(fileInfo.dir(), offFileName);
          QFileInfo accentFile = QFileInfo(fileInfo.dir(), accentFileName);
          if (offFile.exists() && accentFile.exists()) {
              aliases.append(alias);
              qCDebug(jtMetronome) << alias << " OK";
          } else {
              qCDebug(jtMetronome) << alias << " skipped";
          }
       }
    }
    return aliases;
}

QList<int> MetronomeUtils::getAccentBeats(int beatsPerAccent, int bpi)
{
    qCDebug(jtMetronome) << "MetronomeUtils::getAccentBeats" << beatsPerAccent << bpi;
    QList<int> accentBeats = QList<int>();
    for(int i = 1; beatsPerAccent > 0 && i < bpi; i++) {
        if (i % beatsPerAccent == 0) {
            accentBeats.append(i);
        }
    }
    return accentBeats;
}

QList<int> MetronomeUtils::getAccentBeatsFromString(QString value)
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

    qCDebug(jtMetronome) << "MetronomeUtils::getAccentBeatsFromString ->" << value << accentBeats;
    return accentBeats;
}

void MetronomeUtils::createBuiltInSounds(const QString &alias, Audio::SamplesBuffer &firstBeatBuffer, Audio::SamplesBuffer &offBeatBuffer, Audio::SamplesBuffer &accentBeatBuffer, quint32 localSampleRate)
{
    createBuiltInSound(alias, "1st", firstBeatBuffer, localSampleRate);
    createBuiltInSound(alias, "off", offBeatBuffer, localSampleRate);
    createBuiltInSound(alias, "accent", accentBeatBuffer, localSampleRate);
}

void MetronomeUtils::createBuiltInSound(const QString &alias, const QString &beat, Audio::SamplesBuffer &beatBuffer, quint32 localSampleRate) {
    QString beatFile = buildMetronomeFileNameFromAlias(alias, beat);
    createBuffer(QFileInfo(DEFAULT_BUILT_IN_METRONOME_DIR, beatFile).absoluteFilePath(), beatBuffer, localSampleRate);
}

QString MetronomeUtils::buildMetronomeFileNameFromAlias(const QString &alias, const QString &beat)
{
    return (!alias.isEmpty() ? alias : DEFAULT_BUILT_IN_METRONOME_ALIAS) + "_" + beat + ".ogg";
}

void MetronomeUtils::createCustomSounds(const QString &firstBeatAudioFile, const QString &offBeatAudioFile, const QString &accentBeatAudioFile,
                                        Audio::SamplesBuffer &firstBeatBuffer, Audio::SamplesBuffer &offBeatBuffer, Audio::SamplesBuffer &accentBeatBuffer, quint32 localSampleRate)
{
    if (QFileInfo(firstBeatAudioFile).exists()){
        createBuffer(firstBeatAudioFile, firstBeatBuffer, localSampleRate);
        qCDebug(jtMetronome) << firstBeatAudioFile << " OK";
    } else {//use the default click sound if the first beat audio file is not found
        qCDebug(jtMetronome) << firstBeatAudioFile << " missing";
        createBuiltInSound("", "1st", firstBeatBuffer, localSampleRate);
    }
    if (QFileInfo(offBeatAudioFile).exists()) {
        createBuffer(offBeatAudioFile, offBeatBuffer, localSampleRate);
        qCDebug(jtMetronome) << offBeatAudioFile << " OK";
    } else {//using the default click sound if the off beat audio file is not found
        qCDebug(jtMetronome) << offBeatAudioFile << " missing";
        createBuiltInSound("", "off", offBeatBuffer, localSampleRate);
    }
    if (QFileInfo(accentBeatAudioFile).exists()) {
        createBuffer(accentBeatAudioFile, accentBeatBuffer, localSampleRate);
        qCDebug(jtMetronome) << accentBeatAudioFile << " OK";
    } else {//using the default click sound if the accent beat audio file is not found
        qCDebug(jtMetronome) << accentBeatAudioFile << " missing";
        createBuiltInSound("", "accent", accentBeatBuffer, localSampleRate);
    }
}

void MetronomeUtils::removeSilenceInBufferStart(Audio::SamplesBuffer &buffer)
{
    int audioStartingIndex = 0;
    int frames = buffer.getFrameLenght();
    float sampleMix = 0.0f;
    float threshold = 0.2f;
    int channels = buffer.getChannels();
    while(audioStartingIndex < frames){
        sampleMix = 0.0f;
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
