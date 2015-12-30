QT += core widgets
QT -= gui

TARGET = VstScanner
CONFIG -= app_bundle #in MAC create just a binary, not a complete bundle

DEFINES += VST_FORCE_DEPRECATED=0 #enable VST 2.3 features

TEMPLATE = app

ROOT_PATH = "../.."
SOURCE_PATH = "$$ROOT_PATH/src"

INCLUDEPATH += $$SOURCE_PATH/Common
INCLUDEPATH += $$SOURCE_PATH/VstScanner
INCLUDEPATH += $$ROOT_PATH/../VST_SDK/pluginterfaces/vst2.x

VPATH       += $$SOURCE_PATH/Common
VPATH       += $$SOURCE_PATH/VstScanner

HEADERS += vst/VstHost.h
HEADERS += VstPluginScanner.h

SOURCES += main.cpp
SOURCES += vst/VstHost.cpp
SOURCES += vst/VstLoader.cpp
SOURCES += audio/core/PluginDescriptor.cpp
SOURCES += log/logging.cpp
SOURCES += VstPluginScanner.cpp
win32:SOURCES += WindowsVstPluginScanner.cpp
macx:SOURCES += MacVstPluginScanner.cpp

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
    #mac osx doc icon
    ICON = Jamtaba.icns
    CONFIG += console  #in windows we need the /SUBSYSTEM:CONSOLE,5.01 to work in Win XP
}
