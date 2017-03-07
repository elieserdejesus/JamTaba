#include "LooperSaver.h"
#include "Looper.h"
#include "file/WaveFileWriter.h"

#include <QtConcurrent/QtConcurrent>
#include <QJsonDocument>

using namespace Audio;

LoopSaver::LoopSaver(const QString &savePath, Looper *looper)
    : savePath(savePath),
      looper(looper)
{

}

void LoopSaver::save(const QString &loopFileName, uint bpm, uint bpi, bool encodeInOggVorbis, uint sampleRate)
{
    QDir loopDir(QDir(savePath).absoluteFilePath(loopFileName));
    if (!loopDir.exists()) {
        loopDir.mkpath(".");
    }

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

    QFile jsonFile(QDir(savePath).absoluteFilePath(loopFileName) + ".json");
    if (jsonFile.open(QIODevice::WriteOnly)) {
        QJsonObject root;
        root["bpm"] = static_cast<int>(bpm);
        root["bpi"] = static_cast<int>(bpi);
        root["layers"] = layersSamples.count();
        root["loopLenght"] = static_cast<int>(looper->getIntervalLenght());
        root["sampleRate"] = static_cast<int>(sampleRate);
        root["audioFormat"] = encodeInOggVorbis ? "ogg" : "wave";

        QJsonDocument doc(root);
        jsonFile.write(doc.toJson());
    } else {
        qCritical() << jsonFile.errorString();
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
