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

    #supressing warning about missing .pdb files
    QMAKE_LFLAGS += /ignore:4099
}

INCLUDEPATH += ../src
INCLUDEPATH += src
INCLUDEPATH += ../libs/includes/ogg
INCLUDEPATH += ../libs/includes/vorbis
INCLUDEPATH += ../libs/includes/minimp3


win32{
    INCLUDEPATH += "$$VST_SDK_PATH/"
    INCLUDEPATH += "$$VST_SDK_PATH/pluginterfaces/vst2.x/"
    INCLUDEPATH += "$$VST_SDK_PATH/public.sdk/source/vst2.x"
}

VPATH       += ../src
VPATH       += src

DEPENDPATH +=  ../libs/includes/ogg
DEPENDPATH +=  ../libs/includes/vorbis
DEPENDPATH +=  ../libs/includes/minimp3

HEADERS += recorder/JamRecorder.h
HEADERS += recorder/ReaperProjectGenerator.h
HEADERS += ninjam/protocol/ServerMessageParser.h
HEADERS += ninjam/protocol/ServerMessages.h
HEADERS += ninjam/protocol/ClientMessages.h
HEADERS += loginserver/natmap.h
HEADERS += audio/codec.h
HEADERS += audio/Resampler.h
HEADERS += audio/vorbis/VorbisDecoder.h
HEADERS += audio/vorbis/VorbisEncoder.h
HEADERS += persistence/Settings.h
HEADERS += geo/WebIpToLocationResolver.h
HEADERS += Plugin.h
HEADERS += Editor.h
HEADERS += MainControllerVST.h
HEADERS += NinjamRoomWindowVST.h
HEADERS += MainWindowVST.h
HEADERS += KeyboardHook.h

SOURCES += main.cpp
SOURCES += Plugin.cpp
SOURCES += Editor.cpp
SOURCES += MainControllerVST.cpp
SOURCES += NinjamRoomWindowVST.cpp
SOURCES += MainWindowVST.cpp
SOURCES += KeyboardHook.cpp
SOURCES += VstPreferencesDialog.cpp
SOURCES += $$VST_SDK_PATH/public.sdk/source/vst2.x/audioeffectx.cpp
SOURCES += $$VST_SDK_PATH/public.sdk/source/vst2.x/audioeffect.cpp
SOURCES += recorder/JamRecorder.cpp
SOURCES += recorder/ReaperProjectGenerator.cpp
SOURCES += loginserver/LoginService.cpp
SOURCES += loginserver/JsonUtils.cpp
SOURCES += ninjam/protocol/ServerMessages.cpp
SOURCES += ninjam/protocol/ClientMessages.cpp
SOURCES += ninjam/protocol/ServerMessageParser.cpp
SOURCES += ninjam/Server.cpp
SOURCES += audio/Resampler.cpp
SOURCES += audio/vorbis/VorbisDecoder.cpp
SOURCES += audio/samplesbufferrecorder.cpp
SOURCES += audio/vorbis/VorbisEncoder.cpp
SOURCES += persistence/Settings.cpp
SOURCES += geo/WebIpToLocationResolver.cpp
SOURCES += audio/core/PluginDescriptor.cpp


win32 {
    message("Windows VST build")

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
