#include "VorbisEncoder.h"
#include <QDebug>
#include <QThread>
#include "log/Logging.h"
#include "Vorbis.h"

using vorbis::Encoder;

Encoder::Encoder()
    :initialized(false)
{
    init(1, 44100, vorbis::EncoderQualityNormal);
}

Encoder::Encoder(uint channels, uint sampleRate, float quality):
    initialized(false)
{
    init(channels, sampleRate, quality);
}

void Encoder::init(uint channels, uint sampleRate, float quality)
{
    vorbis_info_init(&info);

    if (vorbis_encode_init_vbr(&info, static_cast<long>(channels), static_cast<long>(sampleRate), quality) != 0) {
        qCritical() << "vorbis encoder initialization error!";
    }
    vorbis_comment_init(&comment);
    vorbis_comment_add_tag(&comment, "Encoder", "Jamtaba");

    qCDebug(jtNinjamVorbisEncoder) << "Initializing Encoder sampleRate:" << sampleRate << " channels: " << channels << " quality: " << quality;

    streamID = 0;
    isFirstEncoding = true;

    totalEncoded = 0;
}

void Encoder::clearState()
{
    for (int i = 0; i < 2; ++i) {
        ogg_stream_clear(&streamState);
    }

    vorbis_block_clear(&block);
    vorbis_dsp_clear(&dspState);
    outBuffer.clear();
}

Encoder::~Encoder()
{
    qCDebug(jtNinjamVorbisEncoder) << "ENCODER DESTRUCTOR! Thread:" <<  QThread::currentThreadId();
    if (initialized) {
        clearState();
    }
    vorbis_comment_clear(&comment);
    vorbis_info_clear(&info);
}

void Encoder::encodeFirstVorbisHeaders()
{
    vorbis_analysis_init(&dspState, &info);
    vorbis_block_init(&dspState, &block);

    ogg_stream_init(&streamState, streamID++);

    // writing headers
    ogg_packet header, header_comm, header_code;
    vorbis_analysis_headerout(&dspState, &comment, &header, &header_comm, &header_code);
    ogg_stream_packetin(&streamState, &header);
    ogg_stream_packetin(&streamState, &header_comm);
    ogg_stream_packetin(&streamState, &header_code);

    // write ogg_page page in out buffer;
    while (true) {
        ogg_page page;
        int result = ogg_stream_flush(&streamState, &page);
        if (result == 0) break;
        // header and body
        outBuffer.append((const char*)page.header, page.header_len); // memcpy(buffer, page.header, page.header_len);
        outBuffer.append((const char*)page.body, page.body_len); // memcpy(buffer, page.body, page.body_len);
    }
    initialized = true;
    isFirstEncoding = false; // clear state in next encoding
}

/**
 * @brief Encoder::encode
 * @param data array
 * @param dataLenght Data Lenght in bytes
 * @param channels number of channels
 * @return
 */
QByteArray Encoder::encode(const audio::SamplesBuffer &audioBuffer)
{
    if (!initialized) {
        if (!isFirstEncoding) {
            clearState();
        }
        encodeFirstVorbisHeaders();
    }
    else {
        outBuffer.clear();
    }

    int samples = audioBuffer.getFrameLenght();

    if (samples > 0) { // is not the end
        float** vorbisBuffer = vorbis_analysis_buffer(&dspState, samples); // copy the samples to encode to vorbis input buffer

        int maxChannels = std::min(info.channels, audioBuffer.getChannels());
        int dataLenght = samples * sizeof(float);
        for (int c = 0; c < maxChannels; c++) {
            memcpy(vorbisBuffer[c], audioBuffer.getSamplesArray(c), dataLenght);
        }
    }

    // lenght == 0 in the end of interval
    int result = vorbis_analysis_wrote(&dspState, samples); // tell the library how much we actually submitted
    if (result != 0) {
        qCritical() << "encoder error!";
    }

    //++++++++++++++++++++++++ encoding +++++++++
    bool endOfStream = false;
    while (vorbis_analysis_blockout(&dspState, &block)) {
        vorbis_analysis(&block, NULL);
        vorbis_bitrate_addblock(&block);
        ogg_packet packet;
        while (vorbis_bitrate_flushpacket(&dspState, &packet)) {
            ogg_stream_packetin(&streamState, &packet);
            while (!endOfStream) {
                ogg_page page;
                if (!ogg_stream_flush(&streamState, &page)) {
                    break;
                }

                // header and body
                outBuffer.append((const char*)page.header, page.header_len); // memcpy(buffer, page.header, page.header_len);
                outBuffer.append((const char*)page.body, page.body_len); // memcpy(buffer, page.body, page.body_len);
                if (ogg_page_eos(&page))
                    endOfStream = true;
            }
        }
    }
    return outBuffer;
}

QByteArray Encoder::finishIntervalEncoding()
{
    QByteArray data = encode(audio::SamplesBuffer::ZERO_BUFFER);//pass zero samples to vorbis and finalize the encoding process
    initialized = false;

    return data;
}
