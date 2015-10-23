#include "AudioMixer.h"
#include "AudioNode.h"
#include <QDebug>
#include "plugins.h"
#if _WIN32
    #include "../vst/vstplugin.h"
#endif
#include "../midi/MidiDriver.h"
#include <QMutexLocker>

using namespace Audio;

AudioMixer::AudioMixer(int sampleRate)
    : //inputNode(new LocalInputAudioNode()),
      sampleRate(sampleRate)
{

    //disconnect to test
    //nodes.append(inputNode);

}

void AudioMixer::addNode(AudioNode *node){
    //QMutexLocker locker(&mutex);
    nodes.append(node);
    resamplers.insert(node, new SamplesBufferResampler());
}

void AudioMixer::removeNode(AudioNode *node){
    //QMutexLocker locker(&mutex);
    SamplesBufferResampler* resampler = resamplers[node];
    nodes.removeOne(node);
    if(resampler){
        resamplers[node] = nullptr;
        delete resampler;
    }
}

AudioMixer::~AudioMixer(){
    qDebug() << "Audio mixer destructor...";
//    foreach (SamplesBufferResampler* resampler, resamplers.values()) {
//        delete resampler;
//    }
//    resamplers.clear();
    foreach (Audio::AudioNode* node, resamplers.keys()) {
        removeNode(node);
    }
    qDebug() << "Audio mixer destructor finished!";
}

void AudioMixer::process(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer& midiBuffer, bool attenuateAfterSumming){
    static int soloedBuffersInLastProcess = 0;
    //--------------------------------------
    bool hasSoloedBuffers = soloedBuffersInLastProcess > 0;
    soloedBuffersInLastProcess = 0;
    //QMutexLocker locker(&mutex);
    foreach (AudioNode* node , nodes) {
        bool canProcess = (!hasSoloedBuffers && !node->isMuted()) || (hasSoloedBuffers && node->isSoloed());
        if(canProcess ){
                node->processReplacing(in, out, sampleRate, midiBuffer);
        }
        else{//just discard the samples if node is muted, the internalBuffer is not copyed to out buffer
            static Audio::SamplesBuffer internalBuffer(2);
            internalBuffer.setFrameLenght(out.getFrameLenght());
            node->processReplacing(in, internalBuffer, sampleRate, midiBuffer);
        }
        if(node->isSoloed()){
            soloedBuffersInLastProcess++;
        }
    }

    if(attenuateAfterSumming){
        int nodesConnected = nodes.size();
        if(nodesConnected > 1){//attenuate
            out.applyGain( 1.0/nodesConnected );
        }
    }

}

//++++++++++++++++++++++


