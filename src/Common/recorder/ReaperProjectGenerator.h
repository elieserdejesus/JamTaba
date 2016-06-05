#ifndef __REAPER_PROJECT_GENERATOR__
#define __REAPER_PROJECT_GENERATOR__

#include "JamRecorder.h"

namespace Recorder {
class ReaperProjectGenerator : public JamMetadataWriter
{
public:
    void write(const Jam &jam) override;
    inline QString getWriterId() const override
    {
        return "ReaperProjectGenerator";
    }
    inline QString getWriterName() const override // Localised
    {
        return "Generate Reaper Project file (RPP)";
    }

private:
    static QString buildTrackName(const QString &userName, quint8 channelIndex);
};
}

#endif
