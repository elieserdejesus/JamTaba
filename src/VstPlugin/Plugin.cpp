#include "Plugin.h"
#include <cmath>
#include <QWinWidget>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QStandardPaths>
#include "MainControllerVST.h"
#include "NinjamControllerVST.h"
#include "log/Logging.h"
#include "Editor.h"

// anti troll scheme to avoid multiple connections in ninjam servers
bool JamtabaPlugin::instanceIsInitialized = false;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
    if (!JamtabaPlugin::pluginIsInitialized())// avoid multiple instances inside a DAW.
        return new JamtabaPlugin(audioMaster);
    return nullptr;
}

bool JamtabaPlugin::pluginIsInitialized()
{
    return JamtabaPlugin::instanceIsInitialized;
}

JamtabaPlugin::JamtabaPlugin (audioMasterCallback audioMaster) :
    AudioEffectX(audioMaster, 0, 0),
    listEvnts(0),
    controller(nullptr),
    running(false),
    inputBuffer(DEFAULT_INPUTS*2),
    outputBuffer(DEFAULT_OUTPUTS*2),
    timeInfo(nullptr),
    hostWasPlayingInLastAudioCallBack(false)
{
    qCDebug(jtVstPlugin) << "Plugin constructor...";
    setNumInputs(DEFAULT_INPUTS*2);
    setNumOutputs(DEFAULT_OUTPUTS*2);

    isSynth(false);
    canProcessReplacing(true);
    programsAreChunks(false);
    vst_strncpy(programName, "Default", kVstMaxProgNameLen);    // default program name

    canDoubleReplacing(false);

    setEditor(new VstEditor(this));

    // suspend();
    qCDebug(jtVstPlugin) << "Plugin constructor done.";
}

QString JamtabaPlugin::getHostName()
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

bool JamtabaPlugin::hostIsPlaying() const
{
    if (timeInfo)
        return (timeInfo->flags & kVstTransportPlaying) != 0;
    return false;
}

int JamtabaPlugin::getHostBpm() const
{
    if (timeInfo)
        return (int)timeInfo->tempo;
    return 0;
}

VstInt32 JamtabaPlugin::fxIdle()
{
    QApplication::processEvents();
    return 0;
}

bool JamtabaPlugin::needIdle()
{
    return true;
}

JamtabaPlugin::~JamtabaPlugin ()
{
    qCDebug(jtVstPlugin) << "Plugin destructor";
}

void JamtabaPlugin::initialize()
{
    if (!isRunning()) {
        if (!controller) {
            qCDebug(jtVstPlugin) << "Plugin initialize()...";
            QApplication::setApplicationName("Jamtaba 2");
            QApplication::setApplicationVersion(APP_VERSION);

            Persistence::Settings settings;// read from file in constructor
            settings.load();
            qCDebug(jtVstPlugin)<< "Creating controller!";
            controller.reset(new MainControllerVST(settings, this));
            controller->setSampleRate(getSampleRate());
            controller->start();
            controller->connectInJamtabaServer();

            qCDebug(jtVstPlugin)<< "Controller started!";
            running = true;
            qCDebug(jtVstPlugin) << "Plugin initialize() done";

            JamtabaPlugin::instanceIsInitialized = true; // the anti troll flag :)
        }
    }
}

void JamtabaPlugin::open()
{
    qCDebug(jtVstPlugin) << "Plugin open()";
}

void JamtabaPlugin::close()
{
    qCDebug(jtVstPlugin) << "JamtabaPlugin::close()";
    if (editor) {
        if (editor->isOpen())
            editor->close();
        dynamic_cast<VstEditor *>(editor)->deleteMainWindow();
    }

    running = false;
    qCDebug(jtVstPlugin) << "JamtabaPLugin::close() finished";

    JamtabaPlugin::instanceIsInitialized = false; // the anti troll flag :)
}

VstInt32 JamtabaPlugin::getNumMidiInputChannels()
{
    return 0;
}

VstInt32 JamtabaPlugin::getNumMidiOutputChannels()
{
    return 0;
}

bool JamtabaPlugin::getEffectName(char *name)
{
    if (!name)
        return false;
    vst_strncpy(name, "Jamtaba 2", kVstMaxEffectNameLen);
    return true;
}

bool JamtabaPlugin::getProductString(char *text)
{
    if (!text)
        return false;
    vst_strncpy(text, "Jamtaba 2", kVstMaxProductStrLen);
    return true;
}

bool JamtabaPlugin::getVendorString(char *text)
{
    if (!text)
        return false;
    vst_strncpy(text, "Jamtaba", kVstMaxVendorStrLen);
    return true;
}

VstInt32 JamtabaPlugin::getVendorVersion()
{
    return 2;
}

VstInt32 JamtabaPlugin::canDo(char *text)
{
    if (!text)
        return 0;
    if (!strcmp(text, "receiveVstTimeInfo"))
        return 1;
    return 0;
}

void JamtabaPlugin::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
{
    if (!controller)
        return;
    if (controller->isPlayingInNinjamRoom()) {
        // ++++++++++ sync ninjam BPM with host BPM ++++++++++++
        // ask timeInfo to VST host
        timeInfo = getTimeInfo(kVstTransportPlaying | kVstTransportChanged | kVstTempoValid);
        if (transportStartDetectedInHost()) {// user pressing play/start in host?
            NinjamControllerVST *ninjamController = controller->getNinjamController();
            if (ninjamController->isWaitingForHostSync()) {
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

                ninjamController->startSynchronizedWithHost(startPosition);
            }
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

void JamtabaPlugin::setSampleRate(float sampleRate)
{
    qCDebug(jtVstPlugin) << "JamtabaPlugin::setSampleRate()";
    this->sampleRate = sampleRate;
    if (controller)
        controller->setSampleRate(sampleRate);
}

void JamtabaPlugin::suspend()
{
    qCDebug(jtVstPlugin) << "JamtabaPLugin::suspend()";
    if (controller && controller->isPlayingInNinjamRoom()) {
        controller->getNinjamController()->reset(true);// discard downloaded intervals but keep the most recent
        controller->finishUploads();// send the last part of ninjam intervals
    }
}

void JamtabaPlugin::resume()
{
    qCDebug(jtVstPlugin) << "JamtabaPLugin::resume()";
}
