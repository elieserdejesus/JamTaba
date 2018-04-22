#include "RoomStreamerNode.h"
#include "Mp3Decoder.h"
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

namespace audio {
class Mp3Decoder;
}

using audio::AbstractMp3Streamer;
using audio::NinjamRoomStreamerNode;
using audio::AudioFileStreamerNode;
using audio::Mp3Decoder;
using audio::Mp3DecoderMiniMp3;
using audio::SamplesBuffer;

const int AbstractMp3Streamer::MAX_BYTES_PER_DECODING = 2048;

// +++++++++++++
AbstractMp3Streamer::AbstractMp3Streamer(Mp3Decoder *decoder) :
    decoder(decoder),
    device(nullptr),
    streaming(false),
    bufferedSamples(2, 4096)
{
    bufferedSamples.setFrameLenght(0);// reset internal offset
}

AbstractMp3Streamer::~AbstractMp3Streamer()
{
    delete decoder;
}

void AbstractMp3Streamer::stopCurrentStream()
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

int AbstractMp3Streamer::getSamplesToRender(int targetSampleRate, int outLenght)
{
    bool needResampling = needResamplingFor(targetSampleRate);
    int samplesToRender = needResampling ? getInputResamplingLength(
        getSampleRate(), targetSampleRate, outLenght) : outLenght;
    return samplesToRender;
}

void AbstractMp3Streamer::processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int targetSampleRate, std::vector<midi::MidiMessage> &)
{
    Q_UNUSED(in);

    if (bufferedSamples.isEmpty() || !streaming)
        return;

    int samplesToRender = getSamplesToRender(targetSampleRate, out.getFrameLenght());
    if (samplesToRender <= 0)
        return;

    internalInputBuffer.setFrameLenght(samplesToRender);
    internalInputBuffer.set(bufferedSamples);

    if (needResamplingFor(targetSampleRate)) {
        const auto &resampledBuffer = resampler.resample(internalInputBuffer, out.getFrameLenght());
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

void AbstractMp3Streamer::initialize(const QString &streamPath)
{
    streaming = !streamPath.isNull() && !streamPath.isEmpty();
}

int AbstractMp3Streamer::getSampleRate() const
{
    return decoder->getSampleRate();
}

bool AbstractMp3Streamer::needResamplingFor(int targetSampleRate) const
{
    if (!streaming)
        return false;
    return targetSampleRate != getSampleRate();
}

void AbstractMp3Streamer::decode(const unsigned int maxBytesToDecode)
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
            const auto &decodedBuffer = decoder->decode(in, bytesToProcess);

            // prepare in for the next decoding
            in += bytesToProcess;
            bytesProcessed += bytesToProcess;
            // +++++++++++++++++  PROCESS DECODED SAMPLES ++++++++++++++++
            bufferedSamples.append(decodedBuffer);
        }

        bytesToDecode = bytesToDecode.right(bytesToDecode.size() - bytesProcessed);
    }
}

void AbstractMp3Streamer::setStreamPath(const QString &streamPath)
{
    stopCurrentStream();
    initialize(streamPath);
}

// +++++++++++++++++++++++++++++++++++++++

const int NinjamRoomStreamerNode::BUFFER_SIZE = 128000;

NinjamRoomStreamerNode::NinjamRoomStreamerNode(const QUrl &streamPath) :
    AbstractMp3Streamer(new Mp3DecoderMiniMp3()),
    httpClient(nullptr),
    buffering(false)
{
    setStreamPath(streamPath.toString());
}

bool NinjamRoomStreamerNode::needResamplingFor(int targetSampleRate) const
{
    if (!streaming)
        return false;
    return AbstractMp3Streamer::needResamplingFor(targetSampleRate);
}

void NinjamRoomStreamerNode::initialize(const QString &streamPath)
{
    AbstractMp3Streamer::initialize(streamPath);

    buffering = true;
    bufferedSamples.zero();
    bytesToDecode.clear();

    if (!streamPath.isEmpty()) {

        qCDebug(jtNinjamRoomStreamer) << "connecting in " << streamPath;

        QNetworkReply *reply = httpClient.get(QNetworkRequest(QUrl(streamPath)));
        QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(on_reply_read()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(on_reply_error(QNetworkReply::NetworkError)));
        this->device = reply;
    }
}

void NinjamRoomStreamerNode::on_reply_error(QNetworkReply::NetworkError /*error*/)
{
    QString msg = "ERROR playing room stream";
    qCritical() << msg;
    emit error(msg);
}

void NinjamRoomStreamerNode::on_reply_read()
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
        qCritical() << "problem in device!";
    }
}

NinjamRoomStreamerNode::~NinjamRoomStreamerNode()
{
}

void NinjamRoomStreamerNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out,
                                              int sampleRate, std::vector<midi::MidiMessage> &midiBuffer)
{
    Q_UNUSED(in)
    QMutexLocker locker(&mutex);
    if (buffering && bytesToDecode.size() >= BUFFER_SIZE)
        buffering = false;
    if (!buffering && bytesToDecode.isEmpty())
        buffering = true;
    if (buffering)
        return;
    uint samplesToRender = getSamplesToRender(sampleRate, out.getFrameLenght());
    while (bufferedSamples.getFrameLenght() < samplesToRender) {// need decoding?
        decode(256);
        if (bytesToDecode.isEmpty()) {// no more bytes to decode
            qCritical() << "no more bytes to decode and not enough buffered samples. Buffering ...";
            buffering = true;
            break;
        }
    }
    if (!bufferedSamples.isEmpty())
        AbstractMp3Streamer::processReplacing(in, out, sampleRate, midiBuffer);
}

int NinjamRoomStreamerNode::getBufferingPercentage() const
{
    if (buffering)
        return bytesToDecode.size()/(float)BUFFER_SIZE * 100;

    if (!streaming)
        return 0;
    return 100;//if not buffering and is streaming, the buffer is completed (100%)
}

// ++++++++++++++++++
AudioFileStreamerNode::AudioFileStreamerNode(const QString &file) :
    AbstractMp3Streamer(new Mp3DecoderMiniMp3())
{
    setStreamPath(file);
}

void AudioFileStreamerNode::initialize(const QString &streamPath)
{
    AbstractMp3Streamer::initialize(streamPath);
    QFile *f = new QFile(streamPath);
    if (!f->open(QIODevice::ReadOnly))
        qCritical() << "error opening the file " << streamPath;
    this->device = f;
    bytesToDecode.append(f->readAll());
}

AudioFileStreamerNode::~AudioFileStreamerNode()
{
}

void AudioFileStreamerNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out,
                                             int sampleRate, std::vector<midi::MidiMessage> &midiBuffer)
{
    while (bufferedSamples.getFrameLenght() < out.getFrameLenght())
        decode(1024 + 1024);

    AbstractMp3Streamer::processReplacing(in, out, sampleRate, midiBuffer);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/*
