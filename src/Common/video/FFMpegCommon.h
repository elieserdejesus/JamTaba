#ifndef FFMPEGCOMMON_H
#define FFMPEGCOMMON_H

#include <string>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/imgutils.h>
}

// these defines are necessary to compile in MSVC 2013
#define __STDC_CONSTANT_MACROS
//#define snprintf(buf,len, format,...) _snprintf_s(buf, len,len, format, __VA_ARGS__)

// a magic macro used to convert QImage to FFmpeg image format
#define RGBtoYUV(r, g, b, y, u, v) \
  y = (uint8_t)(((int)30*r + (int)59*g + (int)11*b)/100); \
  u = (uint8_t)(((int)-17*r - (int)33*g + (int)50*b + 12800)/100); \
  v = (uint8_t)(((int)50*r - (int)42*g - (int)8*b + 12800)/100);

// FFMpeg is a C lib, we need use extern 'C' to include the FFMpeg headers
extern "C" {
    #include <libavutil/opt.h>
    #include <libavutil/avassert.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
}

// a little workaround to use the FFMpeg av_err2str macro
static const std::string av_make_error_string(int errnum)
{
    char errbuf[AV_ERROR_MAX_STRING_SIZE];
    av_strerror(errnum, errbuf, AV_ERROR_MAX_STRING_SIZE);
    return (std::string)errbuf;
}

#undef av_err2str
#define av_err2str(errnum) av_make_error_string(errnum).c_str()

// we are using Buffered I/O in FFMpeg functions. This is the buffer size.
#define FFMPEG_BUFFER_SIZE 4096// 32768

#endif // FFMPEGCOMMON_H
