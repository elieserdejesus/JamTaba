!include( ../Jamtaba-common.pri ) {
    error( "Couldn't find the Jamtaba-common.pri file!" )
}

#CONFIG += qtwinmigrate-uselib
!include(../qtwinmigrate/src/qtwinmigrate.pri) {
    error( "Couldn't find the qtwinmigrate common.pri file!" )
}

QT       += core gui network widgets concurrent multimedia multimediawidgets

QTPLUGIN += dsengine # necessary to use QCamera inside VST plugin

TARGET = "JamtabaVST2"  #using this name (with a '2' suffix) to match the previons JTBA versions and avoid duplicated plugin in used machines
TEMPLATE = lib
CONFIG += shared

win32-msvc* {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    #RC_FILE = vstdll.rc

    #supressing warning about missing .pdb files
    QMAKE_LFLAGS += /ignore:4099
}

INCLUDEPATH += $$SOURCE_PATH/Common
INCLUDEPATH += $$SOURCE_PATH/Plugins
INCLUDEPATH += $$SOURCE_PATH/Plugins/VST

INCLUDEPATH += $$ROOT_PATH/libs/includes/ogg
INCLUDEPATH += $$ROOT_PATH/libs/includes/vorbis
INCLUDEPATH += $$ROOT_PATH/libs/includes/minimp3
INCLUDEPATH += $$ROOT_PATH/libs/includes/ffmpeg

win32{
    INCLUDEPATH += "$$VST_SDK_PATH/"
    INCLUDEPATH += "$$VST_SDK_PATH/pluginterfaces/vst2.x/"
    INCLUDEPATH += "$$VST_SDK_PATH/public.sdk/source/vst2.x"
}

VPATH += $$SOURCE_PATH/Common
VPATH += $$SOURCE_PATH/Plugins/
VPATH += $$SOURCE_PATH/Plugins/VST

DEPENDPATH +=  $$ROOT_PATH/libs/includes/ogg
DEPENDPATH +=  $$ROOT_PATH/libs/includes/vorbis
DEPENDPATH +=  $$ROOT_PATH/libs/includes/minimp3

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
SOURCES += $$VST_SDK_PATH/public.sdk/source/vst2.x/audioeffectx.cpp
SOURCES += $$VST_SDK_PATH/public.sdk/source/vst2.x/audioeffect.cpp

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
    CONFIG(release, debug|release):   LIBS += -L$(QTDIR)\plugins\mediaservice\ -ldsengine # necessary to use QCamera

    #debug platform libs
    CONFIG(debug, debug|release): LIBS += -lQt5PlatformSupportd #link windows platform statically
    CONFIG(debug, debug|release): LIBS += -L$(QTDIR)\plugins\platforms\ -lqwindowsd #link windows platform statically
    CONFIG(debug, debug|release): LIBS += -L$(QTDIR)\plugins\mediaservice\ -ldsengined # necessary to use QCamera
    #++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/$$LIBS_PATH -lminimp3 -lvorbisfile -lvorbis -logg -lavcodec -lavutil -lavformat -lswscale -lswresample
    else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/$$LIBS_PATH/ -lminimp3d -lvorbisfiled -lvorbisd -loggd -lavcodecd -lavutild -lavformatd -lswscaled -lswresampled

    CONFIG(release, debug|release) {
      #ltcg - http://blogs.msdn.com/b/vcblog/archive/2009/02/24/quick-tips-on-using-whole-program-optimization.aspx
      QMAKE_CXXFLAGS_RELEASE +=  -GL
      QMAKE_LFLAGS_RELEASE += /LTCG
    }

    QMAKE_LFLAGS += "/NODEFAULTLIB:libcmt"
}
