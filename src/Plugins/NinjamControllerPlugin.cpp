#include "NinjamControllerPlugin.h"
#include "MainControllerPlugin.h"

NinjamControllerPlugin::NinjamControllerPlugin(MainControllerPlugin *controller) :
    NinjamController(controller),
    waitingForHostSync(false)
{
}

void NinjamControllerPlugin::stopAndWaitForHostSync()
{
    if (!waitingForHostSync) {
        waitingForHostSync = true;
        reset(true);// discard the intervals but keep the most recent
    }
}

void NinjamControllerPlugin::disableHostSync()
{
    waitingForHostSync = false;
}

void NinjamControllerPlugin::startSynchronizedWithHost(qint32 startPosition)
{
    if (waitingForHostSync){
        waitingForHostSync = false;
        if (startPosition >= 0)
            intervalPosition = startPosition % samplesInInterval;
        else
            intervalPosition = samplesInInterval - qAbs(startPosition % samplesInInterval);
    }
}

void NinjamControllerPlugin::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                                  int sampleRate)
{
    if (waitingForHostSync)// skip the ninjam processing if is waiting for host sync
        return;
    NinjamController::process(in, out, sampleRate);
}
