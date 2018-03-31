#ifndef __REAPER_PROJECT_GENERATOR__
#define __REAPER_PROJECT_GENERATOR__

#include "JamRecorder.h"
#include "QCoreApplication"

namespace recorder {

class ReaperProjectGenerator : public JamMetadataWriter
{

public:
    void write(const Jam &jam) override;

    inline QString getWriterId() const override
    {
        return "ReaperProjectGenerator";
    }

    inline QString getWriterName() const override // Localized
    {
        return QCoreApplication::translate("Recorder::ReaperProjectGenerator", "Generate Reaper Project file (RPP)");
    }

    void setJamDir(const QString &newJamName, const QString &recordBasePath) override;

    QString getAudioAbsolutePath(const QString &audioFileName) override;
    QString getVideoAbsolutePath(const QString &videoFileName) override;

private:
    static QString buildTrackName(const QString &userName, quint8 channelIndex);
    QString rppPath;

};

} // namespace

#endif
