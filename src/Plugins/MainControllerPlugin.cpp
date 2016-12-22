#include "MainControllerPlugin.h"
#include "midi/MidiDriver.h"
#include "audio/core/LocalInputNode.h"
#include "MainWindow.h"
#include "JamTabaPlugin.h"
#include "log/Logging.h"
//#include "Editor.h"

using namespace Controller;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainControllerPlugin::MainControllerPlugin(const Persistence::Settings &settings, JamTabaPlugin *plugin) :
    MainController(settings),
    plugin(plugin)
{
    qCDebug(jtCore) << "Creating MainControllerVST instance!";
}

MainControllerPlugin::~MainControllerPlugin()
{
    if (mainWindow)
        saveLastUserSettings(mainWindow->getInputsSettings());
}

Persistence::Preset MainControllerPlugin::loadPreset(const QString &name)
{
    return settings.readPresetFromFile(name, false);//don't allow multi subchannels in vst plugin and avoid hacking in json file to create subchannels in VSt plugin.
}

// +++++++++++++++++++++++++++++++++++++
int MainControllerPlugin::addInputTrackNode(Audio::LocalInputNode *inputTrackNode)
{
    int inputTrackID = MainController::addInputTrackNode(inputTrackNode);

    // VST plugins always use audio as input
    int firstChannelIndex = (inputTracks.size()-1) * 2;
    inputTrackNode->setAudioInputSelection(firstChannelIndex, 2);
    return inputTrackID;
}

QString MainControllerPlugin::getUserEnvironmentString() const
{
    return MainController::getUserEnvironmentString() + " running in " + getHostName();
}

// +++++++++++++++++++++++++++++++++++++
QString MainControllerPlugin::getHostName() const
{
    if (plugin)
        return plugin->getHostName();
    return "(Error getting host name)";
}

int MainControllerPlugin::getHostBpm() const
{
    if (plugin)
        return plugin->getHostBpm();
    return -1;
}

void MainControllerPlugin::setSampleRate(int newSampleRate)
{
    //this->sampleRate = newSampleRate;
    MainController::setSampleRate(newSampleRate);
}

float MainControllerPlugin::getSampleRate() const
{
    if (plugin)
        return plugin->getSampleRate();

    return 0;
}

NinjamControllerPlugin *MainControllerPlugin::createNinjamController()
{
    return new NinjamControllerPlugin(const_cast<MainControllerPlugin*>(this));
}

Midi::MidiDriver *MainControllerPlugin::createMidiDriver()
{
    return new Midi::NullMidiDriver();
}

void MainControllerPlugin::setCSS(const QString &css)
{
    qCDebug(jtCore) << "setting CSS";
    qApp->setStyleSheet(css);// qApp is a global variable created in dll main.
}
