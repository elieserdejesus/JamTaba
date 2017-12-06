!include( ../Jamtaba-common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

QT += core gui network widgets concurrent multimedia multimediawidgets

TARGET = Jamtaba2
TEMPLATE = app

INCLUDEPATH += $$ROOT_PATH/libs/includes/portaudio
INCLUDEPATH += $$ROOT_PATH/libs/includes/rtmidi
INCLUDEPATH += $$ROOT_PATH/libs/includes/ogg
INCLUDEPATH += $$ROOT_PATH/libs/includes/vorbis
INCLUDEPATH += $$ROOT_PATH/libs/includes/minimp3
INCLUDEPATH += $$ROOT_PATH/libs/includes/ffmpeg

INCLUDEPATH += $$SOURCE_PATH/Standalone
INCLUDEPATH += $$SOURCE_PATH/Standalone/gui
INCLUDEPATH += $$SOURCE_PATH/Libs
INCLUDEPATH += $$VST_SDK_PATH/pluginterfaces/vst2.x

DEPENDPATH +=  $$ROOT_PATH/libs/includes/portaudio
DEPENDPATH +=  $$ROOT_PATH/libs/includes/rtmidi
DEPENDPATH +=  $$ROOT_PATH/libs/includes/ogg
DEPENDPATH +=  $$ROOT_PATH/libs/includes/vorbis
DEPENDPATH +=  $$ROOT_PATH/libs/includes/minimp3

VPATH += $$SOURCE_PATH/Standalone

HEADERS += MainControllerStandalone.h
HEADERS += gui/MainWindowStandalone.h
HEADERS += gui/PreferencesDialogStandalone.h
HEADERS += gui/LocalTrackViewStandalone.h
HEADERS += gui/LocalTrackGroupViewStandalone.h
HEADERS += gui/ScanFolderPanel.h
HEADERS += gui/FxPanel.h
HEADERS += gui/FxPanelItem.h
HEADERS += gui/MidiToolsDialog.h

HEADERS += audio/PortAudioDriver.h
HEADERS += audio/Host.h
HEADERS += midi/RtMidiDriver.h
HEADERS += vst/VstPlugin.h
HEADERS += vst/VstHost.h
HEADERS += vst/VstLoader.h
HEADERS += PluginFinder.h
HEADERS += vst/VstPluginFinder.h
HEADERS += vst/Utils.h
HEADERS += Libs/SingleApplication/singleapplication.h
HEADERS += Libs/RtMidi/RtMidi.h

mac:HEADERS += AU/AudioUnitHost.h
mac:HEADERS += AU/AudioUnitPlugin.h

SOURCES += main.cpp
SOURCES += MainControllerStandalone.cpp
SOURCES += ConfiguratorStandalone.cpp
SOURCES += gui/MainWindowStandalone.cpp
SOURCES += gui/PreferencesDialogStandalone.cpp
SOURCES += gui/LocalTrackViewStandalone.cpp
SOURCES += gui/LocalTrackGroupViewStandalone.cpp
SOURCES += gui/ScanFolderPanel.cpp
SOURCES += gui/FxPanel.cpp
SOURCES += gui/FxPanelItem.cpp
SOURCES += gui/MidiToolsDialog.cpp
SOURCES += midi/RtMidiDriver.cpp
SOURCES += vst/VstPlugin.cpp
SOURCES += vst/VstHost.cpp
SOURCES += PluginFinder.cpp
SOURCES += vst/VstPluginFinder.cpp
SOURCES += vst/Utils.cpp
SOURCES += vst/VstLoader.cpp
SOURCES += Libs/SingleApplication/singleapplication.cpp
SOURCES += Libs/RtMidi/RtMidi.cpp
SOURCES += audio/PortAudioDriver.cpp

mac:SOURCES += AU/AudioUnitHost.cpp
mac:SOURCES += AU/AudioUnitPluginFinder.cpp


FORMS += gui/MidiToolsDialog.ui

#conditional sources to different platforms
win32{
    SOURCES += audio/WindowsPortAudioDriver.cpp
    SOURCES += vst/WindowsVstPluginChecker.cpp
}
macx{

    AU_SDK_PATH = "$$PWD/../../AU_SDK"
    VPATH += $$AU_SDK_PATH

    SOURCES += audio/MacPortAudioDriver.cpp
    OBJECTIVE_SOURCES += vst/MacVstPluginChecker.mm

    HEADERS += AU/AudioUnitPlugin.h
    OBJECTIVE_SOURCES += AU/AudioUnitPlugin.mm

    INCLUDEPATH += $$AU_SDK_PATH
}
linux{
    SOURCES += audio/LinuxPortAudioDriver.cpp
    SOURCES += vst/LinuxVstPluginChecker.cpp
}


win32{

    win32-msvc*{#all msvc compilers

        #supressing warning about missing .pdb files
        QMAKE_LFLAGS += /ignore:4099

        !contains(QMAKE_TARGET.arch, x86_64) {
            message("msvc x86 build") ## Windows x86 (32bit) specific build here
            LIBS_PATH = "static/win32-msvc"

            #after a lot or research Ezee found this userfull link explaining how compile to be compatible with Windows XP: http://www.tripleboot.org/?p=423
            QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
        } else {
            message("msvc x86_64 build") ## Windows x64 (64bit) specific build here
            LIBS_PATH = "static/win64-msvc"
        }

        CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/$$LIBS_PATH -lportaudio -lminimp3 -lvorbisfile -lvorbis -logg -lavcodec -lavutil -lavformat -lswscale -lswresample -lstackwalker
        else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/$$LIBS_PATH/ -lportaudiod -lminimp3d -lvorbisfiled -lvorbisd -loggd -lavcodecd -lavutild -lavformatd -lswscaled -lswresampled -lstackwalkerd

        CONFIG(release, debug|release) {
            #ltcg - http://blogs.msdn.com/b/vcblog/archive/2009/02/24/quick-tips-on-using-whole-program-optimization.aspx
            QMAKE_CXXFLAGS_RELEASE +=  -GL -Gy -Gw
            QMAKE_LFLAGS_RELEASE += /LTCG
        }
    }

    win32-g++{#MinGW compiler
       message("MinGW x86 build")
       LIBS_PATH = "static/win32-mingw"

       LIBS += -L$$PWD/../../libs/$$LIBS_PATH -lportaudio -lminimp3 -lvorbisfile -lvorbisenc -lvorbis -logg -lavcodec -lavutil -lavformat -lswscale -lswresample
    }

    LIBS +=  -lwinmm -lole32 -lws2_32 -lAdvapi32 -lUser32 -lPsapi
    #performance monitor lib
    QMAKE_CXXFLAGS += -DPSAPI_VERSION=1

    RC_FILE = ../Jamtaba2.rc #windows icon

    QMAKE_LFLAGS += "/NODEFAULTLIB:libcmt"
}

macx{
    message("Mac build")

    #supressing some warnings
    #QMAKE_CXXFLAGS_WARN_ON += -Wunused-variable
    #QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

    macx-clang-32 {
        message("i386 build") ## mac 32bit specific build here
        LIBS_PATH = "static/mac32"
    } else {
        message("x86_64 build") ## mac 64bit specific build here
        LIBS_PATH = "static/mac64"
    }
    LIBS += -L$$PWD/../../libs/$$LIBS_PATH -lportaudio -lminimp3 -lvorbisfile -lvorbisenc -lvorbis -logg -lavcodec -lavutil -lavformat -lswscale -lswresample -liconv
    LIBS += -framework IOKit
    LIBS += -framework CoreAudio
    LIBS += -framework CoreMidi
    LIBS += -framework AudioToolbox
    LIBS += -framework AudioUnit
    LIBS += -framework CoreServices
    LIBS += -framework Carbon
    LIBS += -framework Cocoa

    #mac osx doc icon
    ICON = ../Jamtaba.icns
}

linux{
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS_PATH = "static/linux64"
    } else {
        LIBS_PATH = "static/linux32"
    }
    message($$LIBS_PATH)

    DEFINES += __LINUX_ALSA__


    LIBS += -L$$PWD/../../libs/$$LIBS_PATH -lportaudio -lminimp3 -lvorbisfile -lvorbisenc -lvorbis -logg -lavformat -lavcodec -lswscale -lavutil -lswresample
    LIBS += -lasound
}

!*-msvc*{ #non microsoft compilers
    #supressing some g++ annoying warnings
    QMAKE_CXXFLAGS_WARN_ON += -Wunused-variable
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}
