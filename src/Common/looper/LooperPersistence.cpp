#include "LooperPersistence.h"
#include "Looper.h"
#include "file/WaveFileWriter.h"
#include "file/FileReaderFactory.h"

#include <QtConcurrent/QtConcurrent>
#include <QJsonDocument>
#include "QFileInfo"

using namespace Audio;

LoopSaver::LoopSaver(const QString &savePath, Looper *looper)
    : savePath(savePath),
      looper(looper)
{

}

QList<quint8> LoopSaver::getLockedLayers(Looper *looper)
{
    QList<quint8> lockedLayers;
    for (int l = 0; l < looper->getLayers(); ++l) {
        if (looper->layerIsLocked(l))
            lockedLayers << l;
    }
    return lockedLayers;
}

void LoopSaver::save(const QString &loopFileName, uint bpm, uint bpi, bool encodeInOggVorbis, uint sampleRate)
{
    QDir loopDir(QDir(savePath).absoluteFilePath(loopFileName));
    if (!loopDir.exists()) {
        if(!loopDir.mkpath(".")){
            qCritical() << "Error creating loop dir" << loopDir;
        }
    }

    QList<SamplesBuffer> layersSamples = looper->getLayersSamples();
    for (int layer = 0; layer < layersSamples.size(); ++layer) {
        LoopSaver::saveSamplesToDisk(savePath,
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

        // save locked layers array
        QJsonArray lockedLayersJsonArray;
        QList<quint8> lockedLayers = getLockedLayers(looper);
        for (quint8 layerIndex : lockedLayers)
            lockedLayersJsonArray.append(layerIndex);

        root["lockedLayers"] = lockedLayersJsonArray;

        QJsonDocument doc(root);
        jsonFile.write(doc.toJson());
    } else {
        qCritical() << jsonFile.errorString();
    }

    looper->setChanged(false);
}

void LoopSaver::saveSamplesToDisk(const QString &savePath, const QString &loopFileName, const SamplesBuffer &buffer, quint8 layerIndex, bool encodeInOggVorbis, uint sampleRate)
{
    Q_ASSERT(!loopFileName.isEmpty() && !loopFileName.isNull());
    Q_ASSERT(layerIndex < Looper::MAX_LOOP_LAYERS);
    Q_ASSERT(!savePath.isEmpty());

    if(!encodeInOggVorbis) {
        WaveFileWriter waveFileWriter;
        QString filePath = QDir(savePath).absoluteFilePath(loopFileName +"/layer_" + QString::number(layerIndex) + ".wav");
        waveFileWriter.write(filePath, buffer, sampleRate);
    }
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LoopLoader::LoopLoader(const QString &loadPath)
    : loadPath(loadPath)
{

}

void LoopLoader::load(LoopInfo loopInfo, Looper *looper)
{
    Q_ASSERT(looper);
    looper->stop();

    looper->setLayers(loopInfo.layers);

    bool audioIsEncoded = loopInfo.audioIsEncoded;
    for (quint8 layer = 0; layer < loopInfo.layers; ++layer) {
        SamplesBuffer samples = loadLoopLayerSamples(loadPath, loopInfo.name, layer, audioIsEncoded);
        looper->setLayerSamples(layer, samples);
        if (loopInfo.lockedLayers.contains(layer))
            looper->setLayerLockedState(layer, true);
    }

}

SamplesBuffer LoopLoader::loadLoopLayerSamples(const QString &loadPath, const QString &loopName, quint8 layerIndex, bool audioIsEncoded)
{
    QDir audioDir(QDir(loadPath).absoluteFilePath(loopName));
    if (!audioDir.exists()){
        qCritical() << "Error loading loop layer samples, " << audioDir.absolutePath() << " not exists!";
        return SamplesBuffer::ZERO_BUFFER;
    }

    QString audioFileName("layer_" + QString::number(layerIndex) + (audioIsEncoded ? ".ogg" : ".wav"));
    QString audioFilePath(audioDir.absoluteFilePath(audioFileName));
    QFile audioFile(audioFilePath);
    if (!audioFile.open(QFile::ReadOnly)) {
        qCritical() << "Error loading loop layer samples, can't open " << audioFilePath << audioFile.errorString();
        return SamplesBuffer::ZERO_BUFFER;
    }

    auto fileReader = FileReaderFactory::createFileReader(audioFilePath);
    SamplesBuffer samplesBuffer(2); // stereo
    quint32 audioFileSampleRate;
    fileReader->read(audioFilePath, samplesBuffer, audioFileSampleRate);

    //TODO resampling if audio file sample rate is different from current audio engine sample rate

    return samplesBuffer;
}

QList<LoopInfo> LoopLoader::loadAllLoopsInfo(const QString &loadPath)
{
    QList<LoopInfo> allInfos;

    QDir loadDir(loadPath);
    QDir::Filters filters = QDir::NoDotAndDotDot | QDir::Files;
    QFileInfoList fileInfoList = loadDir.entryInfoList(QStringList("*.json"), filters);
    for(const QFileInfo &fileInfo : fileInfoList) {
        QString loopFilePath = fileInfo.absoluteFilePath();
        LoopInfo loopInfo = LoopLoader::loadLoopInfo(loopFilePath);
        if (loopInfo.isValid())
            allInfos.append(loopInfo);
    }
    return allInfos;
}

LoopInfo LoopLoader::loadLoopInfo(const QString &loopFilePath)
{
    QFile file(loopFilePath);
    if(!file.open(QFile::ReadOnly)) {
        qCritical() << "Error loading loop metada:" << file.errorString();
        return LoopInfo();
    }

    if (QFileInfo(file).suffix() != "json") {
        qCritical() << "Error loading loop metada, not a json file" << loopFilePath;
        return LoopInfo();
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root = doc.object();

    quint8 layers = (root.contains("layers") ? (root["layers"].toInt()) : 0);
    quint32 bpm = (root.contains("bpm") ? (root["bpm"].toInt()) : 0);
    quint16 bpi = (root.contains("bpi") ? (root["bpi"].toInt()) : 0);
    bool audioIsEncoded = root.contains("audioFormat") && root["audioFormat"].toString() == "ogg";
    QString loopName = QFileInfo(file).baseName();

    LoopInfo loopInfo(layers, bpm, bpi, loopName, audioIsEncoded);

    if(root.contains("lockedLayers")) {
        QJsonArray lockedLayers = root["lockedLayers"].toArray();
        for (int l = 0; l < lockedLayers.count(); ++l) {
            loopInfo.lockedLayers.insert(lockedLayers.at(l).toInt());
        }
    }

    return loopInfo;
}
