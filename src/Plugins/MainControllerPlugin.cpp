#include "MainControllerPlugin.h"
#include "midi/MidiDriver.h"
#include "audio/core/LocalInputNode.h"
#include "gui/MainWindow.h"
#include "JamTabaPlugin.h"
#include "log/Logging.h"

MainControllerPlugin::MainControllerPlugin(const Settings &settings, JamTabaPlugin *plugin) :
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

persistence::Preset MainControllerPlugin::loadPreset(const QString &name)
{
    return settings.readPresetFromFile(name, false); // don't allow multi subchannels in vst plugin and avoid hacking in json file to create subchannels in VSt plugin.
}


int MainControllerPlugin::addInputTrackNode(audio::LocalInputNode *inputTrackNode)
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

midi::MidiDriver *MainControllerPlugin::createMidiDriver()
{
    return new midi::NullMidiDriver();
}

void MainControllerPlugin::setCSS(const QString &css)
{
    
    if (qApp) {
        qCDebug(jtCore) << "setting CSS";
        qApp->setStyleSheet(css); // qApp is a global variable created in dll main.
    }
    else
    {
        qWarning() << "Can´t set CSS, qApp is null!";
    }
}
