!include( ../Jamtaba-common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

QT       += core gui network widgets

TARGET = Jamtaba
TEMPLATE = app

HEADERS += \
    ../src/audio/core/PortAudioDriver.h \
    ../src/recorder/JamRecorder.h \
    ../src/recorder/ReaperProjectGenerator.h \
    ../src/ninjam/protocol/ServerMessageParser.h \
    ../src/ninjam/protocol/ServerMessages.h \
    ../src/ninjam/protocol/ClientMessages.h \
    ../src/loginserver/natmap.h \
    ../src/audio/codec.h \
    ../src/midi/rtMidiDriver.h \
    ../src/audio/Resampler.h \
    ../src/audio/vorbis/VorbisDecoder.h \
    ../src/audio/vorbis/VorbisEncoder.h \
    ../src/persistence/Settings.h \
    ../src/audio/vst/VstPlugin.h \
    ../src/audio/vst/vsthost.h \
    ../src/geo/WebIpToLocationResolver.h \
    ../src/StandAloneMainController.h \
    ../src/gui/MainWindowStandalone.h \
    ../src/Libs/SingleApplication/singleapplication.h \
    ../src/audio/core/PluginDescriptor.h \
    ../src/audio/vst/VstLoader.h \

SOURCES += \
    ../src/main.cpp \
    ../src/recorder/JamRecorder.cpp \
    ../src/recorder/ReaperProjectGenerator.cpp \
    ../src/loginserver/LoginService.cpp \
    ../src/loginserver/JsonUtils.cpp \
    ../src/ninjam/protocol/ServerMessages.cpp \
    ../src/ninjam/protocol/ClientMessages.cpp \
    ../src/ninjam/protocol/ServerMessageParser.cpp \
    ../src/ninjam/Server.cpp \
    ../src/midi/rtMidiDriver.cpp \
    ../src/audio/Resampler.cpp \
    ../src/audio/vorbis/VorbisDecoder.cpp \
    ../src/audio/samplesbufferrecorder.cpp \
    ../src/audio/vorbis/VorbisEncoder.cpp \
    ../src/persistence/Settings.cpp \
    ../src/audio/vst/VstPlugin.cpp \
    ../src/audio/vst/vsthost.cpp \
    ../src/geo/WebIpToLocationResolver.cpp \
    ../src/StandAloneMainController.cpp \
    ../src/gui/MainWindowStandalone.cpp \
    ../src/Libs/SingleApplication/singleapplication.cpp \
    ../src/audio/core/PortAudioDriver.cpp \
    ../src/audio/core/PluginDescriptor.cpp \
    ../src/audio/vst/VstLoader.cpp \

#conditional sources to different platforms
win32{
    SOURCES += ../src/audio/core/WindowsPortAudioDriver.cpp \
}
macx{
    SOURCES += ../src/audio/core/MacPortAudioDriver.cpp \
}

INCLUDEPATH += $$PWD/../libs/includes/portaudio        \
               $$PWD/../libs/includes/rtmidi           \
               $$PWD/../libs/includes/ogg              \
               $$PWD/../libs/includes/vorbis           \
               $$PWD/../libs/includes/minimp3          \
#               $$PWD/../src/Libs                       \

DEPENDPATH +=  $$PWD/../libs/includes/portaudio        \
               $$PWD/../libs/includes/rtmidi           \
               $$PWD/../libs/includes/ogg              \
               $$PWD/../libs/includes/vorbis           \
               $$PWD/../libs/includes/minimp3          \
#               $$PWD/../src/Libs                       \

win32{

    win32-msvc*{#all msvc compilers
        !contains(QMAKE_TARGET.arch, x86_64) {
            message("msvc x86 build") ## Windows x86 (32bit) specific build here
            LIBS_PATH = "static/win32-msvc"

            #after a lot or research Ezee found this userfull link explaining how compile to be compatible with Windows XP: http://www.tripleboot.org/?p=423
            QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
        } else {
            message("msvc x86_64 build") ## Windows x64 (64bit) specific build here
            LIBS_PATH = "static/win64-msvc"
        }

        CONFIG(release, debug|release): LIBS += -L$$PWD/../libs/$$LIBS_PATH -lportaudio -lminimp3 -lrtmidi -lvorbisfile -lvorbis -logg
        else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libs/$$LIBS_PATH/ -lportaudiod -lminimp3d -lrtmidid -lvorbisfiled -lvorbisd -loggd

        CONFIG(release, debug|release) {
            #ltcg - http://blogs.msdn.com/b/vcblog/archive/2009/02/24/quick-tips-on-using-whole-program-optimization.aspx
            QMAKE_CXXFLAGS_RELEASE +=  -GL -Gy -Gw
            QMAKE_LFLAGS_RELEASE += /LTCG
        }

    }

    win32-g++{#MinGW compiler
        message("MinGW x86 build")
        LIBS_PATH = "static/win32-mingw"
        LIBS += -L$$PWD/../libs/$$LIBS_PATH -lportaudio -lminimp3 -lrtmidi -lvorbisfile -lvorbisenc -lvorbis -logg

        #supressing some MinGW annoying warnings
        QMAKE_CXXFLAGS_WARN_ON += -Wunused-variable
        QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
    }

    LIBS +=  -lwinmm -lole32 -lws2_32 -lAdvapi32 -lUser32 #-lPsapi
    #performance monitor lib
    #QMAKE_CXXFLAGS += -DPSAPI_VERSION=1

    RC_FILE = Jamtaba2.rc #windows icon
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

    LIBS += -L$$PWD/../libs/$$LIBS_PATH/ -lportaudio -lminimp3  -lrtmidi -lvorbisfile -lvorbisenc -lvorbis -logg

    #mac osx doc icon
    ICON = Jamtaba.icns
}

INCLUDEPATH += $$VST_SDK_PATH/pluginterfaces/vst2.x/

#turn on debug info on release build, usefull sometimes
#QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
#QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO


