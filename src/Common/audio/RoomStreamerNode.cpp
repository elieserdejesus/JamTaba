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
#include <QtConcurrent/QtConcurrent>

using namespace Audio;

const qreal PublicRoomStreamerNode::BUFFER_TIME = 4.0; // in seconds

PublicRoomStreamDecoder::PublicRoomStreamDecoder() :
    mp3Decoder(new Mp3DecoderMiniMp3())
{
    //
}

PublicRoomStreamDecoder::~PublicRoomStreamDecoder()
{
    delete mp3Decoder;
}

void PublicRoomStreamDecoder::reset()
{
    mp3Decoder->reset();
}

int PublicRoomStreamDecoder::getSampleRate() const
{
    return mp3Decoder->getSampleRate();
}

void PublicRoomStreamDecoder::doDecode(QByteArray &bytesToDecode)
{
    qint64 totalBytesToProcess = bytesToDecode.size();
    char *in = bytesToDecode.data();

    if (totalBytesToProcess <= 0)
        return;

    Audio::SamplesBuffer decodedSamples(2);
    int bytesProcessed = 0;
    static const int MAX_BYTES_PER_DECODING = 2048;
    while (bytesProcessed < totalBytesToProcess) {// split bytesReaded in chunks to avoid a very large decoded buffer
        // chunks maxsize is 2048 bytes
        int bytesToProcess = std::min((int)(totalBytesToProcess - bytesProcessed), MAX_BYTES_PER_DECODING);
        const Audio::SamplesBuffer *decodedBuffer = mp3Decoder->decode(in, bytesToProcess);

        // prepare in for the next decoding
        in += bytesToProcess;
        bytesProcessed += bytesToProcess;
        // +++++++++++++++++  PROCESS DECODED SAMPLES ++++++++++++++++
        decodedSamples.append(*decodedBuffer);
    }

    if (!decodedSamples.isEmpty())
        emit audioDecoded(decodedSamples);
}

void PublicRoomStreamDecoder::decode(QByteArray &bytesToDecode)
{
    QtConcurrent::run(this, &PublicRoomStreamDecoder::doDecode, bytesToDecode);
}

PublicRoomStreamerNode::PublicRoomStreamerNode(const QUrl &streamPath) :
    decoder(new PublicRoomStreamDecoder()),
    httpClient(nullptr),
    buffering(false),
    streaming(false),
    device(nullptr),
    bufferedSamples(2, 4096)
{
    setStreamPath(streamPath.toString());
    bufferedSamples.setFrameLenght(0);

    QObject::connect(decoder, &PublicRoomStreamDecoder::audioDecoded, this, &PublicRoomStreamerNode::appendDecodeAudio);
}

void PublicRoomStreamerNode::appendDecodeAudio(const SamplesBuffer &decodedBuffer)
{
    mutexBufferedSamples.lock();

    bufferedSamples.append(decodedBuffer);

    mutexBufferedSamples.unlock();
}

void PublicRoomStreamerNode::stopCurrentStream()
{
    qCDebug(jtNinjamRoomStreamer) << "stopping room stream";

    if (device) {
        device->deleteLater();
        device = nullptr;
    }

    decoder->reset();// discard unprocessed bytes

    mutexBufferedSamples.lock();
    bufferedSamples.setFrameLenght(0);// discard samples
    mutexBufferedSamples.unlock();

    streaming = false;
    buffering = false;

    mutexDownloadedBytes.lock();
    downloadedBytes.clear();
    mutexDownloadedBytes.unlock();

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
    bufferedTime = 0.0;
    bufferedSamples.setFrameLenght(0);
    downloadedBytes.clear();
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
        mutexDownloadedBytes.lock();
        downloadedBytes.append(device->readAll());
        mutexDownloadedBytes.unlock();
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

    if (!streaming)
        return;

    bufferedTime = bufferedSamples.getFrameLenght()/(float)sampleRate;
    int samplesToRender = getSamplesToRender(sampleRate, out.getFrameLenght());

    {
        QMutexLocker locker(&mutexDownloadedBytes);

        // check if is necessary decode more mp3 data
        if (bufferedTime < BUFFER_TIME && !downloadedBytes.isEmpty() ) {
            QByteArray bytesToDecode = downloadedBytes.left(512);
            downloadedBytes.remove(0, bytesToDecode.size());
            decoder->decode(bytesToDecode);
        }

        if (!buffering && downloadedBytes.isEmpty())
            buffering = true;
    }

    if (buffering && bufferedTime >= BUFFER_TIME)
        buffering = false;

    if (buffering)
        return;

    {
        QMutexLocker locker(&mutexBufferedSamples);
        if (bufferedSamples.isEmpty() || samplesToRender <= 0)
            return;

        internalInputBuffer.setFrameLenght(samplesToRender);
        internalInputBuffer.set(bufferedSamples);
        bufferedSamples.discardFirstSamples(samplesToRender);// keep non rendered samples for next audio callback
    }

    QMutexLocker locker(&mutex);
    if (needResamplingFor(sampleRate)) {
        const Audio::SamplesBuffer &resampledBuffer = resampler.resample(internalInputBuffer,
                                                                         out.getFrameLenght());
        internalOutputBuffer.setFrameLenght(resampledBuffer.getFrameLenght());
        internalOutputBuffer.set(resampledBuffer);
    } else {
        internalOutputBuffer.setFrameLenght(out.getFrameLenght());
        internalOutputBuffer.set(internalInputBuffer);
    }

    if (internalOutputBuffer.getFrameLenght() < out.getFrameLenght())
        qCDebug(jtNinjamRoomStreamer) << out.getFrameLenght()
                                         - internalOutputBuffer.getFrameLenght() << " samples missing";

    this->lastPeak.update(internalOutputBuffer.computePeak());

    out.add(internalOutputBuffer);
}

int PublicRoomStreamerNode::getBufferingPercentage()
{
    if (buffering) {
        int percentage = bufferedTime/BUFFER_TIME * 100;
        if (percentage <= 100)
            return percentage;
    }

    if (!streaming)
        return 0;

    return 100;//if not buffering and is streaming, the buffer is completed (100%)
}
