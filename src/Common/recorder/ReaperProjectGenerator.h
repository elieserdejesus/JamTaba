#ifndef __REAPER_PROJECT_GENERATOR__
#define __REAPER_PROJECT_GENERATOR__

#include "JamRecorder.h"
#include "QCoreApplication"

namespace Recorder {
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
    void setJamDir(QString newJamName, QString recordBasePath) override;
    QString getAudioAbsolutePath(QString audioFileName) override;
private:
    static QString buildTrackName(const QString &userName, quint8 channelIndex);
    QString rppPath;
};
}

#endif
