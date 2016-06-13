#ifndef __CLIPSORT_LOG_GENERATOR__
#define __CLIPSORT_LOG_GENERATOR__

#include "JamRecorder.h"
#include "QCoreApplication"

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
        return QCoreApplication::translate("Recorder::ClipSortLogGenerator", "Generate Reaper 'clipsort.log' file");
    }
    inline void setJamDir(QString newJamName, QString recordBasePath) override {
        QDir parentDir(QDir(recordBasePath).absoluteFilePath(newJamName));
        parentDir.mkpath("Reaper/clipsort");
        this->clipsortPath = parentDir.absoluteFilePath("Reaper/clipsort");
    }
    QString getAudioAbsolutePath(QString audioFileName) override;
private:
    QString clipsortPath;
};

}// namespace

#endif
