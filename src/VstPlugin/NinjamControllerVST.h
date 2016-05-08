#ifndef NINJAM_CONTROLLER_VST_H
#define NINJAM_CONTROLLER_VST_H

#include "NinjamController.h"

class MainControllerVST;

class NinjamControllerVST : public Controller::NinjamController
{
public:
    explicit NinjamControllerVST(MainControllerVST *controller);

    inline bool isWaitingForHostSync() const
    {
        return waitingForHostSync;
    }

    void startSynchronizedWithHost(qint32 startPosition);
    void stopAndWaitForHostSync();
    void disableHostSync();

    void process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate);

private:
    bool waitingForHostSync;
};


#endif
