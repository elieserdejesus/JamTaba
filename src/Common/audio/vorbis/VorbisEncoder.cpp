#include "VorbisEncoder.h"
#include <QDebug>
#include <ctime>
#include <QThread>
#include "log/Logging.h"


//these vorbis quality values are discussed here: https://github.com/elieserdejesus/JamTaba/issues/456#issuecomment-226920734
const float VorbisEncoder::QUALITY_LOW    = -0.1f; // ~48 – ~64 kbps
const float VorbisEncoder::QUALITY_NORMAL =  0; // ~64 – ~80 kbps. 
const float VorbisEncoder::QUALITY_HIGH   =  0.3f;  // ~112 – ~128 kbps. In ogg vorbis 112 Kbps is better than 128 kbps mp3

VorbisEncoder::VorbisEncoder()
    :initialized(false)
{
    init(1, 44100, QUALITY_NORMAL);
}

VorbisEncoder::VorbisEncoder(int channels, int sampleRate, float quality):
    initialized(false)
{
    init(channels, sampleRate, quality);
}

void VorbisEncoder::init(int channels, int sampleRate, float quality){
    vorbis_info_init(&info);

    if(vorbis_encode_init_vbr(&info, (long) channels, (long) sampleRate, quality) != 0){
        qCritical() << "vorbis encoder initialization error!";
    }
    vorbis_comment_init(&comment);
    vorbis_comment_add_tag(&comment, "Encoder", "Jamtaba");

    qCDebug(jtNinjamVorbisEncoder) << "Initializing VorbisEncoder sampleRate:" << sampleRate << " channels: " << channels << " quality: " << quality;

    streamID = 0;
    isFirstEncoding = true;

    totalEncoded = 0;
}

//++++++++++++++++++++++++++++++++++++++++++
void VorbisEncoder::clearState(){
    ogg_stream_clear(&streamState);
    vorbis_block_clear(&block);
    vorbis_dsp_clear(&dspState);
    outBuffer.clear();
}

VorbisEncoder::~VorbisEncoder() {
    qCDebug(jtNinjamVorbisEncoder) << "ENCODER DESTRUCTOR! Thread:" <<  QThread::currentThreadId();
    if(initialized){
        clearState();
    }
    vorbis_comment_clear(&comment);
    vorbis_info_clear(&info);
}
//++++++++++++++++++++++++++++++++++++++++++
void VorbisEncoder::encodeFirstVorbisHeaders(){
    vorbis_analysis_init(&dspState, &info);
    vorbis_block_init(&dspState, &block);
    ogg_stream_init(&streamState, streamID++);

    //writing headers
    ogg_packet header, header_comm, header_code;
    vorbis_analysis_headerout(&dspState, &comment, &header, &header_comm, &header_code);
    ogg_stream_packetin(&streamState, &header);
    ogg_stream_packetin(&streamState, &header_comm);
    ogg_stream_packetin(&streamState, &header_code);

    //write ogg_page page in out buffer;
    while (true) {
        ogg_page page;
        int result = ogg_stream_flush(&streamState, &page);
        if (result == 0) break;
        //header and body
        outBuffer.append((const char*)page.header, page.header_len);//memcpy(buffer, page.header, page.header_len);
        outBuffer.append((const char*)page.body, page.body_len);//memcpy(buffer, page.body, page.body_len);
    }
    initialized = true;
    isFirstEncoding = false;//clear state in next encoding
}

//++++++++++++++++++++++++++++++++++++++++++
QByteArray VorbisEncoder::encode(const Audio::SamplesBuffer& samples) {
    //qCDebug(vorbisEncoder) << "Encoding " << samples.getFrameLenght() << " samples.";
    if (!initialized) {
        if(!isFirstEncoding){
            clearState();
        }
        encodeFirstVorbisHeaders();
    }
    else{
        outBuffer.clear();
    }

    if (samples.getFrameLenght() > 0) {//is not the end
        //copy the samples to encode to vorbis input buffer
        float** vorbisBuffer = vorbis_analysis_buffer(&dspState, samples.getFrameLenght());

        int channels = std::min(info.channels, samples.getChannels());
        for (int c = 0; c < channels; c++) {
            memcpy(vorbisBuffer[c], samples.getSamplesArray(c), samples.getFrameLenght() * sizeof(float));
        }
    }
    //lenght == 0 in the end of interval
    int result = vorbis_analysis_wrote(&dspState, samples.getFrameLenght()); // tell the library how much we actually submitted
    if(result != 0){
        qCCritical(jtNinjamVorbisEncoder) << "encoder error!";
    }

    //++++++++++++++++++++++++ encoding +++++++++
    bool endOfStream = false;
    while (vorbis_analysis_blockout(&dspState, &block))
    {
        vorbis_analysis(&block, NULL);
        vorbis_bitrate_addblock(&block);
        ogg_packet packet;
        while (vorbis_bitrate_flushpacket(&dspState, &packet))
        {
            ogg_stream_packetin(&streamState, &packet);
            while (!endOfStream)
            {
                ogg_page page;
                if (!ogg_stream_flush(&streamState, &page)) {
                    break;
                }

                //header and body
                outBuffer.append((const char*)page.header, page.header_len);//memcpy(buffer, page.header, page.header_len);
                outBuffer.append((const char*)page.body, page.body_len);//memcpy(buffer, page.body, page.body_len);
                qCDebug(jtNinjamVorbisEncoder) << "encoded bytes added to out buffer  bytes:" << outBuffer.size();
                if (ogg_page_eos(&page))
                    endOfStream = true;
            }
        }
    }
    return outBuffer;
}
//++++++++++++++++++++++++++++++++++++++++++
QByteArray VorbisEncoder::finishIntervalEncoding() {
    QByteArray data = encode(Audio::SamplesBuffer::ZERO_BUFFER);//pass zero samples to vorbis and finalize the encoding process
    initialized = false;
    return data;
}
//++++++++++++++++++++++++++++++++++++++++++
