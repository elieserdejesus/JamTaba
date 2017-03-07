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

void LoopSaver::save(bool encodeInOggVorbis, uint loopLenght, uint sampleRate)
{
    QList<SamplesBuffer> layersSamples = looper->getLayersSamples();
    for (int layer = 0; layer < layersSamples.size(); ++layer) {
        QtConcurrent::run(this,
                          &LoopSaver::saveSamplesToDisk,
                          layersSamples.at(layer),
                          layer,
                          encodeInOggVorbis,
                          sampleRate);
    }
}

void LoopSaver::saveSamplesToDisk(const SamplesBuffer &buffer, quint8 layerIndex, bool encodeInOggVorbis, uint sampleRate)
{
    if(!encodeInOggVorbis) {
        WaveFileWriter waveFileWriter;
        QString filePath = QDir(savePath).absoluteFilePath("layer_" + QString::number(layerIndex) + ".wav");
        waveFileWriter.write(filePath, buffer, sampleRate);
    }

}
