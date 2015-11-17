!include( ../Jamtaba-common.pri ) {
    error( "Couldn't find the Jamtaba-common.pri file!" )
}

#CONFIG += qtwinmigrate-uselib
!include(../qtwinmigrate/src/qtwinmigrate.pri) {
    error( "Couldn't find the qtwinmigrate common.pri file!" )
}

QT       += core gui network widgets

TARGET = "JamtabaVST"
TEMPLATE = lib
CONFIG += shared

win32-msvc* {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    #RC_FILE = vstdll.rc
}

HEADERS += \
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
    ../src/geo/WebIpToLocationResolver.h \
    Plugin.h \
    Editor.h \
    MainControllerVST.h \
    NinjamRoomWindowVST.h \
    MainWindowVST.h


SOURCES += \
    $$VST_SDK_PATH/public.sdk/source/vst2.x/audioeffectx.cpp \
    $$VST_SDK_PATH/public.sdk/source/vst2.x/audioeffect.cpp \
    ../src/recorder/JamRecorder.cpp \
    ../src/recorder/ReaperProjectGenerator.cpp \
    ../src/loginserver/LoginService.cpp \
    ../src/loginserver/JsonUtils.cpp \
    ../src/ninjam/protocol/ServerMessages.cpp \
    ../src/ninjam/protocol/ClientMessages.cpp \
    ../src/ninjam/protocol/ServerMessageParser.cpp \
    ../src/ninjam/Server.cpp \
    ../src/audio/Resampler.cpp \
    ../src/audio/vorbis/VorbisDecoder.cpp \
    ../src/audio/samplesbufferrecorder.cpp \
    ../src/audio/vorbis/VorbisEncoder.cpp \
    ../src/persistence/Settings.cpp \
    ../src/geo/WebIpToLocationResolver.cpp \
    VstMain.cpp \
    Plugin.cpp \
    Editor.cpp \
    MainControllerVST.cpp \
    NinjamRoomWindowVST.cpp \
    MainWindowVST.cpp


win32 {
    message("Windows VST build")

    INCLUDEPATH += "$$VST_SDK_PATH/"
    INCLUDEPATH += "$$VST_SDK_PATH/pluginterfaces/vst2.x/"
    INCLUDEPATH += "$$VST_SDK_PATH/public.sdk/source/vst2.x"

    LIBS +=  -lwinmm -lole32 -lws2_32 -lAdvapi32 -lUser32 #-lPsapi
    #performance monitor lib
    #QMAKE_CXXFLAGS += -DPSAPI_VERSION=1

    !contains(QMAKE_TARGET.arch, x86_64) {
        message("x86 build") ## Windows x86 (32bit) specific build here
        LIBS_PATH = "static/win32-msvc"
    } else {
        message("x86_64 build") ## Windows x64 (64bit) specific build here
        LIBS_PATH = "static/win64-msvc"
    }

    #+++++++++++++ link windows platform statically +++++++++++++++++++++++++++++++++++
    #release platform libs
    CONFIG(release, debug|release): LIBS += -lQt5PlatformSupport
    CONFIG(release, debug|release): LIBS += -L$(QTDIR)\plugins\platforms\ -lqwindows #link windows platform statically

    #debug platform libs
    CONFIG(debug, debug|release): LIBS += -lQt5PlatformSupportd #link windows platform statically
    CONFIG(debug, debug|release): LIBS += -L$(QTDIR)\plugins\platforms\ -lqwindowsd #link windows platform statically
    #++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    CONFIG(release, debug|release): LIBS += -L$$PWD/../libs/$$LIBS_PATH/ -lminimp3 -lvorbisfile -lvorbis -logg
    else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libs/$$LIBS_PATH/ -lminimp3d -lvorbisfiled -lvorbisd -loggd

    CONFIG(release, debug|release) {
      #ltcg - http://blogs.msdn.com/b/vcblog/archive/2009/02/24/quick-tips-on-using-whole-program-optimization.aspx
      QMAKE_CXXFLAGS_RELEASE +=  -GL
      QMAKE_LFLAGS_RELEASE += /LTCG
    }

}


INCLUDEPATH += $$PWD/../libs/includes/ogg              \
               $$PWD/../libs/includes/vorbis           \
               $$PWD/../libs/includes/minimp3          \
#               $$PWD/../qtwinmigrate/src                  \

DEPENDPATH +=  $$PWD/../libs/includes/ogg              \
               $$PWD/../libs/includes/vorbis           \
               $$PWD/../libs/includes/minimp3          \

#turn on debug info on release build, usefull sometimes
#QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
#QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
