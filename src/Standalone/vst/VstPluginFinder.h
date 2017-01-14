#ifndef VSTPLUGINFINDER_H
#define VSTPLUGINFINDER_H

#include "PluginFinder.h"
#include "audio/core/PluginDescriptor.h"

namespace audio {

class VSTPluginFinder : public PluginFinder
{

public:
    VSTPluginFinder();
    virtual ~VSTPluginFinder();

protected:
    QString getScannerExecutablePath() const override;

    void handleScanningStart(const QString &scannedLine) override;
    void handleScanningFinished(const QString &scannedLine) override;

private:
    QProcess scanProcess;
    QString lastScannedPlugin;// used to recover the last plugin path when the scanner process crash

    Audio::PluginDescriptor getPluginDescriptor(const QFileInfo &f);

};

} //namespace

#endif // VSTPLUGINFINDER_H
