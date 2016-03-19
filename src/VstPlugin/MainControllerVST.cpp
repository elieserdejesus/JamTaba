#include "MainControllerVST.h"
#include "NinjamControllerVST.h"
#include "midi/MidiDriver.h"
#include "MainWindow.h"
#include "Plugin.h"
#include "log/Logging.h"
#include "Editor.h"

using namespace Controller;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainControllerVST::MainControllerVST(const Persistence::Settings &settings, JamtabaPlugin *plugin) :
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

Persistence::Preset MainControllerVST::loadPreset(const QString &name)
{
    return settings.readPresetFromFile(name, false);//don't allow multi subchannels in vst plugin and avoid hacking in json file to create subchannels in VSt plugin.
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
int MainControllerVST::addInputTrackNode(Audio::LocalInputNode *inputTrackNode)
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

void MainControllerVST::setCSS(const QString &css)
{
    qCDebug(jtCore) << "setting CSS";
    qApp->setStyleSheet(css);// qApp is a global variable created in dll main.
}
