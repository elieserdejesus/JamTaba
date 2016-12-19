#include "JamTabaVSTPlugin.h"
#include "MainControllerVST.h"
#include "NinjamControllerPlugin.h"
#include "log/Logging.h"
#include "Editor.h"

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
    if (!JamTabaPlugin::pluginIsInitialized())// avoid multiple instances inside a DAW.
        return new JamTabaVSTPlugin(audioMaster);
    return nullptr;
}

JamTabaVSTPlugin::JamTabaVSTPlugin(audioMasterCallback audioMaster) :
    JamTabaPlugin(DEFAULT_INPUTS * 2, DEFAULT_OUTPUTS * 2),
    AudioEffectX(audioMaster, 0, 0),
    listEvnts(0),
    timeInfo(nullptr)
{
    qCDebug(jtVstPlugin) << "Plugin constructor...";
    setNumInputs(DEFAULT_INPUTS*2);
    setNumOutputs(DEFAULT_OUTPUTS*2);

    canProcessReplacing(true);
    programsAreChunks(false);
    vst_strncpy(programName, "Default", kVstMaxProgNameLen);    // default program name

    canDoubleReplacing(false);

    setEditor(new VstEditor(this));

    // suspend();
    qCDebug(jtVstPlugin) << "Plugin constructor done.";
}

QString JamTabaVSTPlugin::getHostName()
{
    char tempChars[kVstMaxProductStrLen];
    getHostProductString(tempChars);
    QString lowerCaseHostName = QString::fromUtf8(tempChars).toLower();

    if (lowerCaseHostName.startsWith("cakewalk") || lowerCaseHostName.startsWith("fx teleport"))
        return "Sonar";

    if (lowerCaseHostName.contains("reaper"))
        return "Reaper";// in x64 machines reaper host name is Reaperb32 if you are running a 32 bits plugin

    if (lowerCaseHostName.contains("cubase"))
        return "Cubase";// cubase return "cubase vst" as host name

    if (lowerCaseHostName == "presonus vst2 host")
        return "Studio One";

    return QString::fromUtf8(tempChars);// preserve original case
}

bool JamTabaVSTPlugin::hostIsPlaying() const
{
    if (timeInfo)
        return (timeInfo->flags & kVstTransportPlaying) != 0;
    return false;
}

int JamTabaVSTPlugin::getHostBpm() const
{
    if (timeInfo)
        return (int)timeInfo->tempo;
    return 0;
}

VstInt32 JamTabaVSTPlugin::fxIdle()
{
    QApplication::processEvents();
    return 0;
}

bool JamTabaVSTPlugin::needIdle()
{
    return true;
}

JamTabaVSTPlugin::~JamTabaVSTPlugin ()
{
    qCDebug(jtVstPlugin) << "VST Plugin destructor";
}

void JamTabaVSTPlugin::open()
{
    qCDebug(jtVstPlugin) << "Plugin open()";
}

void JamTabaVSTPlugin::close()
{
    qCDebug(jtVstPlugin) << "JamtabaPlugin::close()";
    if (editor) {
        if (editor->isOpen())
            editor->close();
        dynamic_cast<VstEditor *>(editor)->deleteMainWindow();
    }

    JamTabaPlugin::close(); // super class
}

VstInt32 JamTabaVSTPlugin::getNumMidiInputChannels()
{
    return 0;
}

VstInt32 JamTabaVSTPlugin::getNumMidiOutputChannels()
{
    return 0;
}

bool JamTabaVSTPlugin::getEffectName(char *name)
{
    if (!name)
        return false;
    vst_strncpy(name, "Jamtaba 2", kVstMaxEffectNameLen);
    return true;
}

bool JamTabaVSTPlugin::getProductString(char *text) const
{
    if (!text)
        return false;
    vst_strncpy(text, "Jamtaba 2", kVstMaxProductStrLen);
    return true;
}

bool JamTabaVSTPlugin::getVendorString(char *text) const
{
    if (!text)
        return false;
    vst_strncpy(text, "Jamtaba", kVstMaxVendorStrLen);
    return true;
}

VstInt32 JamTabaVSTPlugin::getVendorVersion()
{
    return 2;
}

VstInt32 JamTabaVSTPlugin::canDo(char *text)
{
    if (!text)
        return 0;
    if (!strcmp(text, "receiveVstTimeInfo"))
        return 1;
    return 0;
}

qint32 JamTabaVSTPlugin::getStartPositionForHostSync() const
{
    qint32 startPosition = 0;
    double samplesPerBeat = (60.0 * timeInfo->sampleRate)/timeInfo->tempo;
    if (timeInfo->ppqPos > 0){ //when host start button is pressed (and the cursor is aligned in project start) ppqPos is positive in Reaper, but negative in  Cubase
        double cursorPosInMeasure = timeInfo->ppqPos - timeInfo->barStartPos;
        if (cursorPosInMeasure > 0.00000001) { //the 'cursor' in the vst host is not aligned in the measure start. Whe need shift the interval start a little bit.
            //comparing with 0.00000001 because when the 'cursor' is in 5th beat Reaper is returning barStartPos = 4.9999999 and ppqPos = 5
            double samplesUntilNextMeasure = (timeInfo->timeSigNumerator - cursorPosInMeasure) * samplesPerBeat;
            startPosition = -samplesUntilNextMeasure; //shift the start position to compensate the 'cursor' position
        }
    }
    else{ //host is returning negative values for timeInfo structure when start button is pressed
        startPosition = timeInfo->ppqPos * samplesPerBeat; //wil generate a negative value
    }
    return startPosition;
}

void JamTabaVSTPlugin::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
{
    if (!controller)
        return;

    if (controller->isPlayingInNinjamRoom()) {

        // ++++++++++ sync ninjam BPM with host BPM ++++++++++++
        // ask timeInfo to VST host

        timeInfo = getTimeInfo(kVstTransportPlaying | kVstTransportChanged | kVstTempoValid);
        if (transportStartDetectedInHost()) {// user pressing play/start in host?
            NinjamControllerPlugin *ninjamController = controller->getNinjamController();
            Q_ASSERT(ninjamController);
            if (ninjamController->isWaitingForHostSync())
                ninjamController->startSynchronizedWithHost(getStartPositionForHostSync());
        }
    }

    // ++++++++++ Audio processing +++++++++++++++
    inputBuffer.setFrameLenght(sampleFrames);
    for (int c = 0; c < inputBuffer.getChannels(); ++c)
        memcpy(inputBuffer.getSamplesArray(c), inputs[c], sizeof(float) * sampleFrames);

    outputBuffer.setFrameLenght(sampleFrames);
    outputBuffer.zero();

    controller->process(inputBuffer, outputBuffer, this->sampleRate);

    int channels = outputBuffer.getChannels();
    for (int c = 0; c < channels; ++c)
        memcpy(outputs[c], outputBuffer.getSamplesArray(c), sizeof(float) * sampleFrames);

    // ++++++++++++++++++++++++++++++
    hostWasPlayingInLastAudioCallBack = hostIsPlaying();
}

MainControllerPlugin *JamTabaVSTPlugin::createControllerPlugin(const Persistence::Settings &settings, JamTabaPlugin *plugin) const
{
    return new MainControllerVST(settings, dynamic_cast<JamTabaVSTPlugin*>(plugin));
}

float JamTabaVSTPlugin::getSampleRate() const
{
    return this->sampleRate;
}

void JamTabaVSTPlugin::setSampleRate(float sampleRate)
{
    JamTabaPlugin::setSampleRate(sampleRate);
    this->sampleRate = sampleRate;
}

void JamTabaVSTPlugin::suspend()
{
    qCDebug(jtVstPlugin) << "JamtabaPLugin::suspend()";
}

void JamTabaVSTPlugin::resume()
{
    qCDebug(jtVstPlugin) << "JamtabaPLugin::resume()";
}
