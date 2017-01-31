#include "AudioMixer.h"
#include "AudioNode.h"
#include <QDebug>
#include "Plugins.h"
#include "midi/MidiDriver.h"
#include <QMutexLocker>
#include "log/Logging.h"

using namespace Audio;

AudioMixer::AudioMixer(int sampleRate) :
    sampleRate(sampleRate)
{
}

void AudioMixer::addNode(AudioNode *node)
{
    nodes.append(node);
    resamplers.insert(node, new SamplesBufferResampler());
}

void AudioMixer::removeNode(AudioNode *node)
{
    SamplesBufferResampler *resampler = resamplers[node];
    nodes.removeOne(node);
    if (resampler) {
        resamplers[node] = nullptr;
        delete resampler;
    }
}

AudioMixer::~AudioMixer()
{
    qCDebug(jtAudio) << "Audio mixer destructor...";
    foreach (Audio::AudioNode *node, resamplers.keys())
        removeNode(node);
    qCDebug(jtAudio) << "Audio mixer destructor finished!";
}

void AudioMixer::process(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, std::vector<Midi::MidiMessage> &midiBuffer, bool attenuateAfterSumming)
{
    static int soloedBuffersInLastProcess = 0;
    // --------------------------------------
    bool hasSoloedBuffers = soloedBuffersInLastProcess > 0;
    soloedBuffersInLastProcess = 0;
    foreach (AudioNode *node, nodes) {
        bool canProcess = (!hasSoloedBuffers && !node->isMuted())
                          || (hasSoloedBuffers && node->isSoloed());
        if (canProcess) {
            node->processReplacing(in, out, sampleRate, midiBuffer);
        } else {// just discard the samples if node is muted, the internalBuffer is not copyed to out buffer
            static Audio::SamplesBuffer internalBuffer(2);
            internalBuffer.setFrameLenght(out.getFrameLenght());
            node->processReplacing(in, internalBuffer, sampleRate, midiBuffer);
        }
        if (node->isSoloed())
            soloedBuffersInLastProcess++;
    }

    if (attenuateAfterSumming) {
        int nodesConnected = nodes.size();
        if (nodesConnected > 1)// attenuate
            out.applyGain(1.0/nodesConnected, 0.0);
    }
}

// ++++++++++++++++++++++
