#include "JamTabaAUPlugin.h"


////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class MainControllerAU : public MainControllerPlugin
{
public:
    MainControllerAU(const Persistence::Settings &settings, JamTabaPlugin *plugin)
        : MainControllerPlugin(settings, plugin)
    {

    }

    QString getJamtabaFlavor() const override
    {
        return "AU Plugin";
    }

    void resizePluginEditor(int newWidth, int newHeight) override
    {

    }
};


JamTabaAUPlugin::JamTabaAUPlugin()
    :JamTabaPlugin(2, 2),
    listener(new Listener(this))
{
    
}


void JamTabaAUPlugin::process(const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess)
{
    if (inBuffer.mNumberBuffers != inputBuffer.getChannels())
        return;
    
    if (!controller)
        return;
    
    if (controller->isPlayingInNinjamRoom()) {
        
        // ++++++++++ sync ninjam BPM with host BPM ++++++++++++
        // ask timeInfo to VST host
        
//        timeInfo = getTimeInfo(kVstTransportPlaying | kVstTransportChanged | kVstTempoValid);
//        if (transportStartDetectedInHost()) {// user pressing play/start in host?
//            NinjamControllerPlugin *ninjamController = controller->getNinjamController();
//            Q_ASSERT(ninjamController);
//            if (ninjamController->isWaitingForHostSync())
//                ninjamController->startSynchronizedWithHost(getStartPositionForHostSync());
//        }
    }
    
    // ++++++++++ Audio processing +++++++++++++++
    inputBuffer.setFrameLenght(inFramesToProcess);
    for (int c = 0; c < inputBuffer.getChannels(); ++c)
        memcpy(inputBuffer.getSamplesArray(c), inBuffer.mBuffers[c].mData, sizeof(float) * inFramesToProcess);
    
    outputBuffer.setFrameLenght(inFramesToProcess);
    outputBuffer.zero();
    
    controller->process(inputBuffer, outputBuffer, getSampleRate());
    
    int channels = outputBuffer.getChannels();
    for (int c = 0; c < channels; ++c)
        memcpy(outBuffer.mBuffers[c].mData, outputBuffer.getSamplesArray(c), sizeof(float) * inFramesToProcess);
    
    // ++++++++++++++++++++++++++++++
    hostWasPlayingInLastAudioCallBack = hostIsPlaying();
}

MainControllerPlugin * JamTabaAUPlugin::createPluginMainController(const Persistence::Settings &settings, JamTabaPlugin *plugin) const
{
    return new MainControllerAU(settings, plugin);
}

qint32 JamTabaAUPlugin::getStartPositionForHostSync() const
{
    return 0; // TODO implementar
}

bool JamTabaAUPlugin::hostIsPlaying() const
{
    return true; //TODO implementar
}

int JamTabaAUPlugin::getHostBpm() const
{
    return 120; //TODO implementar
}

float JamTabaAUPlugin::getSampleRate() const
{
    return 44100; // TODO implementar
}

QString JamTabaAUPlugin::getHostName()
{
    return "Implementar!"; //http://lists.apple.com/archives/coreaudio-api/2007/Mar/msg00167.html
}