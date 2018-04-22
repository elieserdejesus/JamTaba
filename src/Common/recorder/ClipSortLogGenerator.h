#ifndef __CLIPSORT_LOG_GENERATOR__
#define __CLIPSORT_LOG_GENERATOR__

#include "JamRecorder.h"
#include "QCoreApplication"

namespace recorder {

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
        return QCoreApplication::translate("Recorder::ClipSortLogGenerator", "Generate Reaper 'clipsort.log' file");
    }

    void setJamDir(const QString &newJamName, const QString &recordBasePath) override;

    QString getAudioAbsolutePath(const QString &audioFileName) override;
    QString getVideoAbsolutePath(const QString &videoFileName) override;

private:
    QString clipsortPath;

};

} // namespace

#endif
