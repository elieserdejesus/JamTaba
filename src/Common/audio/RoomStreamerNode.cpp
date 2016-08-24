#include "RoomStreamerNode.h"
#include "codec.h"
#include "core/AudioDriver.h"
#include "core/SamplesBuffer.h"
#include "log/Logging.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkReply>
#include <QDebug>
#include <QBuffer>
#include <QObject>
#include <QDateTime>
#include <QWaitCondition>
#include <cmath>
#include <QMutexLocker>
#include <QFile>

using namespace Audio;

const int PublicRoomStreamerNode::MAX_BYTES_PER_DECODING = 2048;
const int PublicRoomStreamerNode::BUFFER_SIZE = 128000;

PublicRoomStreamerNode::PublicRoomStreamerNode(const QUrl &streamPath) :
    decoder(new Mp3DecoderMiniMp3()),
    httpClient(nullptr),
    buffering(false),
    streaming(false),
    device(nullptr),
    bufferedSamples(2, 4096)
{
    setStreamPath(streamPath.toString());
    bufferedSamples.setFrameLenght(0);
}

void PublicRoomStreamerNode::stopCurrentStream()
{
    qCDebug(jtNinjamRoomStreamer) << "stopping room stream";

    if (device) {
        decoder->reset();// discard unprocessed bytes
        device->deleteLater();
        device = nullptr;
        bufferedSamples.zero();// discard samples
        streaming = false;
    }
    bytesToDecode.clear();
    lastPeak.zero();
}

int PublicRoomStreamerNode::getSamplesToRender(int targetSampleRate, int outLenght)
{
    bool needResampling = needResamplingFor(targetSampleRate);
    int samplesToRender = needResampling ? getInputResamplingLength(
        getSampleRate(), targetSampleRate, outLenght) : outLenght;
    return samplesToRender;
}


int PublicRoomStreamerNode::getSampleRate() const
{
    return decoder->getSampleRate();
}

void PublicRoomStreamerNode::decode(const unsigned int maxBytesToDecode)
{
    if (!device)
        return;
    qint64 totalBytesToProcess = std::min((int)maxBytesToDecode, bytesToDecode.size());
    char *in = bytesToDecode.data();

    if (totalBytesToProcess > 0) {
        int bytesProcessed = 0;
        while (bytesProcessed < totalBytesToProcess) {// split bytesReaded in chunks to avoid a very large decoded buffer
            // chunks maxsize is 2048 bytes
            int bytesToProcess = std::min((int)(totalBytesToProcess - bytesProcessed),
                                          MAX_BYTES_PER_DECODING);
            const Audio::SamplesBuffer *decodedBuffer = decoder->decode(in, bytesToProcess);

            // prepare in for the next decoding
            in += bytesToProcess;
            bytesProcessed += bytesToProcess;
            // +++++++++++++++++  PROCESS DECODED SAMPLES ++++++++++++++++
            bufferedSamples.append(*decodedBuffer);
        }

        bytesToDecode = bytesToDecode.right(bytesToDecode.size() - bytesProcessed);
    }
}

void PublicRoomStreamerNode::setStreamPath(const QString &streamPath)
{
    stopCurrentStream();
    initialize(streamPath);
}

bool PublicRoomStreamerNode::needResamplingFor(int targetSampleRate) const
{
    if (!streaming)
        return false;
    return targetSampleRate != getSampleRate();
}

void PublicRoomStreamerNode::initialize(const QString &streamPath)
{
    streaming = !streamPath.isNull() && !streamPath.isEmpty();
    buffering = true;
    bufferedSamples.zero();
    bytesToDecode.clear();
    if (!streamPath.isEmpty()) {
        qCDebug(jtNinjamRoomStreamer) << "connecting in " << streamPath;
        if (httpClient)
            httpClient->deleteLater();
        httpClient = new QNetworkAccessManager(this);
        QNetworkReply *reply = httpClient->get(QNetworkRequest(QUrl(streamPath)));
        QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(processDownloadedData()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
                         SLOT(handleNetworkError(QNetworkReply::NetworkError)));
        this->device = reply;
    }
}

void PublicRoomStreamerNode::handleNetworkError(QNetworkReply::NetworkError /*error*/)
{
    QString msg = "ERROR playing room stream";
    qCCritical(jtNinjamRoomStreamer) << msg;
    emit error(msg);
}

void PublicRoomStreamerNode::processDownloadedData()
{
    if (!device) {
        qCDebug(jtNinjamRoomStreamer) << "device is null!";
        return;
    }
    if (device->isOpen() && device->isReadable()) {
        QMutexLocker locker(&mutex);
        bytesToDecode.append(device->readAll());
        if (buffering) {
            qCDebug(jtNinjamRoomStreamer) << "bytes downloaded  bytesToDecode:"<<bytesToDecode.size()
                                      << " bufferedSamples: " << bufferedSamples.getFrameLenght();
        }
    } else {
        qCCritical(jtNinjamRoomStreamer) << "problem in device!";
    }
}

PublicRoomStreamerNode::~PublicRoomStreamerNode()
{
    delete decoder;
}

void PublicRoomStreamerNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out,
                                              int sampleRate, const Midi::MidiMessageBuffer &midiBuffer)
{
    Q_UNUSED(in)

    QMutexLocker locker(&mutex);
    if (buffering && bytesToDecode.size() >= BUFFER_SIZE)
        buffering = false;
    if (!buffering && bytesToDecode.isEmpty())
        buffering = true;
    if (buffering)
        return;
    int samplesToRender = getSamplesToRender(sampleRate, out.getFrameLenght());
    while (bufferedSamples.getFrameLenght() < samplesToRender) {// need decoding?
        if (!bytesToDecode.isEmpty()) {
            decode(qMin(32, bytesToDecode.size())); //decoding small packets to avoid blocking the audio thread
        }
        else {// no more bytes to decode
            //qDebug() << "no more bytes to decode and not enough buffered samples. Buffering ...";
            buffering = true;
            break;
        }
    }
    if (!bufferedSamples.isEmpty()) {
        Q_UNUSED(in);

        if (bufferedSamples.isEmpty() || !streaming)
            return;

        int samplesToRender = getSamplesToRender(sampleRate, out.getFrameLenght());
        if (samplesToRender <= 0)
            return;

        internalInputBuffer.setFrameLenght(samplesToRender);
        internalInputBuffer.set(bufferedSamples);

        if (needResamplingFor(sampleRate)) {
            const Audio::SamplesBuffer &resampledBuffer = resampler.resample(internalInputBuffer,
                                                                             out.getFrameLenght());
            internalOutputBuffer.setFrameLenght(resampledBuffer.getFrameLenght());
            internalOutputBuffer.set(resampledBuffer);
        } else {
            internalOutputBuffer.setFrameLenght(out.getFrameLenght());
            internalOutputBuffer.set(internalInputBuffer);
        }

        bufferedSamples.discardFirstSamples(samplesToRender);// keep non rendered samples for next audio callback

        if (internalOutputBuffer.getFrameLenght() < out.getFrameLenght())
            qCDebug(jtNinjamRoomStreamer) << out.getFrameLenght()
                - internalOutputBuffer.getFrameLenght() << " samples missing";

        this->lastPeak.update(internalOutputBuffer.computePeak());

        out.add(internalOutputBuffer);
    }
}

int PublicRoomStreamerNode::getBufferingPercentage() const
{
    if (buffering)
        return bytesToDecode.size()/(float)BUFFER_SIZE * 100;

    if (!streaming)
        return 0;
    return 100;//if not buffering and is streaming, the buffer is completed (100%)
}
