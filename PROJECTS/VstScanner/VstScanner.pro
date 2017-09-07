QT += core
QT -= gui

TARGET = VstScanner
CONFIG -= app_bundle #in MAC create just a binary, not a complete bundle
CONFIG += c++11
DEFINES += VST_FORCE_DEPRECATED=0 #enable VST 2.3 features

linux{
    DEFINES += __cdecl="" #avoid tons of errors in VST_SDK in linux
}

# the VstScanner executable is generated in the Standalone folder
macx:DESTDIR = $$OUT_PWD/../Standalone/Jamtaba2.app/Contents/MacOS
linux:DESTDIR = $$OUT_PWD/../Standalone
win32{
    CONFIG(debug, debug|release) {
        DESTDIR = $$OUT_PWD/../Standalone/debug
    } else {
        DESTDIR = $$OUT_PWD/../Standalone/release
    }
}
message("Generating VstScanner executable in" $$DESTDIR)

TEMPLATE = app

ROOT_PATH = "../.."
SOURCE_PATH = $$ROOT_PATH/src

INCLUDEPATH += $$SOURCE_PATH/Common
INCLUDEPATH += $$SOURCE_PATH/Scanners
INCLUDEPATH += $$ROOT_PATH/VST_SDK/pluginterfaces/vst2.x

VPATH       += $$SOURCE_PATH/Common
VPATH       += $$SOURCE_PATH/Scanners

HEADERS += vst/VstHost.h
HEADERS += vst/Utils.h
HEADERS += VstScanner/VstPluginScanner.h
HEADERS += BaseScanner.h

SOURCES += VstScanner/main.cpp
SOURCES += BaseScanner.cpp
SOURCES += VstScanner/VstPluginScanner.cpp
SOURCES += vst/VstHost.cpp
SOURCES += vst/VstLoader.cpp
SOURCES += vst/Utils.cpp
SOURCES += audio/core/PluginDescriptor.cpp
SOURCES += midi/MidiMessage.cpp
SOURCES += log/logging.cpp


#including the correct implementation for VstPluginChecker
INCLUDEPATH += $$SOURCE_PATH/Standalone/vst #to allow a simple '#include "VstPluginChecker.h"' in the code
win32:SOURCES += $$SOURCE_PATH/Standalone/vst/WindowsVstPluginChecker.cpp
macx:OBJECTIVE_SOURCES  += $$SOURCE_PATH/Standalone/vst/MacVstPluginChecker.mm
linux:SOURCES += $$SOURCE_PATH/Standalone/vst/LinuxVstPluginChecker.cpp

win32{

    win32-msvc*{#all msvc compilers
        #windows XP support
        QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01 /SUBSYSTEM:CONSOLE,5.01


        CONFIG(release, debug|release) {
            #ltcg - http://blogs.msdn.com/b/vcblog/archive/2009/02/24/quick-tips-on-using-whole-program-optimization.aspx
            QMAKE_CXXFLAGS_RELEASE +=  -GL -Gy -Gw
            QMAKE_LFLAGS_RELEASE += /LTCG
        }
    }

    LIBS +=  -lwinmm -lole32 -lws2_32 -lAdvapi32 -lUser32
    RC_FILE = ../Jamtaba2.rc #windows icon
}


macx{
    message("VstScanner Mac build")

    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
    LIBS+= -dead_strip

    #mac osx doc icon
    ICON = Jamtaba.icns
    CONFIG += console  #in windows we need the /SUBSYSTEM:CONSOLE,5.01 to work in Win XP
}
