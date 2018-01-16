#include "LooperPersistence.h"
#include "Looper.h"
#include "file/WaveFileWriter.h"
#include "audio/vorbis/VorbisEncoder.h"
#include "file/FileReaderFactory.h"
#include "audio/SamplesBufferResampler.h"
#include "Utils.h"

#include <QtConcurrent/QtConcurrent>
#include <QJsonDocument>
#include <QFileInfo>

using namespace audio;

LoopInfo::LoopInfo(quint32 bpm, quint16 bpi, const QString &name, bool audioIsEncoded, quint8 mode) :
    bpm(bpm),
    bpi(bpi),
    name(name),
    usingEncodedAudio(audioIsEncoded),
    looperMode(mode)
{
    //
}

LoopInfo::LoopInfo()
    : LoopInfo(0, 0, QString(), false, 0) // calling overloaded constructor
{
    //
}

bool LoopInfo::isValid() const
{
    return !name.isEmpty() && bpm > 0 && bpi > 0 && layers.size() > 0;
}

QString LoopInfo::toString(bool showBpm) const
{
    QString text = name;
    text += " (";

    if (showBpm)
        text += QString::number(bpm) + " BPM, ";

    text += QString::number(bpi) + " BPI, ";
    text += QString::number(layers.size()) + " layers";

    text += ")";

    return text;
}

void LoopInfo::addLayer(bool isLocked, float gain, float pan)
{
    LoopLayerInfo layerInfo;
    layerInfo.locked = isLocked;
    layerInfo.gain = gain;
    layerInfo.pan = pan;
    layers.append(layerInfo);
}

// -------------------------------------------------------------------

LoopSaver::LoopSaver(const QString &savePath, Looper *looper) :
    savePath(savePath),
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

void LoopSaver::save(const QString &loopFileName, uint bpm, uint bpi, bool encodeInOggVorbis, float vorbisQuality, uint sampleRate, quint8 bitDepth)
{
    QDir loopDir(QDir(savePath).absoluteFilePath(loopFileName));
    if (!loopDir.exists()) {
        if (!loopDir.mkpath(".")) {
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
                                     vorbisQuality,
                                     sampleRate,
                                     bitDepth);
    }

    QFile jsonFile(QDir(savePath).absoluteFilePath(loopFileName) + ".json");
    if (jsonFile.open(QIODevice::WriteOnly)) {
        QJsonObject root;
        root["bpm"] = static_cast<int>(bpm);
        root["bpi"] = static_cast<int>(bpi);
        root["loopLenght"] = static_cast<int>(looper->getIntervalLenght());
        root["audioFormat"] = encodeInOggVorbis ? "ogg" : "wave";
        root["looperMode"] = static_cast<int>(looper->getMode());

        QJsonArray layers;
        for (quint8 l = 0; l < looper->getLayers(); ++l) {
            QJsonObject layer;
            layer["locked"] = looper->layerIsLocked(l);
            layer["gain"] = Utils::poweredGainToLinear(looper->getLayerGain(l));
            layer["pan"] = looper->getLayerPan(l);
            layers.append(layer);
        }
        root["layers"] = layers;

        QJsonDocument doc(root);
        jsonFile.write(doc.toJson());
    } else {
        qCritical() << jsonFile.errorString();
    }

    looper->setChanged(false);
}

void LoopSaver::saveSamplesToDisk(const QString &savePath, const QString &loopFileName, const SamplesBuffer &buffer, quint8 layerIndex, bool encodeInOggVorbis, float vorbisQuality, uint sampleRate, quint8 bitDepth)
{
    Q_ASSERT(!loopFileName.isEmpty() && !loopFileName.isNull());
    Q_ASSERT(layerIndex < MAX_LOOP_LAYERS);
    Q_ASSERT(!savePath.isEmpty());

    if (!encodeInOggVorbis) {
        WaveFileWriter waveFileWriter;
        QString filePath = QDir(savePath).absoluteFilePath(loopFileName +"/layer_" + QString::number(layerIndex) + ".wav");
        waveFileWriter.write(filePath, buffer, sampleRate, bitDepth);
    }
    else {
        vorbis::Encoder encoder(2, sampleRate, vorbisQuality);
        QByteArray encodedData = encoder.encode(buffer);
        encodedData.append(encoder.finishIntervalEncoding());
        QString filePath = QDir(savePath).absoluteFilePath(loopFileName +"/layer_" + QString::number(layerIndex) + ".ogg");
        QFile oggFile(filePath);
        if (oggFile.open(QFile::WriteOnly)) {
            oggFile.write(encodedData);
        }
        else {
            qCritical() << "Can't write in the file " << filePath;
        }
    }
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LoopLoader::LoopLoader(const QString &loadPath) :
    loadPath(loadPath)
{

}

void LoopLoader::load(LoopInfo loopInfo, Looper *looper, uint currentSampleRate, quint32 samplesPerInterval)
{
    if (!loopInfo.isValid())
        return;

    looper->setChanged(false);
    looper->setLoading(true);

    looper->stop();
    looper->setMode(static_cast<Looper::Mode>(loopInfo.getLooperMode()));
    looper->setLayers(loopInfo.getLayersCount());

    bool audioIsEncoded = loopInfo.audioIsEncoded();
    QList<LoopLayerInfo> layersInfo = loopInfo.getLayersInfo();
    for (quint8 layer = 0; layer < layersInfo.size(); ++layer) {
        SamplesBuffer samples(2, samplesPerInterval);
        bool loadResult = LoopLoader::loadLoopLayerSamples(loadPath, loopInfo.getName(), layer, audioIsEncoded, currentSampleRate, samples);
        if (loadResult) {
            looper->setLayerSamples(layer, samples);
            bool layerIsLocked = layersInfo.at(layer).locked;
            looper->setLayerLockedState(layer, layerIsLocked);
            looper->setLayerGain(layer, Utils::linearGainToPower(layersInfo.at(layer).gain));
            looper->setLayerPan(layer, layersInfo.at(layer).pan);
        }
    }

    looper->setLoading(false);
    looper->setLoopName(loopInfo.getName());
}

bool LoopLoader::loadAudioFile(const QString &filePath, uint currentSampleRate, SamplesBuffer &out)
{
    QFile audioFile(filePath);
    if (!audioFile.open(QFile::ReadOnly)) {
        qCritical() << "Error loading loop layer samples, can't open " << filePath << audioFile.errorString();
        out.setFrameLenght(0);
        return false;
    }

    auto fileReader = FileReaderFactory::createFileReader(filePath);
    quint32 audioFileSampleRate = 0;
    if (!fileReader->read(filePath, out, audioFileSampleRate))
        return false;

    bool needResample = audioFileSampleRate > 0 && currentSampleRate != audioFileSampleRate;
    if (needResample) {
        SamplesBufferResampler resampler;
        uint desiredLenght = currentSampleRate/static_cast<float>(audioFileSampleRate) * out.getFrameLenght();
        const SamplesBuffer resampledBuffer = resampler.resample(out, desiredLenght);
        out.setFrameLenght(desiredLenght);
        out.set(resampledBuffer);
    }

    return true;
}

bool LoopLoader::loadLoopLayerSamples(const QString &loadPath, const QString &loopName, quint8 layerIndex, bool audioIsEncoded, uint currentSampleRate, SamplesBuffer &out)
{
    QDir audioDir(QDir(loadPath).absoluteFilePath(loopName));
    if (!audioDir.exists()) {
        qCritical() << "Error loading loop layer samples, " << audioDir.absolutePath() << " not exists!";
        out.setFrameLenght(0);
        return false;
    }

    QString audioFileName("layer_" + QString::number(layerIndex) + (audioIsEncoded ? ".ogg" : ".wav"));
    QString audioFilePath(audioDir.absoluteFilePath(audioFileName));

    return LoopLoader::loadAudioFile(audioFilePath, currentSampleRate, out);
}

QList<LoopInfo> LoopLoader::loadLoopsInfo(const QString &loadPath, quint32 bpmToMatch)
{
    QList<LoopInfo> allInfos;

    QDir loadDir(loadPath);
    QDir::Filters filters = QDir::NoDotAndDotDot | QDir::Files;
    QFileInfoList fileInfoList = loadDir.entryInfoList(QStringList("*.json"), filters);
    for (const QFileInfo &fileInfo : fileInfoList) {
        QString loopFilePath = fileInfo.absoluteFilePath();
        LoopInfo loopInfo = LoopLoader::loadLoopInfo(loopFilePath);
        if (loopInfo.isValid() && loopInfo.getBpm() == bpmToMatch)
            allInfos.append(loopInfo);
    }

    return allInfos;
}

LoopInfo LoopLoader::loadLoopInfo(const QString &loopFilePath)
{
    QFile file(loopFilePath);
    if (!file.open(QFile::ReadOnly)) {
        qCritical() << "Error loading loop metada:" << file.errorString();
        return LoopInfo();
    }

    if (QFileInfo(file).suffix() != "json") {
        qCritical() << "Error loading loop metada, not a json file" << loopFilePath;
        return LoopInfo();
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root = doc.object();

    quint32 bpm = (root.contains("bpm") ? (root["bpm"].toInt()) : 0);
    quint16 bpi = (root.contains("bpi") ? (root["bpi"].toInt()) : 0);
    bool audioIsEncoded = root.contains("audioFormat") && root["audioFormat"].toString() == "ogg";
    QString loopName = QFileInfo(file).baseName();
    quint8 looperMode = root.contains("looperMode") ? root["looperMode"].toInt() : 0;

    LoopInfo loopInfo(bpm, bpi, loopName, audioIsEncoded, looperMode);

    if (root.contains("layers")) {
        if (root["layers"].isArray()) { // new loop file format
            QJsonArray layers = root["layers"].toArray();
            for (int i = 0; i < layers.size(); ++i) {
                QJsonObject layer = layers.at(i).toObject();
                bool isLocked = layer.contains("locked") ? layer["locked"].toBool() : false;
                float gain = layer.contains("gain") ? layer["gain"].toDouble() : 1.0;
                float pan = layer.contains("pan") ? layer["pan"].toDouble() : 0.0;
                loopInfo.addLayer(isLocked, gain, pan);
            }
        }
        else { // using old loop file format
            int layers = root["layers"].toInt(0);
            for (int l = 0; l < layers; ++l) {
                loopInfo.addLayer(false, 1.0, 0.0);
            }
        }
    }

    return loopInfo;
}
