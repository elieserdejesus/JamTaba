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

void NinjamControllerVST::startSynchronizedWithHost()
{
    if (waitingForHostSync)
        waitingForHostSync = false;
}

void NinjamControllerVST::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                                  int sampleRate)
{
    if (waitingForHostSync)// skip the ninjam processing if is waiting for host sync
        return;
    NinjamController::process(in, out, sampleRate);
}
