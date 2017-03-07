#include "LooperSaver.h"
#include "Looper.h"
#include "file/WaveFileWriter.h"

#include <QtConcurrent/QtConcurrent>

using namespace Audio;

LoopSaver::LoopSaver(const QString &savePath, Looper *looper)
    : savePath(savePath),
      looper(looper)
{

}

void LoopSaver::save(const QString &loopFileName, uint bpm, uint bpi, bool encodeInOggVorbis, uint sampleRate)
{
    QDir loopDir(QDir(savePath).absoluteFilePath(loopFileName));
    if (!loopDir.exists())
        loopDir.mkpath(".");

    QList<SamplesBuffer> layersSamples = looper->getLayersSamples();
    for (int layer = 0; layer < layersSamples.size(); ++layer) {
        QtConcurrent::run(this,
                          &LoopSaver::saveSamplesToDisk,
                          loopFileName,
                          layersSamples.at(layer),
                          layer,
                          encodeInOggVorbis,
                          sampleRate);
    }
}

void LoopSaver::saveSamplesToDisk(const QString &loopFileName, const SamplesBuffer &buffer, quint8 layerIndex, bool encodeInOggVorbis, uint sampleRate)
{
    if(!encodeInOggVorbis) {
        WaveFileWriter waveFileWriter;
        QString filePath = QDir(savePath).absoluteFilePath(loopFileName +"/layer_" + QString::number(layerIndex) + ".wav");
        waveFileWriter.write(filePath, buffer, sampleRate);
    }

}
