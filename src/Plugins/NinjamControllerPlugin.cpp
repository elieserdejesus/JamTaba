#include "NinjamControllerPlugin.h"
#include "MainControllerPlugin.h"

#include "audio/MetronomeTrackNode.h"
#include "audio/NinjamTrackNode.h"

NinjamControllerPlugin::NinjamControllerPlugin(MainControllerPlugin *controller) :
    NinjamController(controller),
    waitingForHostSync(false),
    controller(controller)
{
    //
}

void NinjamControllerPlugin::stopAndWaitForHostSync()
{
    if (!waitingForHostSync) {
        waitingForHostSync = true;

        reset(true);// discard the intervals but keep the most recent

        deactivateNinjamAudioNodes(); // metronome and ninjam audio nodes will not be rendered
    }
}

void NinjamControllerPlugin::deactivateNinjamAudioNodes()
{
    metronomeTrackNode->deactivate();

    for (NinjamTrackNode *node : trackNodes)
        node->deactivate();
}

void NinjamControllerPlugin::activateNinjamAudioNodes()
{
    metronomeTrackNode->activate();

    for (NinjamTrackNode *node : trackNodes)
        node->activate();
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

        activateNinjamAudioNodes();
    }
}

void NinjamControllerPlugin::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate)
{
    if (!waitingForHostSync)
        NinjamController::process(in, out, sampleRate);
    else
        controller->doAudioProcess(in, out, sampleRate); // will process input only, ninjam related audio nodes will not be rendered, interval will not progress, etc.
}
