#ifndef __REAPER_PROJECT_GENERATOR__
#define __REAPER_PROJECT_GENERATOR__

#include "JamRecorder.h"

namespace Recorder {
class ReaperProjectGenerator : public JamMetadataWriter
{
public:
    void write(const Jam &jam) override;

private:
    static QString buildTrackName(const QString &userName, quint8 channelIndex);
};
}

#endif
