#include "JamTabaPlugin.h"
#include "MainControllerPlugin.h"
#include "NinjamControllerPlugin.h"
#include "log/Logging.h"
#include <QApplication>

// anti troll scheme to avoid multiple connections in ninjam servers
bool JamTabaPlugin::instanceIsInitialized = false;


bool JamTabaPlugin::pluginIsInitialized()
{
    return JamTabaPlugin::instanceIsInitialized;
}

JamTabaPlugin::JamTabaPlugin(quint8 inputChannels, quint8 outputChannels) :
    controller(nullptr),
    running(false),
    inputBuffer(inputChannels),
    outputBuffer(outputChannels),
    hostWasPlayingInLastAudioCallBack(false)
{
    qCDebug(jtVstPlugin) << "Base Plugin constructor...";
}

JamTabaPlugin::~JamTabaPlugin ()
{
    qCDebug(jtVstPlugin) << "Base Plugin destructor";
}

MainControllerPlugin *JamTabaPlugin::getController()
{
    return controller.data();
}

void JamTabaPlugin::initialize()
{
    if (!isRunning()) {
        if (!controller) {
            qCDebug(jtVstPlugin) << "Plugin initialize()..." << this;
            QApplication::setApplicationName("Jamtaba 2");
            QApplication::setApplicationVersion(VERSION);

            Persistence::Settings settings; // read from file in constructor
            settings.load();
            qCDebug(jtVstPlugin)<< "Creating controller!";
            controller.reset(createPluginMainController(settings, this));
            controller->setSampleRate(getSampleRate());
            controller->start();
            controller->connectInJamtabaServer();

            qCDebug(jtVstPlugin)<< "Controller started!";
            running = true;
            qCDebug(jtVstPlugin) << "Plugin initialize() done";

            JamTabaPlugin::instanceIsInitialized = true; // the anti troll flag :)
        }
    }
}

void JamTabaPlugin::close()
{
    qCDebug(jtVstPlugin) << "JamtabaPlugin::close()";

    running = false;

    JamTabaPlugin::instanceIsInitialized = false; // the anti troll flag :)
}

void JamTabaPlugin::setSampleRate(float sampleRate)
{
    if (controller)
        controller->setSampleRate(sampleRate);
}
