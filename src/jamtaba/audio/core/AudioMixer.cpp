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
    //inputNode->connect(*mainOutNode);
    //inputNode->setGain(1);

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

void AudioMixer::process(SamplesBuffer &in, SamplesBuffer &out){
    static int soloedBuffersInLastProcess = 0;
    //--------------------------------------
    bool hasSoloedBuffers = soloedBuffersInLastProcess > 0;
    soloedBuffersInLastProcess = 0;
    QMutexLocker locker(&mutex);
    foreach (AudioNode* node , nodes) {
        bool canProcess = (!hasSoloedBuffers && !node->isMuted()) || (hasSoloedBuffers && node->isSoloed());
        if(canProcess){
            if(node->needResamplingFor(sampleRate)){
                //read N samples from the node
                static SamplesBuffer tempOutBuffer(2, 4096 * 2);
                tempOutBuffer.zero();
                double resampleFactor = (double)node->getSampleRate()/(double)sampleRate;
                int samplesToGrabFromNode = (int)(out.getFrameLenght() * resampleFactor);
                //samplesToGrabFromNode++;
                tempOutBuffer.setFrameLenght(samplesToGrabFromNode);
                node->processReplacing(in, tempOutBuffer);

                //resample the readed samples to match the current mixer sample rate
                //assert(resamplers[node] != nullptr);
                SamplesBufferResampler* resampler = resamplers[node];
                const SamplesBuffer& resampledBuffer = resampler->resample(tempOutBuffer, out.getFrameLenght(), node->getSampleRate(), this->sampleRate);
                out.add(resampledBuffer);
                if(resampledBuffer.getFrameLenght() != out.getFrameLenght()){
                    qDebug() << resampledBuffer.getFrameLenght();
                }
            }
            else{
                node->processReplacing(in, out);
            }
        }
        else{//just discard the samples if node is muted, the internalBuffer is not copyed to out buffer
            static Audio::SamplesBuffer internalBuffer(2, 4096);
            internalBuffer.setFrameLenght(out.getFrameLenght());
            node->processReplacing(in, internalBuffer);
        }
        if(node->isSoloed()){
            soloedBuffersInLastProcess++;
        }
    }

    int nodesConnected = nodes.size();
    if(nodesConnected > 1){//attenuate
        out.applyGain( 1.0/nodesConnected );
    }

}

//++++++++++++++++++++++


