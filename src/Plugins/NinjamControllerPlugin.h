#ifndef NINJAM_CONTROLLER_VST_H
#define NINJAM_CONTROLLER_VST_H

#include "NinjamController.h"

class MainControllerPlugin;

class NinjamControllerPlugin : public controller::NinjamController
{

public:
    explicit NinjamControllerPlugin(MainControllerPlugin *controller);

    bool isWaitingForHostSync() const;

    void startSynchronizedWithHost(qint32 startPosition);
    void stopAndWaitForHostSync();
    void disableHostSync();

    void process(const audio::SamplesBuffer &in, audio::SamplesBuffer &out, int sampleRate);

private:

    MainControllerPlugin *controller; // just a casted version of mainController instance

    bool waitingForHostSync;

    void deactivateAudioNodes(); // ninjam related audio nodes will not be rendered
    void activateAudioNodes();
};


inline bool NinjamControllerPlugin::isWaitingForHostSync() const
{
    return waitingForHostSync;
}

#endif
