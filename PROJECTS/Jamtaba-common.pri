QT += core gui network widgets concurrent multimedia multimediawidgets

CONFIG += c++11

mac:LIBS += -dead_strip

ROOT_PATH = "../.."
SOURCE_PATH = "$$ROOT_PATH/src"
VST_SDK_PATH = "$$PWD/../VST_SDK"

win32 {
    CONFIG(release, debug|release) {
        COMMON_PATH = ../Common/release
    } else {
        COMMON_PATH = ../Common/debug
    }
} else {
    COMMON_PATH = ../Common
}

LIBS += -L$${COMMON_PATH} -lCommon
INCLUDEPATH += $$shadowed($$PWD)/Common #including ui_xxx generated files by common lib

INCLUDEPATH += $$SOURCE_PATH/Common
INCLUDEPATH += $$SOURCE_PATH/Common/gui
INCLUDEPATH += $$SOURCE_PATH/Common/gui/widgets
INCLUDEPATH += $$SOURCE_PATH/Common/gui/chords
INCLUDEPATH += $$SOURCE_PATH/Common/gui/chat
INCLUDEPATH += $$SOURCE_PATH/Common/gui/screensaver

INCLUDEPATH += $$SOURCE_PATH/Libs/RtMidi

INCLUDEPATH += $$ROOT_PATH/libs/includes/ogg
INCLUDEPATH += $$ROOT_PATH/libs/includes/vorbis
INCLUDEPATH += $$ROOT_PATH/libs/includes/minimp3
INCLUDEPATH += $$ROOT_PATH/libs/includes/ffmpeg
win32:INCLUDEPATH += $$ROOT_PATH/libs/includes/stackwalker

VPATH       += $$SOURCE_PATH/Common
VPATH       += $$SOURCE_PATH

DEFINES += VST_FORCE_DEPRECATED=0   #enable VST 2.3 features
DEFINES += OV_EXCLUDE_STATIC_CALLBACKS  #avoid ogg static callback warnings

linux{ #avoid errors in VST SDK when compiling in Linux
    DEFINES += __cdecl=""
    QMAKE_CXXFLAGS += -D__LINUX_ALSA__
}

macx{
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++ -D__MACOSX_CORE__
    LIBS += -mmacosx-version-min=10.7 -stdlib=libc++
}

win32 {
    QMAKE_LFLAGS_RELEASE += /DEBUG # releasing with debug symbols
    QMAKE_CXXFLAGS += -D__WINDOWS_MM__
}

PRECOMPILED_HEADER += PreCompiledHeaders.h

RESOURCES += ../resources/jamtaba.qrc

DISTFILES += $$PWD/uncrustify.cfg  #this is the file used by the Beautifier QtCreator plugin (using the Uncrustify tool)
