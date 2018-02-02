#include "Plugins.h"
#include "AudioDriver.h"
#include "midi/MidiDriver.h"
#include "SamplesBuffer.h"
#include <QDebug>
#include <QDialog>
#include <QMutexLocker>

using audio::Plugin;
using audio::PluginDescriptor;
using audio::JamtabaDelay;
using audio::SamplesBuffer;

Plugin::Plugin(const PluginDescriptor &pluginDescriptor) :
    name(pluginDescriptor.getName()),
    editorWindow(nullptr),
    descriptor(pluginDescriptor)
{
    //
}

void Plugin::closeEditor()
{
    if (editorWindow) {
        editorWindow->setVisible(false);
        editorWindow->deleteLater();
        editorWindow = nullptr;
    }
}

void Plugin::editorDialogFinished()
{
    closeEditor();
}

Plugin::~Plugin()
{
    closeEditor();
}

// ++++++++++++++++++++++++++++

const int JamtabaDelay::MAX_DELAY_IN_SECONDS = 3;

JamtabaDelay::JamtabaDelay(int sampleRate) :
    Plugin(PluginDescriptor("Delay", PluginDescriptor::Native_Plugin, "JamTaba")),
    delayTimeInMs(0),
    internalBuffer(new SamplesBuffer(2)) // 2 channels, 3 seconds delay
{
    internalIndex = 0;
    feedbackGain = 0.3f;// feedback start in this gain
    level = 1;
    setSampleRate(sampleRate);
}

QByteArray JamtabaDelay::getSerializedData() const
{
    return QByteArray();
}

void JamtabaDelay::restoreFromSerializedData(const QByteArray &data)
{
    Q_UNUSED(data)
}

void JamtabaDelay::setSampleRate(int newSampleRate)
{
    this->sampleRate = newSampleRate;
    delayTimeInSamples = this->sampleRate / 2; // half second
    internalBuffer->setFrameLenght(delayTimeInSamples);
}

JamtabaDelay::~JamtabaDelay()
{
    delete internalBuffer;
    // delete mutex;
}

void JamtabaDelay::start()
{
    //
}

void JamtabaDelay::process(const SamplesBuffer &in, SamplesBuffer &out, std::vector<midi::MidiMessage> &midiBuffer)
{
    Q_UNUSED(midiBuffer)
    Q_UNUSED(in)
    Q_UNUSED(out)
// if(isBypassed()){
// return;
// }
// float bufferValue = 0, internalValue = 0;
// for (int s = 0; s < in.getFrameLenght(); ++s) {
// for (int c = 0; c < in.getChannels(); ++c) {
// bufferValue = in.get(c, s);
// internalValue = internalBuffer->get(c, internalIndex);
// in.add(c, s, internalValue * level);//copy the internal sample to out buffer
// internalBuffer->set(c, internalIndex, bufferValue + internalValue * feedbackGain  ); //acumulate the sample in internal buffer
// }
// internalIndex = (internalIndex + 1) % internalBuffer->getFrameLenght();
// }
}

void JamtabaDelay::setDelayTime(int delayTimeInMs)
{
    if (delayTimeInMs > 0) {
        if (delayTimeInMs > MAX_DELAY_IN_SECONDS * sampleRate)
            delayTimeInMs = MAX_DELAY_IN_SECONDS * sampleRate;

        this->delayTimeInMs = delayTimeInMs;
        this->delayTimeInSamples = delayTimeInMs/1000.0 * sampleRate;
        this->internalBuffer->setFrameLenght(delayTimeInSamples);
    }
}

void JamtabaDelay::setFeedback(float feedback)
{
    this->feedbackGain = feedback;
}

void JamtabaDelay::setLevel(float level)
{
    if (level >= 0)
        this->level = level;
}

void JamtabaDelay::openEditor(const QPoint &p)
{
    Q_UNUSED(p);
}
