include(../Jamtaba-common.pri)

#CONFIG += qtwinmigrate-uselib
include(../qtwinmigrate/src/qtwinmigrate.pri)

QTPLUGIN += dsengine # necessary to use QCamera inside VST plugin

TARGET = "JamtabaVST2"  #using this name (with a '2' suffix) to match the previons JTBA versions and avoid duplicated plugin in user machines
TEMPLATE = lib
CONFIG += shared

INCLUDEPATH += $$SOURCE_PATH/Plugins
INCLUDEPATH += $$SOURCE_PATH/Plugins/VST

win32{
    DEFINES += _CRT_SECURE_NO_WARNINGS

    QMAKE_LFLAGS += /ignore:4099 #supressing warning about missing .pdb files

    INCLUDEPATH += "$$VST_SDK_PATH/VST2_SDK"
    INCLUDEPATH += "$$VST_SDK_PATH/VST2_SDK/pluginterfaces/vst2.x/"
    INCLUDEPATH += "$$VST_SDK_PATH/VST2_SDK/public.sdk/source/vst2.x"

    DEF_FILE = VstPlugin.def #exporting DLL functions - fixing #1131
}

VPATH += $$SOURCE_PATH/Common
VPATH += $$SOURCE_PATH/Plugins/
VPATH += $$SOURCE_PATH/Plugins/VST

HEADERS += JamTabaPlugin.h
HEADERS += JamTabaVSTPlugin.h
HEADERS += Editor.h
HEADERS += MainControllerPlugin.h
HEADERS += MainControllerVST.h
HEADERS += MainWindowVST.h
HEADERS += NinjamControllerPlugin.h
HEADERS += NinjamRoomWindowPlugin.h
HEADERS += MainWindowPlugin.h
HEADERS += TopLevelTextEditorModifier.h
win32:HEADERS += SonarTextEditorModifier.h
HEADERS += PreferencesDialogPlugin.h
HEADERS += KeyboardHook.h

SOURCES += main.cpp
SOURCES += JamTabaPlugin.cpp
SOURCES += JamTabaVSTPlugin.cpp
SOURCES += Editor.cpp
SOURCES += MainControllerVST.cpp
SOURCES += MainControllerPlugin.cpp
SOURCES += ConfiguratorPlugin.cpp
SOURCES += NinjamRoomWindowPlugin.cpp
SOURCES += NinjamControllerPlugin.cpp
SOURCES += MainWindowPlugin.cpp
SOURCES += MainWindowVST.cpp
SOURCES += TopLevelTextEditorModifier.cpp
win32:SOURCES += SonarTextEditorModifier.cpp
SOURCES += PreferencesDialogPlugin.cpp
SOURCES += KeyboardHook.cpp
SOURCES += $$VST_SDK_PATH/VST2_SDK/public.sdk/source/vst2.x/audioeffectx.cpp
SOURCES += $$VST_SDK_PATH/VST2_SDK/public.sdk/source/vst2.x/audioeffect.cpp

win32 {
    #message("Windows VST build")

    LIBS +=  -lwinmm -lole32 -lws2_32 -lAdvapi32 -lUser32 #-lPsapi
    #performance monitor lib
    #QMAKE_CXXFLAGS += -DPSAPI_VERSION=1

    !contains(QMAKE_TARGET.arch, x86_64) {
        #message("x86 build") ## Windows x86 (32bit) specific build here
        LIBS_PATH = "static/win32-msvc"
    } else {
        #message("x86_64 build") ## Windows x64 (64bit) specific build here
        LIBS_PATH = "static/win64-msvc"
    }

    #+++++++++++++ link windows platform statically +++++++++++++++++++++++++++++++++++
    #release platform libs
    CONFIG(release, debug|release): LIBS += -lQt5PlatformSupport
    CONFIG(release, debug|release): LIBS += -L$(QTDIR)\plugins\platforms\ -lqwindows #link windows platform statically
    CONFIG(release, debug|release):   LIBS += -L$(QTDIR)\plugins\mediaservice\ -ldsengine # necessary to use QCamera
    CONFIG(release, debug|release):   LIBS += -L$(QTDIR)\plugins\mediaservice\ -lqtfreetype



    #debug platform libs
    CONFIG(debug, debug|release): LIBS += -lQt5PlatformSupportd #link windows platform statically
    CONFIG(debug, debug|release): LIBS += -L$(QTDIR)\plugins\platforms\ -lqwindowsd #link windows platform statically
    CONFIG(debug, debug|release): LIBS += -L$(QTDIR)\plugins\mediaservice\ -ldsengined # necessary to use QCamera
    CONFIG(debug, debug|release):   LIBS += -L$(QTDIR)\plugins\mediaservice\ -lqtfreetyped
    #++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    LIBS += -L$$PWD/../../libs/$$LIBS_PATH -lminimp3 -lvorbisfile -lvorbis -logg -lx264 -lavcodec -lavutil -lavformat -lswscale -lswresample -lstackwalker -lminiupnpc

    LIBS += -lIPHlpApi # used by miniupnp lib
    LIBS += -lSecur32   # used by libx264
    LIBS += -lMfplat
    LIBS += -lmfuuid
    LIBS += -lD3D9
    LIBS += -lMf
    LIBS += -lDxva2
    LIBS += -lMfreadwrite
    LIBS += -levr
    LIBS += -lWtsapi32
    LIBS += -lwmcodecdspuuid



    CONFIG(release, debug|release) {
      #ltcg - http://blogs.msdn.com/b/vcblog/archive/2009/02/24/quick-tips-on-using-whole-program-optimization.aspx
      QMAKE_CXXFLAGS_RELEASE +=  -GL
      QMAKE_LFLAGS_RELEASE += /LTCG
    }

    QMAKE_LFLAGS += "/NODEFAULTLIB:libcmt"
}
