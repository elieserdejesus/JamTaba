#ifndef __CLIPSORT_LOG_GENERATOR__
#define __CLIPSORT_LOG_GENERATOR__

#include "JamRecorder.h"

namespace Recorder {
class ClipSortLogGenerator : public JamMetadataWriter
{
public:
    void write(const Jam &jam) override;
    inline QString getWriterId() const override
    {
        return "ClipSortLogGenerator";
    }
    inline QString getWriterName() const override // Localised
    {
        return "Generate Reaper 'clipsort.log' file";
    }
};
}
#endif
