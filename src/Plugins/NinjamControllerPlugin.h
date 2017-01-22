#ifndef NINJAM_CONTROLLER_VST_H
#define NINJAM_CONTROLLER_VST_H

#include "NinjamController.h"

class MainControllerPlugin;

class NinjamControllerPlugin : public Controller::NinjamController
{
public:
    explicit NinjamControllerPlugin(MainControllerPlugin *controller);

    bool isWaitingForHostSync() const;

    void startSynchronizedWithHost(qint32 startPosition);
    void stopAndWaitForHostSync();
    void disableHostSync();

    void process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate);

private:

    MainControllerPlugin *controller; // just a casted version of mainController instance

    bool waitingForHostSync;

    void deactivateNinjamAudioNodes(); // ninjam related audio nodes will not be rendered
    void activateNinjamAudioNodes();
};


inline bool NinjamControllerPlugin::isWaitingForHostSync() const
{
    return waitingForHostSync;
}

#endif
