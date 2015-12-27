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

const int AbstractMp3Streamer::MAX_BYTES_PER_DECODING = 2048;

// +++++++++++++
AbstractMp3Streamer::AbstractMp3Streamer(Audio::Mp3Decoder *decoder) :
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

void AbstractMp3Streamer::processReplacing(const Audio::SamplesBuffer &in,
                                           Audio::SamplesBuffer &out, int targetSampleRate,
                                           const Midi::MidiBuffer &)
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
        const Audio::SamplesBuffer &resampledBuffer = resampler.resample(internalInputBuffer,
                                                                         out.getFrameLenght());
        internalOutputBuffer.setFrameLenght(resampledBuffer.getFrameLenght());
        internalOutputBuffer.set(resampledBuffer);
    } else {
        internalOutputBuffer.setFrameLenght(out.getFrameLenght());
        internalOutputBuffer.set(internalInputBuffer);
    }

    qCDebug(jtNinjamRoomStreamer) << "discarding " << samplesToRender << " samples";
    bufferedSamples.discardFirstSamples(samplesToRender);// keep non rendered samples for next audio callback

    if (internalOutputBuffer.getFrameLenght() < out.getFrameLenght())
        qCDebug(jtNinjamRoomStreamer) << out.getFrameLenght()
            - internalOutputBuffer.getFrameLenght() << " samples missing";

    this->lastPeak.update(internalOutputBuffer.computePeak());

    out.add(internalOutputBuffer);
}

void AbstractMp3Streamer::initialize(QString streamPath)
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
            const Audio::SamplesBuffer *decodedBuffer = decoder->decode(in, bytesToProcess);

            // prepare in for the next decoding
            in += bytesToProcess;
            bytesProcessed += bytesToProcess;
            // +++++++++++++++++  PROCESS DECODED SAMPLES ++++++++++++++++
            bufferedSamples.append(*decodedBuffer);
        }

        bytesToDecode = bytesToDecode.right(bytesToDecode.size() - bytesProcessed);
        if (bytesToDecode.isEmpty())
            qCDebug(jtNinjamRoomStreamer) << bytesProcessed << " decoded  bytesToDecode: "
                                          << bytesToDecode.size();
    }
}

void AbstractMp3Streamer::setStreamPath(QString streamPath)
{
    stopCurrentStream();
    initialize(streamPath);
}

// +++++++++++++++++++++++++++++++++++++++
NinjamRoomStreamerNode::NinjamRoomStreamerNode(QUrl streamPath, int bufferTimeInSeconds) :
    AbstractMp3Streamer(new Mp3DecoderMiniMp3()),
    httpClient(nullptr),
    bufferTime(bufferTimeInSeconds),
    buffering(false)
{
    setStreamPath(streamPath.toString());
}

NinjamRoomStreamerNode::NinjamRoomStreamerNode(int bufferTimeInSeconds) :
    AbstractMp3Streamer(new Mp3DecoderMiniMp3()),
    httpClient(nullptr),
    bufferTime(bufferTimeInSeconds),
    buffering(false)
{
    setStreamPath("");
}

bool NinjamRoomStreamerNode::needResamplingFor(int targetSampleRate) const
{
    if (!streaming)
        return false;
    return AbstractMp3Streamer::needResamplingFor(targetSampleRate);
}

void NinjamRoomStreamerNode::initialize(QString streamPath)
{
    AbstractMp3Streamer::initialize(streamPath);
    buffering = true;
    bufferedSamples.zero();
    bytesToDecode.clear();
    if (!streamPath.isEmpty()) {
        qCDebug(jtNinjamRoomStreamer) << "connecting in " << streamPath;
        if (httpClient)
            httpClient->deleteLater();
        httpClient = new QNetworkAccessManager(this);
        QNetworkReply *reply = httpClient->get(QNetworkRequest(QUrl(streamPath)));
        QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(on_reply_read()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
                         SLOT(on_reply_error(QNetworkReply::NetworkError)));
        this->device = reply;
    }
}

void NinjamRoomStreamerNode::on_reply_error(QNetworkReply::NetworkError /*error*/)
{
    QString msg = "ERROR playing room stream";
    qCCritical(jtNinjamRoomStreamer) << msg;
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
        qCDebug(jtNinjamRoomStreamer) << "bytes downloaded  bytesToDecode:"<<bytesToDecode.size()
                                      << " bufferedSamples: " << bufferedSamples.getFrameLenght();
    } else {
        qCCritical(jtNinjamRoomStreamer) << "problem in device!";
    }
}

NinjamRoomStreamerNode::~NinjamRoomStreamerNode()
{
    qCDebug(jtNinjamRoomStreamer) << "RoomStreamerNode destructor!";
}

void NinjamRoomStreamerNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out,
                                              int sampleRate, const Midi::MidiBuffer &midiBuffer)
{
    Q_UNUSED(in)
    QMutexLocker locker(&mutex);
    if (buffering && bytesToDecode.size() >= 120000)
        buffering = false;
    if (!buffering && bytesToDecode.isEmpty())
        buffering = true;
    if (buffering)
        return;
    int samplesToRender = getSamplesToRender(sampleRate, out.getFrameLenght());
    while (bufferedSamples.getFrameLenght() < samplesToRender) {// need decoding?
        decode(256);
        if (bytesToDecode.isEmpty()) {// no more bytes to decode
            qCritical() << "BREAK";
            break;
        }
    }
    if (!bufferedSamples.isEmpty())
        AbstractMp3Streamer::processReplacing(in, out, sampleRate, midiBuffer);
}

// ++++++++++++++++++
AudioFileStreamerNode::AudioFileStreamerNode(QString file) :
    AbstractMp3Streamer(new Mp3DecoderMiniMp3())
{
    setStreamPath(file);
}

void AudioFileStreamerNode::initialize(QString streamPath)
{
    AbstractMp3Streamer::initialize(streamPath);
    QFile *f = new QFile(streamPath);
    if (!f->open(QIODevice::ReadOnly))
        qCCritical(jtNinjamRoomStreamer) << "error opening the file " << streamPath;
    this->device = f;
    bytesToDecode.append(f->readAll());
}

AudioFileStreamerNode::~AudioFileStreamerNode()
{
}

void AudioFileStreamerNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out,
                                             int sampleRate, const Midi::MidiBuffer &midiBuffer)
{
    while (bufferedSamples.getFrameLenght() < out.getFrameLenght())
        decode(1024 + 1024);

    AbstractMp3Streamer::processReplacing(in, out, sampleRate, midiBuffer);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/*

TestStreamerNode::TestStreamerNode(int sampleRate) :
    AbstractMp3Streamer(new Mp3DecoderMiniMp3())
{
    oscilator = new OscillatorAudioNode(440, sampleRate);
    playing = true;
}

TestStreamerNode::~TestStreamerNode()
{
    delete oscilator;
}

void TestStreamerNode::initialize(QString streamPath)
{
    AbstractMp3Streamer::initialize(streamPath);
}

void TestStreamerNode::processReplacing(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                                        int sampleRate, const Midi::MidiBuffer &midiBuffer)
{
    if (playing)
        oscilator->processReplacing(in, out, sampleRate, midiBuffer);
    lastPeak.update(out.computePeak());
}

void TestStreamerNode::setStreamPath(QString streamPath)
{
    Q_UNUSED(streamPath);
    playing = true;
    bytesToDecode.clear();
}

void TestStreamerNode::stopCurrentStream()
{
    playing = false;
}
