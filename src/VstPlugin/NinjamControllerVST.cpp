#include "NinjamControllerVST.h"
#include "MainControllerVST.h"

NinjamControllerVST::NinjamControllerVST(MainControllerVST *controller) :
    NinjamController(controller),
    waitingForHostSync(false)
{
}

void NinjamControllerVST::stopAndWaitForHostSync()
{
    if (!waitingForHostSync) {
        waitingForHostSync = true;
        reset(true);// discard the intervals but keep the most recent
    }
}

void NinjamControllerVST::disableHostSync()
{
    waitingForHostSync = false;
}

void NinjamControllerVST::startSynchronizedWithHost(qint32 startPosition)
{
    if (waitingForHostSync){
        waitingForHostSync = false;
        if (startPosition >= 0)
            intervalPosition = startPosition % samplesInInterval;
        else
            intervalPosition = samplesInInterval - qAbs(startPosition % samplesInInterval);
    }
}

void NinjamControllerVST::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                                  int sampleRate)
{
    if (waitingForHostSync)// skip the ninjam processing if is waiting for host sync
        return;
    NinjamController::process(in, out, sampleRate);
}
