#include "MainControllerVST.h"
#include "NinjamControllerPlugin.h"
#include "midi/MidiDriver.h"
#include "audio/core/LocalInputNode.h"
#include "MainWindow.h"
#include "JamTabaVSTPlugin.h"
#include "log/Logging.h"
#include "Editor.h"

using namespace Controller;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainControllerVST::MainControllerVST(const Persistence::Settings &settings, JamTabaVSTPlugin *plugin) :
    MainControllerPlugin(settings, plugin),
    vstPlugin(plugin)
{
    qCDebug(jtCore) << "Creating MainControllerVST instance!";
}

void MainControllerVST::resizePluginEditor(int newWidth, int newHeight)
{
    if (vstPlugin) {
        VstEditor *editor = (VstEditor *)vstPlugin->getEditor();
        if (editor)
            editor->resize(newWidth, newHeight);
        vstPlugin->sizeWindow(newWidth, newHeight);
    }
}
