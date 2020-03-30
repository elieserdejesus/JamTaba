#include "JamTabaVSTPlugin.h"
#include <QtGlobal>
#include "MainControllerVST.h"
#include "log/Logging.h"

/**
    These two lines are necessary to load the Qt windows platform plugin statically. By default
    qt load the platform plugin from a external dll and I have an error about loading the windows platform DLL.
 */
#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(DSServicePlugin) // necessary to use QCamera inside VST Plugin

extern AudioEffect *createEffectInstance(audioMasterCallback audioMaster);

extern "C" {
#define VST_EXPORT _declspec(dllexport)
// #define VST_EXPORT

// ------------------------------------------------------------------------
/** Prototype of the export function main */
// ------------------------------------------------------------------------
VST_EXPORT AEffect *VSTPluginMain(audioMasterCallback audioMaster)
{
    // Get VST Version of the Host
    if (!audioMaster(0, audioMasterVersion, 0, 0, 0, 0))
        return 0;      // old version

    // Create the AudioEffect
    AudioEffect *effect = createEffectInstance(audioMaster);
    if (!effect)
        return 0;

    // Return the VST AEffect structure
    return effect->getAeffect();
}
} // extern "C"

// ------------------------------------------------------------------------

#if WIN32
#include <windows.h>
#include <QMfcApp>

extern "C" {
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID /*lpvReserved*/)
{
    static bool ownApplication = FALSE;
    if (dwReason == DLL_PROCESS_ATTACH) {
        QApplication::setApplicationName("Jamtaba 2");
        QApplication::setApplicationVersion(APP_VERSION);
        QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // fixing issue https://github.com/elieserdejesus/JamTaba/issues/1216

        // start the configurator
        Configurator *configurator = Configurator::getInstance();
        if (!configurator->setUp())
            qCWarning(jtConfigurator) << "JTBConfig->setUp() FAILED !";

        ownApplication = QMfcApp::pluginInstance(hInst);
    }
    if (dwReason == DLL_PROCESS_DETACH && ownApplication)
        delete qApp;

    return TRUE;
}
}// extern "C"

#endif
