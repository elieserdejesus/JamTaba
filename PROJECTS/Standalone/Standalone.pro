!include( ../Jamtaba-common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

QT += core gui network widgets

TARGET = Jamtaba2
TEMPLATE = app

INCLUDEPATH += $$ROOT_PATH/libs/includes/portaudio
INCLUDEPATH += $$ROOT_PATH/libs/includes/rtmidi
INCLUDEPATH += $$ROOT_PATH/libs/includes/ogg
INCLUDEPATH += $$ROOT_PATH/libs/includes/vorbis
INCLUDEPATH += $$ROOT_PATH/libs/includes/minimp3

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

HEADERS += StandaloneMainController.h
HEADERS += gui/StandaloneMainWindow.h
HEADERS += gui/StandalonePreferencesDialog.h
HEADERS += gui/StandaloneLocalTrackView.h
HEADERS += gui/StandaloneLocalTrackGroupView.h
HEADERS += gui/ScanFolderPanel.h
HEADERS += gui/FxPanel.h
HEADERS += gui/FxPanelItem.h

HEADERS += audio/core/PortAudioDriver.h
HEADERS += recorder/JamRecorder.h
HEADERS += recorder/ReaperProjectGenerator.h
HEADERS += ninjam/protocol/ServerMessageParser.h
HEADERS += ninjam/protocol/ServerMessages.h
HEADERS += ninjam/protocol/ClientMessages.h
HEADERS += loginserver/natmap.h
HEADERS += audio/codec.h
HEADERS += midi/rtMidiDriver.h
HEADERS += audio/Resampler.h
HEADERS += audio/vorbis/VorbisDecoder.h
HEADERS += audio/vorbis/VorbisEncoder.h
HEADERS += persistence/Settings.h
HEADERS += vst/VstPlugin.h
HEADERS += vst/VstHost.h
HEADERS += vst/VstLoader.h
HEADERS += vst/PluginFinder.h
HEADERS += geo/WebIpToLocationResolver.h
HEADERS += Libs/SingleApplication/singleapplication.h
HEADERS += audio/core/PluginDescriptor.h

SOURCES += main.cpp
SOURCES += StandaloneMainController.cpp
SOURCES += gui/StandaloneMainWindow.cpp
SOURCES += gui/StandalonePreferencesDialog.cpp
SOURCES += gui/StandaloneLocalTrackView.cpp
SOURCES += gui/StandaloneLocalTrackGroupView.cpp
SOURCES += gui/ScanFolderPanel.cpp
SOURCES += gui/FxPanel.cpp
SOURCES += gui/FxPanelItem.cpp

SOURCES += recorder/JamRecorder.cpp
SOURCES += recorder/ReaperProjectGenerator.cpp
SOURCES += loginserver/LoginService.cpp
SOURCES += loginserver/JsonUtils.cpp
SOURCES += ninjam/protocol/ServerMessages.cpp
SOURCES += ninjam/protocol/ClientMessages.cpp
SOURCES += ninjam/protocol/ServerMessageParser.cpp
SOURCES += ninjam/Server.cpp
SOURCES += midi/rtMidiDriver.cpp
SOURCES += audio/Resampler.cpp
SOURCES += audio/vorbis/VorbisDecoder.cpp
SOURCES += audio/samplesbufferrecorder.cpp
SOURCES += audio/vorbis/VorbisEncoder.cpp
SOURCES += persistence/Settings.cpp
SOURCES += vst/VstPlugin.cpp
SOURCES += vst/VstHost.cpp
SOURCES += vst/PluginFinder.cpp
SOURCES += vst/VstLoader.cpp
SOURCES += geo/WebIpToLocationResolver.cpp
SOURCES += Libs/SingleApplication/singleapplication.cpp
SOURCES += audio/core/PortAudioDriver.cpp
SOURCES += audio/core/PluginDescriptor.cpp

#conditional sources to different platforms
win32{
    SOURCES += audio/core/WindowsPortAudioDriver.cpp
}
macx{
    SOURCES += audio/core/MacPortAudioDriver.cpp
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

        CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/$$LIBS_PATH -lportaudio -lminimp3 -lrtmidi -lvorbisfile -lvorbis -logg
        else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/$$LIBS_PATH/ -lportaudiod -lminimp3d -lrtmidid -lvorbisfiled -lvorbisd -loggd

        CONFIG(release, debug|release) {
            #ltcg - http://blogs.msdn.com/b/vcblog/archive/2009/02/24/quick-tips-on-using-whole-program-optimization.aspx
            QMAKE_CXXFLAGS_RELEASE +=  -GL -Gy -Gw
            QMAKE_LFLAGS_RELEASE += /LTCG
        }
    }

    win32-g++{#MinGW compiler
        message("MinGW x86 build")
        LIBS_PATH = "static/win32-mingw"
        LIBS += -L$$PWD/../../libs/$$LIBS_PATH -lportaudio -lminimp3 -lrtmidi -lvorbisfile -lvorbisenc -lvorbis -logg

        #supressing some MinGW annoying warnings
        QMAKE_CXXFLAGS_WARN_ON += -Wunused-variable
        QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
    }

    LIBS +=  -lwinmm -lole32 -lws2_32 -lAdvapi32 -lUser32 #-lPsapi
    #performance monitor lib
    #QMAKE_CXXFLAGS += -DPSAPI_VERSION=1

    RC_FILE = ../Jamtaba2.rc #windows icon
}


macx{
    message("Mac build")

    macx-clang-32 {
        message("i386 build") ## mac 32bit specific build here
        LIBS_PATH = "static/mac32"
    } else {
        message("x86_64 build") ## mac 64bit specific build here
        LIBS_PATH = "static/mac64"
    }

    LIBS += -framework CoreAudio
    LIBS += -framework CoreMidi
    LIBS += -framework AudioToolbox
    LIBS += -framework AudioUnit
    LIBS += -framework CoreServices
    LIBS += -framework Carbon

    LIBS += -L$$PWD/../../libs/$$LIBS_PATH/ -lportaudio -lminimp3  -lrtmidi -lvorbisfile -lvorbisenc -lvorbis -logg

    #mac osx doc icon
    ICON = ../Jamtaba.icns
}
