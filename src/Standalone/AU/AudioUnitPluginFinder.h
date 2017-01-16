#ifndef AUDIOUNITPLUGINFINDER_H
#define AUDIOUNITPLUGINFINDER_H

#include "PluginFinder.h"

namespace audio {

class AudioUnitPluginFinder : public PluginFinder
{
public:
    AudioUnitPluginFinder();

protected:
    QString getScannerExecutablePath() const override;

    void handleScanningStart(const QString &scannedLine) override;
    void handleScanningFinished(const QString &scannedLine) override;
};

} // namespace

#endif // AUDIOUNITPLUGINFINDER_H
