#ifndef _VORBIS_
#define _VORBIS_

namespace vorbis
{

    // these vorbis quality values are discussed here: https://github.com/elieserdejesus/JamTaba/issues/456#issuecomment-226920734
    const float EncoderQualityLow    = -0.1f;  // ~48 – ~64 kbps
    const float EncoderQualityNormal =  0;     // ~64 – ~80 kbps.
    const float EncoderQualityHigh   =  0.3f;  // ~112 – ~128 kbps. In ogg vorbis 112 Kbps is better than 128 kbps mp3


} // namespace

#endif
