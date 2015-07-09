#include "AudioMixer.h"
#include "AudioNode.h"
#include <QDebug>
#include "plugins.h"
#include "../vst/VstPlugin.h"
#include "../midi/MidiDriver.h"
#include <QMutexLocker>

using namespace Audio;

AudioMixer::AudioMixer(int sampleRate)
    : inputNode(new LocalInputAudioNode()),
      sampleRate(sampleRate)
{

    //disconnect to test
    //nodes.append(inputNode);

}

void AudioMixer::addNode(AudioNode *node){
    QMutexLocker locker(&mutex);
    nodes.append(node);
    resamplers.insert(node, new SamplesBufferResampler());
}

void AudioMixer::removeNode(AudioNode *node){
    QMutexLocker locker(&mutex);
    SamplesBufferResampler* resampler = resamplers[node];
    nodes.removeOne(node);
    delete resampler;
}

AudioMixer::~AudioMixer(){
    delete inputNode;
    inputNode = nullptr;
}

void AudioMixer::process(SamplesBuffer &in, SamplesBuffer &out, bool attenuateAfterSumming){
    static int soloedBuffersInLastProcess = 0;
    //--------------------------------------
    bool hasSoloedBuffers = soloedBuffersInLastProcess > 0;
    soloedBuffersInLastProcess = 0;
    QMutexLocker locker(&mutex);
    foreach (AudioNode* node , nodes) {
        bool canProcess = (!hasSoloedBuffers && !node->isMuted()) || (hasSoloedBuffers && node->isSoloed());
        if(canProcess ){
            if(node->needResamplingFor(sampleRate)){
                //qDebug() << node->getSampleRate() << "=>" << sampleRate;
                //read N samples from the node
                static SamplesBuffer tempBuffer(2);
                tempBuffer.zero();
                int samplesToGrabFromNode = node->getInputResamplingLength(sampleRate, out.getFrameLenght());

                tempBuffer.setFrameLenght(samplesToGrabFromNode);
                node->processReplacing(in, tempBuffer);

                const SamplesBuffer& resampledBuffer = resamplers[node]->resample(tempBuffer, node->getSampleRate(), out.getFrameLenght(), this->sampleRate);
                out.add(resampledBuffer);

                int discardedSamples = resampledBuffer.getFrameLenght() - out.getFrameLenght();
                if(discardedSamples != 0){
                    qDebug() << "discarded: " << discardedSamples;
                }

                if(resampledBuffer.getFrameLenght() != out.getFrameLenght()){
                    qDebug() << "resampled buffer size problem: " << resampledBuffer.getFrameLenght() << " != " << out.getFrameLenght();
                }
            }
            else{
                node->processReplacing(in, out);
            }
        }
        else{//just discard the samples if node is muted, the internalBuffer is not copyed to out buffer
            static Audio::SamplesBuffer internalBuffer(2);
            internalBuffer.setFrameLenght(out.getFrameLenght());
            node->processReplacing(in, internalBuffer);
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


