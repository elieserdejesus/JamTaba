#include "VorbisEncoder.h"
#include <QDebug>
#include <ctime>
#include <QThread>

VorbisEncoder::VorbisEncoder()
    :initialized(false)
{
    init(1, 44100);
}

VorbisEncoder::VorbisEncoder(int channels, int sampleRate):
    //finishIntervalRequested(false),
    initialized(false)
    //endOfStream(false)
{
    init(channels, sampleRate);
}

void VorbisEncoder::init(int channels, int sampleRate){
    //vorbis init
    //qDebug() << "Encoder init Thread:" << QThread::currentThreadId();
    vorbis_info_init(&info);

    if(vorbis_encode_init_vbr(&info, (long) channels, (long) sampleRate, 0.4) != 0){
        qCritical() << "vorbis encoder initialization error!";
    }
    vorbis_comment_init(&comment);
    vorbis_comment_add_tag(&comment, "Encoder", "Jamtaba");

    totalEncoded = 0;
}

//++++++++++++++++++++++++++++++++++++++++++
VorbisEncoder::~VorbisEncoder() {
    //qDebug() << "ENCODER DESTRUCTOR! Thread:" <<  QThread::currentThreadId();;
    ogg_stream_clear(&streamState);
    vorbis_block_clear(&block);
    vorbis_dsp_clear(&dspState);
    vorbis_comment_clear(&comment);
    vorbis_info_clear(&info);

}
//++++++++++++++++++++++++++++++++++++++++++
void VorbisEncoder::initializeVorbis() {
    if (!initialized) {
        totalEncoded = 0;
        initialized = true;
        vorbis_analysis_init(&dspState, &info);
        vorbis_block_init(&dspState, &block);
        srand((unsigned int) time(NULL));
        ogg_stream_init(&streamState, rand());

        //writing headers
        ogg_packet header, header_comm, header_code;
        vorbis_analysis_headerout(&dspState, &comment, &header, &header_comm, &header_code);
        ogg_stream_packetin(&streamState, &header);
        ogg_stream_packetin(&streamState, &header_comm);
        ogg_stream_packetin(&streamState, &header_code);

        //write ogg_page page in out buffer;
        //char buffer[4096];
        while (true) {
            int result = ogg_stream_flush(&streamState, &page);
            if (result == 0) break;
            //header
            outBuffer.append((const char*)page.header, page.header_len);//memcpy(buffer, page.header, page.header_len);
            //body

            outBuffer.append((const char*)page.body, page.body_len);//memcpy(buffer, page.body, page.body_len);
        }

    }
}
//++++++++++++++++++++++++++++++++++++++++++
QByteArray VorbisEncoder::encode(const Audio::SamplesBuffer& samples) {
    //qDebug() << "Encoder::encode() Thread:" << QThread::currentThreadId();
    if (!initialized) {
        initializeVorbis();
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
        qCritical() << "encoder error!";
    }
    //    if (samples.getFrameLenght() <= 0) {
//        finishIntervalRequested = true;
//    }
    //++++++++++++++++++++++++ encoding +++++++++
    bool endOfStream = false;
    while (vorbis_analysis_blockout(&dspState, &block)) {
        vorbis_analysis(&block, NULL);
        vorbis_bitrate_addblock(&block);

        while (vorbis_bitrate_flushpacket(&dspState, &packet)) {

            ogg_stream_packetin(&streamState, &packet);

            while (!endOfStream) {

                if (!ogg_stream_pageout(&streamState, &page)) {
                    break;
                }

                //header
                outBuffer.append((const char*)page.header, page.header_len);//memcpy(buffer, page.header, page.header_len);
                //body
                outBuffer.append((const char*)page.body, page.body_len);//memcpy(buffer, page.body, page.body_len);

                if (ogg_page_eos(&page)) {
                    endOfStream = 1;
                }
            }
        }
    }//end of encoding loop
    return outBuffer;//return the byte Array
}
//++++++++++++++++++++++++++++++++++++++++++
QByteArray VorbisEncoder::finishIntervalEncoding() {
    QByteArray data = encode(Audio::SamplesBuffer::ZERO_BUFFER);//pass zero samples to vorbis and finalize the encoding process
    initialized = false;
    return data;
}
//++++++++++++++++++++++++++++++++++++++++++

