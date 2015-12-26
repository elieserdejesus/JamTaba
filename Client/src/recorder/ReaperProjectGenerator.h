#ifndef __REAPER_PROJECT_GENERATOR__
#define __REAPER_PROJECT_GENERATOR__

#include "JamRecorder.h"

namespace Recorder {
class ReaperProjectGenerator : public JamMetadataWriter
{
public:
    void write(Jam jam);

private:
    static QString buildTrackName(QString userName, quint8 channelIndex);
};
}

#endif
