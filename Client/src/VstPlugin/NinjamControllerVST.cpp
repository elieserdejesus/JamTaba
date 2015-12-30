#include "NinjamControllerVST.h"
#include "MainControllerVST.h"

NinjamControllerVST::NinjamControllerVST(MainControllerVST *controller) :
    NinjamController(controller),
    waitingForHostSync(false)
{
}

void NinjamControllerVST::waitForHostSync()
{
    waitingForHostSync = true;
    reset();
}

void NinjamControllerVST::syncWithHost()
{
    waitingForHostSync = false;
}

void NinjamControllerVST::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                                  int sampleRate)
{
    if (waitingForHostSync)// skip the ninjam processing if is waiting for sync
        return;
    NinjamController::process(in, out, sampleRate);
}
