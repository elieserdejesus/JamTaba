#ifndef _JTBA_AUDIO_ENCODER_
#define _JTBA_AUDIO_ENCODER_

#include <QByteArray>

#include "audio/core/SamplesBuffer.h"

/**
 * @brief The 'interface' for audio encoders
 */
class AudioEncoder
{
    public:
        virtual ~AudioEncoder(){}
        virtual QByteArray encode(const Audio::SamplesBuffer &audioBuffer) = 0;
        virtual QByteArray finishIntervalEncoding() = 0;
        virtual int getChannels() const = 0;
        virtual int getSampleRate() const = 0;
};

#endif
