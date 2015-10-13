#include "vst.h"


//these two lines are necessary to load the Qt windows platform plugin statically. By default
    //qt load the platform plugin from a external dll and I have an error about loading the windows platform DLL.
#include <QtPlugin>
Q_IMPORT_PLUGIN (QWindowsIntegrationPlugin);

extern AudioEffect* createEffectInstance (audioMasterCallback audioMaster);

extern "C" {

#define VST_EXPORT _declspec(dllexport)

    //------------------------------------------------------------------------
    /** Prototype of the export function main */
    //------------------------------------------------------------------------
    VST_EXPORT AEffect* VSTPluginMain (audioMasterCallback audioMaster)
    {
        // Get VST Version of the Host
        if (!audioMaster (0, audioMasterVersion, 0, 0, 0, 0))
            return 0;  // old version

        // Create the AudioEffect
        AudioEffect* effect = createEffectInstance (audioMaster);
        if (!effect)
            return 0;

        // Return the VST AEffect structur
        return effect->getAeffect ();
    }

} // extern "C"

//------------------------------------------------------------------------
#if WIN32
#include <windows.h>
#include <QMfcApp>

extern "C" {
BOOL WINAPI DllMain( HINSTANCE hInst, DWORD dwReason, LPVOID /*lpvReserved*/ )
{
    static bool ownApplication = FALSE;
     if ( dwReason == DLL_PROCESS_ATTACH )
         ownApplication = QMfcApp::pluginInstance( hInst );
     if ( dwReason == DLL_PROCESS_DETACH && ownApplication ) {
         delete qApp;
     }

    return TRUE;
}
}// extern "C"

#endif





///**************************************************************************
//#    Copyright 2010-2011 Raphaël François
//#    Contact : ctrlbrk76@gmail.com
//#
//#    This file is part of VstBoard.
//#
//#    VstBoard is free software: you can redistribute it and/or modify
//#    it under the terms of the under the terms of the GNU Lesser General Public License as published by
//#    the Free Software Foundation, either version 3 of the License, or
//#    (at your option) any later version.
//#
//#    VstBoard is distributed in the hope that it will be useful,
//#    but WITHOUT ANY WARRANTY; without even the implied warranty of
//#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//#    under the terms of the GNU Lesser General Public License for more details.
//#
//#    You should have received a copy of the under the terms of the GNU Lesser General Public License
//#    along with VstBoard.  If not, see <http://www.gnu.org/licenses/>.
//**************************************************************************/


//#include "vst.h"
//#include <QApplication>

//extern AudioEffect* createEffectInstance (audioMasterCallback audioMaster);

//extern "C" {

//    #define VST_EXPORT _declspec(dllexport)

//    //------------------------------------------------------------------------
//    /** Prototype of the export function main */
//    //------------------------------------------------------------------------
//    VST_EXPORT AEffect* VSTPluginMain (audioMasterCallback audioMaster)
//    {
//        // Get VST Version of the Host
//        if (!audioMaster (0, audioMasterVersion, 0, 0, 0, 0))
//            return 0;  // old version

//        // Create the AudioEffect
//        AudioEffect* effect = createEffectInstance (audioMaster);
//        if (!effect)
//            return 0;

//        // Return the VST AEffect structur
//        return effect->getAeffect ();
//    }
//} // extern "C"


////------------------------------------------------------------------------

//#if WIN32
//    #include <windows.h>

//    void* hInstance;
//    QCoreApplication* app;

//    extern "C" {
//        BOOL WINAPI DllMain (HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved){
//            hInstance = hInst;
//            int argc = 0;

//            app = new QCoreApplication(argc, 0);
//            return 1;
//        }
//    } // extern "C"
//#endif





