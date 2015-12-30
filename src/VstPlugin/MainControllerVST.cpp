#include "MainControllerVST.h"
#include "midi/MidiDriver.h"
#include "NinjamController.h"
#include "MainWindow.h"
#include "Plugin.h"
#include "log/Logging.h"
#include "Editor.h"

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
using namespace Controller;

//class AudioDriverVST : public Audio::NullAudioDriver
//{
//public:
//    AudioDriverVST() :
//        NullAudioDriver()
//    {
//        globalInputRange = Audio::ChannelRange(0, 4);// 4 inputs
//        globalOutputRange = Audio::ChannelRange(0, 2);// 2 outputs
//    }
//};
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NinjamControllerVST::NinjamControllerVST(MainControllerVST *c) :
    NinjamController(c),
    waitingForHostSync(false)
{
}

void NinjamControllerVST::waitForHostSync()
{
    waitingForHostSync = true;
    reset();
}

void NinjamControllerVST::syncWithHost()
{
    waitingForHostSync = false;
}

void NinjamControllerVST::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                                  int sampleRate)
{
    if (waitingForHostSync)// skip the ninjam processing if is waiting for sync
        return;
    NinjamController::process(in, out, sampleRate);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainControllerVST::MainControllerVST(Persistence::Settings settings, JamtabaPlugin *plugin) :
    MainController(settings),
    plugin(plugin)
{
    qCDebug(jtCore) << "Creating MainControllerVST instance!";
}

MainControllerVST::~MainControllerVST()
{
    if (mainWindow)
        saveLastUserSettings(mainWindow->getInputsSettings());
}

void MainControllerVST::resizePluginEditor(int newWidth, int newHeight)
{
    if (plugin) {
        VstEditor *editor = (VstEditor *)plugin->getEditor();
        if (editor)
            editor->resize(newWidth, newHeight);
        plugin->sizeWindow(newWidth, newHeight);
    }
}

// +++++++++++++++++++++++++++++++++++++
int MainControllerVST::addInputTrackNode(Audio::LocalInputAudioNode *inputTrackNode)
{
    int inputTrackID = MainController::addInputTrackNode(inputTrackNode);

    // VST plugins always use audio as input
    int firstChannelIndex = (inputTracks.size()-1) * 2;
    inputTrackNode->setAudioInputSelection(firstChannelIndex, 2);
    return inputTrackID;
}

// ++++++++++++++++++++++++++++++++++
QString MainController::getJamtabaFlavor() const
{
    return "Vst Plugin";
}

QString MainControllerVST::getUserEnvironmentString() const
{
    return MainController::getUserEnvironmentString() + " running in " + getHostName();
}

// +++++++++++++++++++++++++++++++++++++
QString MainControllerVST::getHostName() const
{
    if (plugin)
        return plugin->getHostName();
    return "(Error getting host name)";
}

int MainControllerVST::getHostBpm() const
{
    if (plugin)
        return plugin->getHostBpm();
    return -1;
}

void MainControllerVST::setSampleRate(int newSampleRate)
{
    this->sampleRate = newSampleRate;
    MainController::setSampleRate(newSampleRate);
}

Controller::NinjamController *MainControllerVST::createNinjamController()
{
    return new NinjamControllerVST(this);
}

Midi::MidiDriver *MainControllerVST::createMidiDriver()
{
    return new Midi::NullMidiDriver();
}

void MainControllerVST::setCSS(QString css)
{
    qCDebug(jtCore) << "setting CSS";
    qApp->setStyleSheet(css);// qApp is a global variable created in dll main.
}
