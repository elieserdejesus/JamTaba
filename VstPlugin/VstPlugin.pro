#CONFIG += qtwinmigrate-uselib

!include( ../Jamtaba-common.pri ) {
    error( "Couldn't find the Jamtaba-common.pri file!" )
}

!include(../qtwinmigrate/src/qtwinmigrate.pri) {
    error( "Couldn't find the qtwinmigrate common.pri file!" )
}

QT += core widgets network

TARGET = "JamtabaVST"
TEMPLATE = lib
DEFINES += VST_FORCE_DEPRECATED=0
CONFIG += shared

win32 {
    LIBS += -lwinmm
    LIBS += -ladvapi32
    LIBS += -lws2_32
    LIBS += -lQt5PlatformSupport #link windows platform statically
    LIBS += -L$(QTDIR)\plugins\platforms\ -lqwindows #link windows platform statically
    #LIBS += -limm32
}

win32-msvc* {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    #RC_FILE = vstdll.rc
}

HEADERS += \
    vst.h \
    ../src/recorder/JamRecorder.h \
    ../src/recorder/ReaperProjectGenerator.h \
    ../src/ninjam/protocol/ServerMessageParser.h \
    ../src/ninjam/protocol/ServerMessages.h \
    ../src/ninjam/protocol/ClientMessages.h \
    ../src/loginserver/natmap.h \
    ../src/audio/codec.h \
    ../src/audio/Resampler.h \
    ../src/audio/vorbis/VorbisDecoder.h \
    ../src/audio/vorbis/VorbisEncoder.h \
    ../src/persistence/Settings.h \
    #../src/audio/vst/VstPlugin.h \
    #../src/audio/vst/vsthost.h \
    ../src/geo/WebIpToLocationResolver.h \


SOURCES += \
    main.cpp \
    audioeffectx.cpp \
    audioeffect.cpp \
    vst.cpp \
    ../src/recorder/JamRecorder.cpp \
    ../src/recorder/ReaperProjectGenerator.cpp \
    ../src/loginserver/LoginService.cpp \
    ../src/loginserver/JsonUtils.cpp \
    ../src/ninjam/protocol/ServerMessages.cpp \
    ../src/ninjam/protocol/ClientMessages.cpp \
    ../src/ninjam/protocol/ServerMessageParser.cpp \
    ../src/ninjam/Server.cpp \
    #../src/audio/vst/PluginFinder.cpp \
    #../src/midi/portmididriver.cpp \
    ../src/audio/Resampler.cpp \
    ../src/audio/vorbis/VorbisDecoder.cpp \
    ../src/audio/SamplesBufferResampler.cpp \
    ../src/audio/samplesbufferrecorder.cpp \
    ../src/audio/vorbis/VorbisEncoder.cpp \
    ../src/persistence/Settings.cpp \
    #../src/audio/vst/VstPlugin.cpp \
    #../src/audio/vst/vsthost.cpp \
    ../src/geo/WebIpToLocationResolver.cpp \


win32 {
    message("Windows VST build")

    INCLUDEPATH += "E:/Jamtaba2/VST3 SDK/"
    INCLUDEPATH += "E:/Jamtaba2/VST3 SDK/pluginterfaces/vst2.x/"
    INCLUDEPATH += "E:/Jamtaba2/VST3 SDK/public.sdk/source/vst2.x"

    LIBS +=  -lwinmm -lole32 -lws2_32 -lAdvapi32 -lUser32  \

    VST_SDK_PATH = "E:/Jamtaba2/VST3 SDK/pluginterfaces/vst2.x/"

    !contains(QMAKE_TARGET.arch, x86_64) {
        message("x86 build") ## Windows x86 (32bit) specific build here
        LIBS_PATH = "win32-msvc"
    } else {
        message("x86_64 build") ## Windows x64 (64bit) specific build here
        LIBS_PATH = "win64-msvc"
    }

    CONFIG(release, debug|release): LIBS += -L$$PWD/../libs/$$LIBS_PATH/ -lportaudio -lminimp3 -lportmidi -lvorbisfile -lvorbis -logg
    else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libs/$$LIBS_PATH/ -lportaudiod -lminimp3d -lportmidid -lvorbisfiled -lvorbisd -loggd
}

INCLUDEPATH += $$PWD/../libs/includes/portaudio        \
               $$PWD/../libs/includes/portmidi         \
               $$PWD/../libs/includes/ogg              \
               $$PWD/../libs/includes/vorbis           \
               $$PWD/../libs/includes/minimp3          \
               #$$PWD/../libs/includes/libip2location   \

DEPENDPATH += $$PWD/../libs/includes/portaudio         \
               $$PWD/../libs/includes/portmidi         \
               $$PWD/../libs/includes/ogg              \
               $$PWD/../libs/includes/vorbis           \
               $$PWD/../libs/includes/minimp3          \
               #$$PWD/../libs/includes/libip2location   \
