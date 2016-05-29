#ifndef __CLIPSORT_LOG_GENERATOR__
#define __CLIPSORT_LOG_GENERATOR__

#include "JamRecorder.h"

namespace Recorder {
class ClipSortLogGenerator : public JamMetadataWriter
{
public:
    void write(const Jam &jam) override;
};
}
#endif
